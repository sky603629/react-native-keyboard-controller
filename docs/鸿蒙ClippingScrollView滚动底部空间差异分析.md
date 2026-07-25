# 鸿蒙 ClippingScrollView 滚动底部空间差异分析

## 对应上游 commit

- 上游顺序：`108 / 155`
- 上游 commit：`36abe0b8b3`
- 标题：`feat: add ClippingScrollView component (#1289)`
- 相关后续 commit：`109 / 155` `167a000a9c`，`feat: add ScrollViewWithBottomPadding component (#1294)`

## 上游功能要解决什么问题

`ClippingScrollView` 是上游为 Android 新增的原生 decorator 组件。它的目标是补齐 Android `ScrollView` 缺少稳定 `contentInset.bottom` 语义的问题，让 `KeyboardAwareScrollView` / 后续 ChatKit 可以不用额外插入一个底部占位 View，也能获得稳定的底部可滚动空白。

上游 JS 侧导出 `ClippingScrollView`，在 Android 上渲染原生 `ClippingScrollViewDecoratorView`，在 iOS 上退化为直接返回 children。原因是 iOS 原生 `ScrollView` 已经支持 `contentInset.bottom`，不需要这个 decorator。

## Android 是怎么实现的

Android 原生实现包装第一个 child `ScrollView`：

1. `ClippingScrollViewDecoratorView` 作为 wrapper。
2. 找到 `getChildAt(0) as? ScrollView`。
3. 设置目标 ScrollView：
   - `clipToPadding = false`
   - `setPadding(left, top, right, contentInsetBottom.px)`

Android 的关键点是：`ScrollView` 的 padding 会参与可滚动边界计算；`clipToPadding=false` 让 padding 区域内的内容/空白可以在视口中稳定显示。因此底部 inset 不只是“拖拽时能看到”，而是会成为松手后仍可停留的合法 scroll range。

## iOS 为什么不需要这个 decorator

iOS 走的是系统 `ScrollView.contentInset.bottom`。这个 inset 本身就是 iOS ScrollView 的正式内容避让语义，会影响可滚动范围和滚动指示器行为。

所以 iOS 不需要通过 wrapper 去改 padding，也不需要 `clipToPadding` 类似能力。上游 JS 里 iOS 直接返回 children，是合理的。

## Harmony 试验实现

测试工程按 Android decorator 路线试过真实 Harmony native 组件，不是空 fallback：

1. 新增 Harmony `ClippingScrollViewDecoratorView` component descriptor / JSI binder / component instance。
2. 在 component instance 中通过 RNOH `ComponentInstance` API 查找 child：
   - `getChildren()`
   - `getComponentName()`
   - `getLocalRootArkUINode()`
3. 找到第一个 `componentName == "ScrollView"` 的 child。
4. 对该 child 的 ArkUI root node 设置：
   - `setClip(...)`
   - `setPadding(0, 0, 0, contentInsetBottom)`

RNOH 侧确认 `ScrollViewComponentInstance` 的本地根节点是 `ScrollNode`，内部真正承载内容的是私有 `m_contentContainerNode`。

## 实测现象

测试工程 demo 设置 `contentInsetBottom=240` 后，native 日志确认 decorator 已应用：

```text
native.decorate.applied ... bottom=240 clip=true paddingBottom=240
```

JS 侧滚动日志显示：

```text
contentHeight=806.15
layoutHeight=260
合法 maxY = 806.15 - 260 = 546.15
```

拖拽时可以拉到：

```text
endDrag y=734.77 distanceToEnd=-188.62
momentumEnd y=786.77 distanceToEnd=-240.62
```

随后松手后回到：

```text
scroll y=546.15 distanceToEnd=0
```

这说明 Harmony 上 `ScrollNode` root padding 确实让拖拽阶段出现了约 `240` 的底部视觉/弹性区域，但它没有进入 RNOH/RN 认为的 `contentSize.height`。原生 Scroll 最终仍按 `contentSize.height - layoutHeight` 把 offset 拉回合法边界。

## 为什么会出现“拖拽生效但松手回弹”

Harmony 这里存在两套距离：

| 距离类型 | 现象 | 来源 |
| --- | --- | --- |
| 拖拽中的视觉距离 | 可以继续往下拉，看见底部空白 | ArkUI `ScrollNode` root padding / overscroll 区域 |
| 松手后的合法滚动距离 | 回弹到 `contentSize.height - layoutHeight` | RNOH ScrollView 的真实 content size |

Android 的 `ScrollView.setPadding + clipToPadding=false` 会把 bottom padding 变成稳定可停留区域；Harmony 的 `ArkUINode::setPadding` 作用在 `ScrollNode` root 上，只改变拖拽/显示阶段的空白，不改变 RNOH `ScrollViewComponentInstance` 内部 content container 的高度。

## 为什么不继续用 `setClip(false)`

试验中 `setClip(false)` 会导致 ScrollView 内容越出自身区域，甚至盖到上方 plain ScrollView 区域。它不是 Android `clipToPadding=false` 的等价语义。

因此即使保留试验，也只能使用 `clip=true` 避免越界。但 `clip=true + root padding` 只能做到“不越界、拖拽时能看到底部空白”，不能做到“松手后稳定停留”。

## 为什么不改最后一个子节点 margin

也试过不改 Scroll root padding，而是给 ScrollView 的最后一个 React child 设置 bottom margin。实测底部增高区域不生效。

原因是 RNOH `ScrollViewComponentInstance` 的真实内容容器是内部私有 `m_contentContainerNode`，外部 decorator 能看到 React child component，但不能可靠修改内部 content container 的布局高度。给最后一个业务 child 改 ArkUI margin 既不等价于 content inset，也会覆盖业务自身 margin，不能作为库能力同步。

## 当前结论

在“不修改 RNOH/RN 框架代码”的硬约束下，Harmony 不能完整支持上游第 `108 / 155` 的 `ClippingScrollView` 语义。

原因不是没有 native component 能力，也不是 decorator 找不到 ScrollView，而是 Harmony/RNOH 当前没有公开接口让外部库把 bottom inset 写入 `ScrollViewComponentInstance` 的真实 content container / content size。

因此第 `108 / 155` 不能按 Android decorator 方案同步进主仓。第 `109 / 155` 的 `ScrollViewWithBottomPadding` 依赖第 108 提供稳定 `contentInsetBottom`，也不能单独同步。

## 后续可行方向

如果未来要完整支持，需要 RNOH/RN 层提供其中一种能力：

1. RNOH ScrollView 原生支持 `contentInset.bottom`，并让它进入 `contentSize` / scroll range。
2. RNOH `ScrollViewComponentInstance` 暴露安全 API，让外部 native component 修改内部 content container 的 bottom padding。
3. 在 RNOH core 中实现等价 Android `clipToPadding=false + paddingBottom` 的稳定 scroll range 语义。

在这些能力出现前，本库不应导出一个看似支持但松手回弹的 `ClippingScrollView`，避免后续 `KeyboardAwareScrollView` / ChatKit 基于错误假设继续叠加问题。
