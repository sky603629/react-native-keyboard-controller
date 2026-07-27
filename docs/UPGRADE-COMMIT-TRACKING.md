# 上游 `1.16.5..1.21.8` Commit 同步台账

> 用途：记录每个上游 commit 是否已经分析、是否已经同步代码、如果修改则修改了什么功能和内容、同步/不同步原因、是否需要测试工程验证。
>
> 规则：以后每处理一个 commit，都先更新本文；不要只凭聊天记录判断进度。

## 接手检查清单

新会话接手时先按下面顺序确认，不要直接开始改代码：

1. `git status --short`：当前仓可能有未跟踪历史 docs，未确认前不要删除或纳入提交。
2. `git branch --show-current` / `git remote -v`：当前工作分支应为 `ups`，推送目标是 `origin/ups`。
3. 读取 `UPGRADE-SOURCE-COMMIT-CHECKLIST-155.md` 的“当前位置”：当前已分析完第 `155 / 155`；第 `123-126 / 155` `KeyboardAvoidingView.automaticOffset`、类型修复、`viewPositionInWindow` 与 KASV 顶边自动检测已通过测试工程真机验证并回写主仓；第 `130 / 155` 的测量失败降级已按上游直接同步、未单独验证。第 `127 / 155` `KeyboardToolbar.Group` 已完成测试工程验证并回写主仓；第 `117 / 155` 基础 `KeyboardChatScrollView` 已回写上游 JS，但 Harmony spacer 验证结论已撤销，当前因原生 contentInset/稳定滚动范围缺失标记为不能完全做；其他状态以总清单逐笔结论为准；后续没有未分析源码 commit。
4. 查询上游真实 diff：

```powershell
$up = "E:\Devsoftware\kbc\react-native-keyboard-controller"
git -C $up show --stat <hash>
git -C $up show --patch --find-renames <hash>
```

5. 映射 Harmony 现有实现，至少检查 `src`、`harmony\keyboard_controller\src\main\ets`、`harmony\keyboard_controller\src\main\cpp`，平台 only 也不能直接跳过。
6. 若需要适配，先改测试工程，不先改主仓；用户验证通过后再回写主仓、更新台账、提交并推送。

## 适配硬约束

- 进度分母固定为 `155` 笔源码相关 commit，不能再用旧的 `83` 笔清单作为进度。
- 一笔 commit 的结论必须来自真实 diff 和 Harmony 对应链路分析；不能只按标题、平台目录或“看起来不相关”判断。
- Android-only / iOS-only 必须写清楚 Harmony 是否存在同类问题；没有同类问题时也要列出对应证据，例如无 UIKit delegate、无 Android ViewManager、无 `WindowDimensionsListener`、无 `inputAccessoryView` 等。
- JS API/types/hooks 侧尽量与上游一致；不要增加 Harmony-only public 字段，不改上游属性名。关键属性/API 名在中文说明中保持英文。
- 平台语义优先级：Harmony 适配优先判断能否按 iOS 语义实现；如果 Harmony 原生能力/事件时序/系统 API 无法等价 iOS，再按 Android/非 iOS 路线实现或记录不支持。不能仅凭上游条件写成 `OS !== "ios"` 就机械地把 Harmony 归入 Android，必须说明为什么不能走 iOS。
- 不修改 RNOH/RN 框架代码完成本库能力。如果测试时发现只有改框架才生效，回退测试改动并记录为“Harmony 当前无可用暴露接口”。
- Harmony 缺公开系统 API 时，不新增误导性的原生空实现；JS fallback 只能在符合历史策略且不会冒充完整能力时使用。
- 测试工程 JS 改动通常要同步 `src`、`lib/module`、`lib/commonjs`，必要时同步 `lib/typescript`；只改 `.tsx` 后无效时，先查实际 bundle 解析路径。
- demo 只补当前 commit 的最小验证入口；临时 demo 或 unsupported 能力验证后要回退，主仓提交默认不包含测试工程 demo。
- 用户不要求构建时不要主动构建；本任务模式是“我们分析并改测试工程，用户自己编译测试”。
- 用户验证通过后才提交主仓；提交必须同时更新总清单和本台账，说明修改了什么功能、内容和实际效果。
- 默认按 5 笔一批分析；若遇到需要 JS/原生验证的关键改动，停下来让用户测，不要越过验证点继续处理后续功能。

## 当前关键状态

- 当前待测试候选：第 `28 / 155`、`75 / 155` 可做 Harmony `windowDidResize` 事件链路试验验证；第 `139 / 155` 可补 `getConstants().keyboardBorderRadius=0` 和 `KEYBOARD_BORDER_RADIUS`；第 `147 / 155` 可验证 Toolbar fixed bounds。第 `123-126 / 155` 已在测试工程完成普通 RN/Reanimated View tag 的 ShadowTree 窗口测量、三种 behavior、offset 和 KASV 固定 Header 顶边恢复验证并回写主仓，第 `130 / 155` 失败降级已直接同步，第 `132 / 155` 自适应父容器布局修复已验证并回写。此前 ComponentInstance -> ArkUI node 的失败只证明该映射路线不可用，不代表 Fabric tag 无法测量。
- 最近已推送代码提交：第 `93`、`95 / 155` 已提交为 `a31f12f8 同步 useAnimatedKeyboard 兼容接口`；第 `96`、`100 / 155` 已提交为 `fd5f3e1a 同步 KASV deps 修复和 Provider 类型导出`；第 `101 / 155` 已提交为 `85dc6ee5 同步 KAV 非 iOS 位移条件`；第 `107 / 155` 已提交为 `ff355e62 同步 KASV useCombinedRef 重构`。本地 `ups` 另有第 `127 / 155` `KeyboardToolbar.Group` 和第 `117 / 155` 基础 `KeyboardChatScrollView` 两笔提交尚未推送；第 117 正通过新增纠正提交恢复上游 JS 原样实现。
- 最近已验证并回写：第 `76 / 155` `ad76c6ab56` compound `KeyboardToolbar`，以及第 `79 / 155` `692494f11d` `dismiss({ animated })` API 形状；Harmony 的 `animated=false` 当前为接口兼容降级。
- 下一笔待处理：无，`1.16.5..1.21.8` 源码相关 commit 已全量审计完毕。
- 已提前同步：第 `91 / 155` `852fa4a223`，动态 `bottomOffset` over-scrolling；到第 91 时只需复核，不推进重复实现。

## 已知不可直接同步 / 易误判点

- `a28dbec565` KASV full screen input support：Harmony selection 坐标语义和 Android/iOS caret 局部坐标不一致，缺少公开替代 API，不能同步。
- `keyboardAppearance`：RNOH 框架未暴露 `keyboardAppearance` native prop，本库不改框架，记录为无可用接口。
- `windowDidResize`：经复查不应再视为缺原生窗口事件。RNOH 已提供 `WINDOW_SIZE_CHANGE` 生命周期事件；本库可桥接为 `KeyboardController::windowDidResize`，第 `28`、`75` 改为可实现待验证。
- `ClippingScrollView`：第 `108 / 155` 测试工程先后验证 root padding、`NODE_SCROLL_CONTENT_END_OFFSET` 和内部 content container margin。内部 margin 可让 ArkUI 精确停在加入 bottom inset 后的终点，但 RNOH 随后在 `ScrollViewComponentInstance::onFinalizeUpdates() -> updateOffsetAfterChildChange()` 中按私有 Fabric `m_contentSize` 再次夹回原终点；外部库无法同步该状态。JS spacer 虽可稳定停留，却会改变布局和 `contentSize`，不符合 Clipping 的无布局更新语义。因此正式记录为“因 RNOH 框架能力缺失不能完全做”，不修改主仓实现。详见 `docs/鸿蒙ClippingScrollView滚动底部空间差异分析.md`。
- `KeyboardChatScrollView`：统一结论为“因 Harmony/RNOH 无法正确应用并反馈动态 `contentInset`，组件专属属性暂时均不能实现并对外宣称支持”。第 `117 / 155` 的公共 API、组件结构和内部 JS 已恢复为上游原样实现，不再保留 Harmony spacer、强制 `inverted=false`、内容高度扣减或 `useScrollViewOffset` 替代逻辑。当前缺口不仅是传入 `contentInset.top/bottom`：原生还必须把 inset 纳入合法滚动范围、`contentSize`/offset 边界和动态更新，并让 JS 读到与实际布局一致的状态；Harmony 当前均不能保证。因此 `keyboardLiftBehavior` 四种模式、`offset`、`inverted`、`freeze`、`extraContentPadding`、`blankSpace`、`onContentInsetChange`、`onEndVisible` 以及相关组合能力统一暂记为 No。`freeze` 和 `onEndVisible` 的独立 JS 判断可以执行，但它们消费的是未正确应用 inset 后的错误/不完整滚动状态，不能据此把组件属性标成支持。后续 JS 仍按上游顺序同步，不能再用 Harmony JS fallback 掩盖原生缺口；只有 RNOH 补齐稳定 contentInset 闭环后才重新验证这些属性。
- `KeyboardAvoidingView automaticOffset`：第 `123 / 155` public prop、第 `124 / 155` 类型 union 修复和第 `125 / 155` 内部 `viewPositionInWindow` 已作为一个完整功能同步。Harmony 不再尝试把 Fabric tag 映射到 ComponentInstance/ArkUI node，而是在 C++ TurboModule 中从 `UIManagerBinding` 查找 ShadowNode，取得当前 ShadowTree revision 后调用 Fabric `dom::measureInWindow`。该路径能覆盖 layout-only View 和 `position` 外层 View；找不到 tag/revision 时 Promise reject，JS 按上游回退 `onLayout`。测试工程真机确认 cold start、behavior 切换、`padding/height/position` 与 `keyboardVerticalOffset=0/50/100` 计算正确，判定为已实现。
- `KeyboardAwareScrollView` top border auto detect：第 `126 / 155` 已复用第 125 的 Fabric tag 窗口测量能力完成真机验证并回写主仓；固定 Header 场景中，被 KASV 顶部遮挡但仍位于窗口 `y>0` 的输入框能够重新恢复可见。第 `130 / 155` 已按上游增加 `try/catch`，为 lazy pager/卸载竞态下的测量 reject 提供静默降级，按用户要求未单独验证。
- `KeyboardToolbar.Group`：第 `127 / 155` 已按上游语义落地，不是单纯 JS wrapper。Harmony 使用 marker native component 和 C++ component tree 遍历实现 group 边界；导航扫描跳过 Group，真实焦点扫描仍进入 Group，保证 Prev/Next、`focusDidSet`、布局和 selection 事件都能覆盖组内输入。测试工程已验证构建链路并保留独立 demo。
- `OverKeyboardView`：专项原型已在真机确认 Dialog / 应用子窗口低于系统输入法，统一标记为“因系统能力缺失不能完全做”；不补空实现，详见 `鸿蒙OverKeyboardView无法等价实现说明.md`。
- iOS `KeyboardExtender` 后续多笔 accessory / glass / `keyboardLayoutGuide` 修复：Harmony 当前按 Android 同级 JS polyfill 实现，不具备 iOS 系统级 accessory 语义。
- Android listener / StatusBar / EdgeToEdge 修复不能泛化套到 Harmony；必须基于 Harmony 实际 API 和复现。

## 状态定义

| 状态 | 含义 |
| --- | --- |
| 未分析 | 还没有看 diff。 |
| 已分析-待测试 | 已看 diff，判断需要先同步到测试工程验证。 |
| 已分析-不需同步 | 已看 diff，对 Harmony 库无有效代码影响，或属于纯维护/平台专用。 |
| 测试工程已同步 | 已同步到测试工程，但还没有确认是否进入代码仓。 |
| 代码仓已提交 | 已提交到 `E:\Devsoftware\kbc\finalfeat\react-native-keyboard-controller`。 |
| 代码仓已推送 | 已推送到远端分支。 |
| 因系统能力缺失不能完全做 | 已确认 Harmony 缺少公开 API，只能降级或记录。 |

## 当前远端分支

```text
origin/ups
```

## 当前进度口径

当前升级进度以本地上游仓实际统计的源码相关 commit 为准：

```powershell
git -C "E:\Devsoftware\kbc\react-native-keyboard-controller" rev-list --count 1.16.5..1.21.8 -- src android ios cpp
```

统计结果为 `155` 笔。完整顺序和当前状态见 `UPGRADE-SOURCE-COMMIT-CHECKLIST-155.md`。

`UPGRADE-COMMIT-ORDER-PLAN.md` 中列出的 `83` 笔是早期关键实施清单，不能作为当前整体升级进度分母；本文后面的 `72` 笔补充审计池也是历史拆分结果，后续只作为审计提示，不作为主清单。

截至第 `155 / 155` 笔源码相关 commit 已按时间顺序完成证据审计，最后分析的是 `38076a9df8`。第 `93`、`95 / 155`、第 `96`、`100 / 155`、第 `101 / 155`、第 `107 / 155` 已完成测试工程验证、回写主仓并推送到 `origin/ups`；第 `123-127 / 155` 中可实现的 automaticOffset、窗口测量、KASV 顶边检测与 `KeyboardToolbar.Group` 已完成测试工程验证并回写主仓，第 `130 / 155` 测量失败降级已直接同步，第 `132 / 155` 自适应父容器布局修复已验证并回写，以上本地提交尚未推送。第 `117 / 155` 基础 `KeyboardChatScrollView` 已回写上游 JS，但本轮已撤销 Harmony JS workaround 和相应“验证通过”结论，功能因原生 contentInset/稳定滚动范围缺失不能完全做。第 `28 / 155`、`75 / 155` 是 `windowDidResize` 可实现待验证候选；第 `139 / 155` 是新增 `keyboardBorderRadius` 常量接口的待测试候选；第 `147 / 155` 是 Toolbar touch bounds 功能修复候选。后续没有未分析源码 commit。

补充说明：为修复已验证的动态 `bottomOffset` 过度滚动问题，本轮按用户确认提前同步第 `91 / 155` 笔 `852fa4a223`，但不推进主线顺序游标。

当前已经推送到 `ups` 的本地同步提交：

| 本仓 commit | 内容 |
| --- | --- |
| `1ed7243` | 同步上游 `2a923a6962`：新增 `src/architecture.ts`，`OverKeyboardView` 使用 `IS_FABRIC`。 |
| `5bffbcf` | 同步上游 `b8a4de91fd` 的源码类型部分：`KeyboardToolbar` 使用 `React.JSX.Element`。 |
| `06e1abe` | 同步上游 `2b21634df9`：`KeyboardAvoidingView` shared values 改为在 `useLayoutEffect` 初始化。 |
| `0937de2` | 同步上游 `8eb6a7be14` + `5b65344e6b` 并补 Harmony ETS 事件过滤修复：`KeyboardController.dismiss` 直接作为事件 handler 不崩溃，启动后键盘未显示过时 Promise 不挂起，且 `removeListeners` 后 dismiss didHide 仍可送达 JS Promise 监听。 |
| `8a425ed` | 同步上游 `efd038cba2` + `9080f313be` + `cb8161cf00`：完善 `KeyboardToolbar` Done 隐藏、内容区占满和 `insets` 安全区边距。 |
| `f75a60a` | 同步上游 `d379ff5f21`：`KeyboardAvoidingView` 非 `height` 行为在 autoFocus/首帧布局变化后刷新 frame。 |
| `ce10428` | 同步上游 `b27ed8a284` + `0c6ae77362`：新增并修正 `useKeyboardState`，`KeyboardController.state()` 与 hook 聚合状态解耦。 |
| `3b7f1c7` | 修复鸿蒙 `KeyboardStickyView` 的 `onKeyboardMoveStart` 延迟：Harmony 在 start 阶段刷新 Reanimated shared values，保持原本瞬时切换但不等到 end。 |
| `c2a9838` | 同步上游 `9011a90a78` + `0071eb058d`：`KeyboardAwareScrollView enabled=false` 不再挂载底部 Reanimated padding view，并新增 `findNodeHandle` 平台封装。 |
| `17d373c` | 同步上游 `c5c00778f2` + `e431917472`，并提前同步 `852fa4a223`：`KeyboardAwareScrollView` 支持动态 `bottomOffset` 且避免过度滚动；`KeyboardAvoidingView behavior="height"` 避免写入 0/负高度。 |
| `0b03f32` | 已回退：曾尝试把上游 `331293a9cc` 扩展为 Harmony window keyboard listener 生命周期修复；测试发现会导致键盘事件异常，最终判断 Harmony 无 Android 同类问题，不同步该代码。 |
| `43f6631e` | 同步上游 `a57fa4b427` + `347fef35c0`：新增 Harmony `KeyboardBackgroundView` JS fallback 与 `KeyboardExtender` Android 同级别 polyfill；实现方式与 Android 一致，和 iOS 原生 accessory / 私有键盘背景材质能力不一致。 |
| `04da6bf4` | 同步上游 `60ec0ceab8` + `ecb3595085`：`useKeyboardState` 支持 selector；`KeyboardProvider` 移除 JS monkey-patch 深导入依赖，Harmony 保留现有原生 StatusBarManagerCompat 路径。 |
| `376f84d9` | 同步上游 `5ab201112c`：`KeyboardAwareScrollView` 改用 selection caret y 驱动多行输入滚动，加入 `lastSelection`、`clamp` 和 selection debounce；Harmony 原生侧已具备 selection 坐标事件，无需 C++/ETS 修改。 |
| `d7eba660` | 同步上游 `49979932c7` + `1c03e7b9cf`：新增 `KeyboardController.preload()` 和 `KeyboardProvider preload` 默认调用；Harmony 原生侧按 Android 策略做 no-op，避免真实拉起键盘。 |
| `08714c47` | 同步上游 `aff3cbe7d1`：`KeyboardAvoidingView enabled=false` 时直接返回空 animated style，完全关闭 `height/position/padding/translate-with-padding` 避让行为。 |
| 本批最新提交 | 同步上游 `ad76c6ab56` + `692494f11d`：新增 compound `KeyboardToolbar` API；`dismiss` 支持 `{ keepFocus, animated }` 参数形状。Harmony 对 `animated=false` 接口兼容但不能保证系统无动画隐藏。 |
| 本批状态 | 第 `123-126 / 155` 已完成 automaticOffset、普通 Fabric tag 窗口测量和 KASV 顶边检测验证并回写主仓；第 `127 / 155` 已完成 `KeyboardToolbar.Group` marker component、traversal 边界和深度焦点扫描验证并回写主仓，测试工程独立 demo 保留；第 `130 / 155` 测量失败降级已直接同步、未单独验证；第 `132 / 155` 父容器自适应布局修复已验证并回写。第 `28 / 155`、`75 / 155` 可验证 `windowDidResize`；第 `139 / 155`、`147 / 155` 仍是 Toolbar 待测试候选。第 `108 / 155` 已验证当前能力缺失不能完全做；第 `146 / 155` 已试验后撤销；其余暂不同步结论见总清单。 |

## Commit 台账

| 顺序 | 上游 commit | 日期 | 标题 | 分析状态 | 同步状态 | 修改功能 | 修改内容 | 原因 / 处理说明 | 测试工程验证 |
| ---: | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | `2a923a6962` | 2025-02-25 | `refactor: use nativeFabricUIManager instead of IS_FABRIC to detect new architecture in JS (#832)` | 已分析 | 代码仓已推送 | JS 架构检测 / OverKeyboardView 渲染分支 | 新增 `src/architecture.ts`；`OverKeyboardView` 从 `global._IS_FABRIC` 改为引用 `IS_FABRIC`。 | 只影响 JS 架构检测。Harmony 当前没有 `OverKeyboardView` 原生实现，但该 commit 不要求实现 overlay 能力。 | 不需要新增 demo；只需保证打包不报错。 |
| 2 | `b8a4de91fd` | 2025-02-26 | `chore: react-native 0.78 (#812)` | 已分析 | 代码仓已推送 | KeyboardToolbar 类型 | `KeyboardToolbar` 中 `JSX.Element` -> `React.JSX.Element`。 | 上游大部分是 example / lockfile / RN 版本维护；当前鸿蒙仓已有自己的依赖基线，不能直接覆盖 `package.json`/`yarn.lock`。仅同步库源码有效改动。 | 不需要新增 demo；类型/构建验证即可。 |
| 3 | `2b21634df9` | 2025-02-27 | `fix: set shared values in layout effects (#838)` | 已分析 | 代码仓已推送 | KeyboardAvoidingView shared value 初始化 | 已同步测试工程 JS 三处并验证无明显问题；已回写代码仓 `src/components/KeyboardAvoidingView/hooks.ts`，本仓提交 `06e1abe` 推送到 `origin/ups`。把 `heightWhenOpened`、`height`、`progress`、`isClosed`、`padding` 初始值设为稳定默认值，并在 `useLayoutEffect` 中读取 `reanimated.height/progress` 后写入 shared values；保留 Harmony `onEnd` 更新 `heightWhenOpened` 的既有分支。 | 属于 JS 行为修复，避免 KAV 初始化到事件 handler 绑定之间键盘关闭时状态滞后，也避免 render 阶段读取 Reanimated shared value。 | 已在测试工程验证 KAV 修改无明显问题；后续回归继续覆盖首次渲染、键盘已打开进入页面、初始化期间键盘关闭、切换输入框是否抖动。 |
| 4 | `1714227edd` | 2025-02-27 | `fix: exclude StatusBar height from useWindowDimensions if StatusBar is not translucent (#837)` | 已分析-不需同步 | 未同步 | Harmony 窗口高度 / 状态栏同类问题审计 | 无测试工程代码修改。上游只改 Android `WindowDimensionListener.kt`，把 `content.height + content.marginTop` 改为 `content.height`。Harmony 当前键盘高度来自 `WindowAvoidArea.TYPE_KEYBOARD`，focused input 位置来自 ArkUI `GetLayoutPositionInWindow`，未发现同类 `height + marginTop` 计算路径。 | Harmony 没有对应 Kotlin `WindowDimensionListener`，且当前库内未发现把状态栏高度叠加到窗口高度的实现；不直接同步代码。 | 建议随 KAV demo 验证非透明状态栏场景：`KeyboardAvoidingView` 高度不应额外少/多一个状态栏高度。 |
| 5 | `3a366a3a3b` | 2025-02-27 | `chore: release 1.16.6 (#840)` | 已分析-不需同步 | 未同步 | 不涉及 Harmony 功能 | 无代码修改。上游只改 `package.json` 版本和 iOS example / FabricExample 的 `Podfile.lock`。 | release commit，不同步上游 npm 版本号；Harmony 包有自己的 `1.17.0-beta.2` 发布体系。 | 不需要。 |
| 6 | `8eb6a7be14` | 2025-03-06 | `fix: crash when pass dismiss as event handler (#845)` | 已分析 | 代码仓已推送 | KeyboardController.dismiss 旧签名容错 | 已同步测试工程 JS 三处和代码仓 `src/module.ts`：`dismiss` 参数从解构默认值改为 `options?.keepFocus ?? false`；测试工程 `KeyboardController` demo 增加 `dismiss direct handler` 按钮用于直接绑定验证；本仓提交 `0937de2` 已推送到 `origin/ups`。 | 避免 `onPress={KeyboardController.dismiss}` 这类直接作为事件回调时崩溃。注意这是旧签名修复，不是后续 `dismiss({ animated })`。 | 已确认 `dismiss direct handler` 可用；与 `5b65344e6b` 合并为同一笔 dismiss 稳定性修复。 |
| 7 | `41d798532d` | 2025-03-07 | `fix: selection coordinates in container with static height (#844)` | 已分析-不需同步 | 未同步 | Android multiline TextInput selection 坐标审计 | 无测试工程代码修改。上游只改 Android `EditText.kt` 选择坐标手算公式：文本溢出固定高度容器时忽略垂直 gravity，并用 `view.scrollY` 修正 cursor y。 | Harmony 当前 selection 事件从 ArkUI `NODE_TEXT_INPUT_CARET_OFFSET` / `NODE_TEXT_AREA_CARET_OFFSET` 读取系统 caret 坐标，未走 Android 的 baseline/gravity/scrollY 手算路径；同时系统没有公开 API 读取 selection start/end 两端完整矩形，当前实现仍按已记录降级策略两端复用 caret 坐标。暂不做代码同步。 | 需要补回归：多行固定高度/`maxHeight` 文本框、内容超过可视区域后滚动，检查 `onFocusedInputSelectionChanged` 的 `position` 正确，`y` 若系统返回则应随可视区域变化；完整 start/end 几何仍属降级。 |
| 8 | `18e550c898` | 2025-03-07 | `chore: release 1.16.7 (#846)` | 已分析-不需同步 | 未同步 | 不涉及 Harmony 功能 | 无代码修改。上游只改 `package.json` 版本和 iOS example / FabricExample 的 `Podfile.lock`。 | release commit，不同步上游 npm 版本号；Harmony 包有自己的发布体系。 | 不需要。 |
| 9 | `efd038cba2` | 2025-03-11 | `feat: ability to hide "Done" button in KeyboardToolbar (#849)` | 已分析 | 代码仓已推送 | KeyboardToolbar Done 按钮显示规则 | 已同步测试工程 `KeyboardToolbar` 源码、ESM/CJS 构建产物和 d.ts；已回写代码仓 `src/components/KeyboardToolbar/index.tsx`，本仓提交 `8a425ed` 推送到 `origin/ups`：`doneText` 默认值改为 `"Done"`，渲染时改为 `doneText && <Button...>`。测试 demo 增加 `hide Done` / `show Done`。 | 功能效果：`doneText=""` 时不再被 `|| "Done"` 兜底，Done 按钮会真正隐藏。纯 JS/布局改动，无原生侧对应修改。 | 已验证有效且正常。 |
| 10 | `5b65344e6b` | 2025-03-14 | `fix: pending dismiss if keyboard wasn't shown in app yet (#857)` | 已分析 | 代码仓已推送 | KeyboardController.dismiss Promise 初始状态 / Harmony 事件过滤 | 已同步测试工程 JS 三处和代码仓 `src/module.ts`：`isClosed` 初始值从 `false` 改成 `true`；测试工程 `KeyboardController` demo 增加 `dismiss promise` 按钮显示 `pending/resolved`。补充 Harmony ETS：`removeListeners` 不再清空 `eventListeners`，与测试工程 native 当前行为一致；本仓提交 `0937de2` 已推送到 `origin/ups`。 | 上游真实 diff 只改 JS：app 启动默认键盘应为关闭。Harmony 还存在本地等价问题：旧 ETS `removeListeners()` 会清空键盘事件白名单，导致之后即使 `dismiss()` 已通过 `stopInputSession()` 关闭键盘，`keyboardDidHide` 也被过滤，JS Promise 等不到 resolve。测试工程 native 中的 `dismiss(keepFocus, animated)` 属于另一个后续/本地能力线索，不混入本次 commit。 | 待补测：刚进页面不聚焦输入框直接点 `dismiss promise`，应从 `pending` 变 `resolved`；执行 add/remove listener 后再聚焦输入框点 `dismiss promise`，也应关闭并 resolve。 |
| 11 | `9080f313be` | 2025-03-17 | `fix: allow keyboard toolbar take all available space (#865)` | 已分析 | 代码仓已推送 | KeyboardToolbar 内容区布局 | 已同步测试工程 `KeyboardToolbar` 源码、ESM/CJS 构建产物和 d.ts；已回写代码仓 `src/components/KeyboardToolbar/index.tsx`，本仓提交 `8a425ed` 推送到 `origin/ups`：移除 toolbar 容器 `paddingHorizontal`，新增 `styles.arrows` 包裹前后箭头并给箭头区加 `paddingLeft`；Done 容器改为 `marginRight: 16, marginLeft: 8`。 | 功能效果：隐藏箭头或隐藏 Done 后，中间 `content` 能占用原本被容器 padding 固定预留的空间。纯 JS/布局改动，无原生侧对应修改。 | 已验证有效且正常。 |
| 12 | `cb8161cf00` | 2025-03-18 | `feat: insets prop for KeyboardToolbar (#866)` | 已分析 | 代码仓已推送 | KeyboardToolbar 安全区边距 | 已同步测试工程 `KeyboardToolbar` 源码、ESM/CJS 构建产物和 d.ts；已回写代码仓 `src/components/KeyboardToolbar/index.tsx`，本仓提交 `8a425ed` 推送到 `origin/ups`：新增 `insets?: { left: number; right: number }`，toolbarStyle 根据 `insets.left/right` 设置左右 padding。测试 demo 增加 `insets 32/32` / `insets 0/0`。 | 功能效果：业务可在横屏或安全区场景手动传入左右 inset，避免 Toolbar 内容贴到安全区域边缘。纯 JS prop/布局改动，无原生侧对应修改。 | 已验证有效且正常。 |
| 13 | `e1a4cc4e58` | 2025-03-19 | `chore: release 1.16.8 (#868)` | 已分析-不需同步 | 未同步 | 不涉及 Harmony 功能 | 无代码修改。上游只改 `package.json` 版本和 iOS example / FabricExample 的 `Podfile.lock`。 | release commit，不同步上游 npm 版本号；Harmony 包有自己的发布体系。 | 不需要。 |
| 14 | `03937a761c` | 2025-03-25 | `feat: OverKeyboardView with custom ShadowNode (#863)` | 因系统能力缺失不能完全做 | 未同步 | OverKeyboardView 原生 overlay / Fabric ShadowNode | 曾在测试工程实现 C++ descriptor / binder / instance 和 Native Dialog / 子窗口原型，后因真机确认窗口低于系统输入法而全部回退。 | 普通 Phone / Tablet 应用没有同时满足键盘上层、保持输入焦点和承载当前 RN Fabric children 的公开窗口 API；`TYPE_FLOAT` 受控且限 PC / 2in1，输入法浮窗类型仅输入法可用。 | 不保留误导性 demo；系统开放等价窗口能力后重新评估。 |
| 15 | `87e47e556f` | 2025-03-28 | `Update correct path of RNKCOverKeyboardViewComponentDescriptor.h in … (#886)` | 已分析-不需同步 | 未同步 | iOS Fabric header path 审计 | 无代码修改。上游只改 iOS `.mm` import 路径和 podspec `header_dir`，从 `<reactnativekeyboardcontroller/...>` 改为 `<react/renderer/components/reactnativekeyboardcontroller/...>`。 | 本仓无 `ios/`、无 `react-native-keyboard-controller.podspec`，Harmony C++ include 路径使用本地 codegen-harmony 头文件，不存在该 iOS header_dir 问题。 | 不需要。 |
| 16 | `d379ff5f21` | 2025-03-29 | `fix: KeyboardAvoidingView + autoFocus + native-stack (#880)` | 已分析 | 代码仓已推送 | KeyboardAvoidingView 初始 frame 更新规则 | 已同步测试工程 JS 三处：`src/components/KeyboardAvoidingView/index.tsx`、`lib/module/.../index.js`、`lib/commonjs/.../index.js`；已回写代码仓 `src/components/KeyboardAvoidingView/index.tsx`，本仓提交 `f75a60a` 推送到 `origin/ups`。`onLayoutWorklet` 改为当 `keyboard.isClosed`、`initialFrame` 为空或 `behavior !== "height"` 时刷新 `initialFrame`，依赖项增加 `behavior`；测试工程 KAV demo 增加 `autoFocus position` 按钮。 | 功能效果：没有新增接口/props，是内部行为能力增强。`position` / `padding` / `translate-with-padding` 这类非 `height` 行为，在 autoFocus 或导航首帧布局变化后继续刷新 frame，避免用旧 frame 计算键盘避让；`height` 行为保持打开键盘时不重写初始高度，避免高度越算越小。纯 JS/Reanimated 行为修复，无原生侧对应修改。 | 已验证 `autoFocus position` 可生效。 |
| 17 | `b27ed8a284` | 2025-04-01 | `feat: useKeyboardState (#894)` | 已分析 | 代码仓已推送 | 新增 `useKeyboardState` hook / `KeyboardController.state()` 默认状态 | 已同步代码仓 `src/hooks/index.ts`、新增 `src/hooks/useKeyboardState/index.ts`，并修改 `src/module.ts` / `src/types.ts`；已同步测试工程 `@react-native-ohos` 包源码和 ESM/CJS/d.ts 产物；测试 demo 新增 `Hook_useKeyboardState` 入口。本仓提交 `ce10428` 已推送到 `origin/ups`。 | 功能效果：业务可通过 `useKeyboardState()` 响应键盘显隐并获得 `height/duration/timestamp/target/type/appearance/isVisible`；`KeyboardController.state()` 从 nullable 改为始终返回默认键盘事件数据，避免调用方初始态必须判空。纯 JS hook/API 改动，依赖现有 Harmony `keyboardDidShow/keyboardDidHide` 事件回流，无新增原生接口。 | 已验证 `Hook_useKeyboardState` 没问题。 |
| 18 | `13dfb32c54` | 2025-04-04 | `fix: set delegate to all UITextView (#902)` | 已分析-不需同步 | 未同步 | iOS UITextView delegate 兼容审计 | 无测试工程代码修改。上游只改 iOS：把 `RCTUITextView+DelegateManager` 改为 `UITextView+DelegateManager`，并让 `FocusedInputObserver.swift` 对基础 `UITextView` 设置 force delegate，解决三方 UITextView 子类和 dynamic frameworks 下 `RCTUITextView` 内部类不可见问题。 | Harmony 没有 UIKit `UITextView` / `RCTUITextView` / Objective-C category / Swift delegate 链路；当前 focused input 事件来自 Harmony ArkTS/C++ 节点与 RNOH 事件，不存在“只对 RN 私有 TextView 子类设置 delegate”这条路径。本批不补原生空实现。 | 不需要单独 demo；后续 focused input 回归仍覆盖普通 TextInput、TextArea/多行、三方输入组件场景。 |
| 19 | `820fcb4b9d` | 2025-04-07 | `feat: KeyboardStickyView using plain Animated (#898)` | 已分析 | 未同步 | KeyboardStickyView 动画实现切换 | 已尝试同步到测试工程 `@react-native-ohos` 包源码和 ESM/CJS 产物，但测试观察到表现有问题；已按要求回退测试工程和代码仓 `KeyboardStickyView` 修改，保留 Reanimated 版本用于对比验证。 | 功能效果目标：减少 StickyView/Toolbar 对 Reanimated worklet 的依赖，规避上游记录的 RN/Reanimated 组合下影响其它组件行为的问题。当前 Harmony 测试表现需继续分析，暂不提交。纯 JS 动画实现替换，无原生侧对应修改。 | 待重新验证和分析问题原因后再同步。 |
| 20 | `0c6ae77362` | 2025-04-09 | `fix: useKeyboardState tweaks (#907)` | 已分析 | 代码仓已推送 | `useKeyboardState` 与 `state()` 语义修正 | 已与 `b27ed8a284` 合并同步到代码仓和测试工程：`KeyboardController.state()` 只返回 `KeyboardEventData`，不再包含 `isVisible`；`useKeyboardState()` 内部通过 `KeyboardController.isVisible()` 聚合出最终 `KeyboardState`。 | 功能效果：保持 imperative 的 `KeyboardController.state()` 和 reactive 的 `useKeyboardState()` 解耦；hook 仍向业务返回完整聚合状态。纯 JS 类型/逻辑修正，无原生侧对应修改。 | 已随 `Hook_useKeyboardState` demo 验证。 |
| 21 | `c44e7e7e31` | 2025-04-11 | ``fix: `OverKeyboardView` crash (#913)`` | 因系统能力缺失不能完全做 | 未同步 | OverKeyboardView touch 防崩 | 上游修复已存在 overlay host 的 touch dispatch。 | Harmony 缺少 `03937a761c` 所需的键盘上层基础宿主，不能单独承接此修复。 | 不测试错误层级的近似实现。 |
| 22 | `033e043774` | 2025-04-15 | `fix: Square payment SDK integration (#900)` | 已分析-不需同步 | 未同步 | iOS Square SDK delegate 兼容审计 | 无测试工程代码修改。上游只改 iOS：`KCTextInputCompositeDelegate` 增加 `canSubstituteTextFieldDelegate`，当原 delegate 类型名包含 `SQIPTextFieldInputModifier` 时不替换 delegate；`FocusedInputObserver` 在替换 `UITextField.delegate` 前调用该判断。 | 上游问题语义是 Square SDK 私有 `SQIPTextFieldInputModifier` 会把输入事件转发回注入的 keyboard-controller delegate，导致递归/崩溃。Harmony 没有 UIKit `UITextFieldDelegate` / Objective-C/Swift delegate 注入链路，当前 focused input 监听走 Harmony/RNOH 节点事件与 ArkUI 输入组件，不存在该私有 iOS delegate 递归路径。 | 不需要单独 demo；若未来接入鸿蒙支付 SDK 的自定义输入控件，再按实际控件事件链路做兼容审计。 |
| 23 | `9011a90a78` | 2025-04-17 | `fix: disabling KeyboardAwareScrollView didn't prevent useAnimatedStyle flickering (#921)` | 已分析 | 代码仓已推送 | KeyboardAwareScrollView `enabled=false` 禁用语义 | 已同步测试工程 `@react-native-ohos` 包源码和 ESM/CJS 产物；已回写代码仓 `src/components/KeyboardAwareScrollView/index.tsx`，本仓提交 `c2a9838` 已推送到 `origin/ups`。`KeyboardAwareScrollView` 在 `enabled=false` 时不再渲染底部 `<Reanimated.View style={view} />`。 | 功能效果：禁用 KASV 时彻底移除由 `useAnimatedStyle` 驱动的 padding view，避免 Fabric/Reanimated style 值不同步导致页面闪烁或 modal 高度异常；启用时原键盘避让逻辑保持不变。纯 JS 侧修改，无原生侧对应修改。 | 已确认没什么问题。后续回归：`enabled=false` 时页面不应产生额外底部 padding / flicker；`enabled=true` 时输入框聚焦仍能正常滚动避让。 |
| 24 | `0071eb058d` | 2025-04-17 | `fix: findNodeHandle on the web platform (#912)` | 已分析 | 代码仓已推送 | `findNodeHandle` 平台封装 | 已同步测试工程 `@react-native-ohos` 包源码、ESM/CJS 和 d.ts 产物；已回写代码仓新增 `src/utils/findNodeHandle/index.native.ts`、`src/utils/findNodeHandle/index.ts`，并让 KASV 改为从该 util 导入，本仓提交 `c2a9838` 已推送到 `origin/ups`。当前鸿蒙分支 `src/internal.ts` 没有上游 `useEventHandlerRegistration/findNodeHandle` 使用点，因此不额外添加未使用 import。 | 功能效果：native/Harmony 解析到 `.native` 时仍使用 RN `findNodeHandle`，行为不变；web 默认入口直接返回 handle/null，避免新版 `react-native-web` 抛错。对鸿蒙是结构对齐和未来兼容，非原生能力变更。 | 已确认没什么问题。后续回归：KASV 聚焦滚动仍能正确识别 ScrollView target；打包解析 `utils/findNodeHandle` 不报错。 |
| 25 | `c8a08ce2b8` | 2025-04-18 | `fix: OverKeyboardView rotation (fabric) (#918)` | 因系统能力缺失不能完全做 | 未同步 | OverKeyboardView 旋转尺寸 | 上游修复已存在 overlay host / ShadowNode 的旋转尺寸同步。 | Harmony 缺少键盘上层基础宿主，不能单独承接此修复。 | 不需要。 |
| 26 | `59dddfee31` | 2025-04-21 | `fix: App Store compatible icons (#926)` | 已分析-不需同步 | 未同步 | iOS example 图标资产 | 无测试工程代码修改。上游批量替换 iOS example / FabricExample / KeyboardControllerNative 的 AppIcon PNG 和 `Contents.json`，并调整 `cspell.json`。 | 该 commit 解决 App Store 提交时 iOS 图标透明边缘问题，只影响上游 iOS 示例工程发布资产；Harmony 库功能、JS API、ETS/C++ 原生实现均不受影响。 | 不需要。 |
| 27 | `661b39e24c` | 2025-05-16 | `docs: better JSDoc (#942)` | 已分析-不需同步 | 未同步 | JSDoc / 类型组织审计 | 无测试工程代码修改。上游增加 `eslint-plugin-jsdoc` 规则，补充大量公开 API JSDoc，并把 `src/types.ts` 拆成 `src/types/index.ts`、`hooks.ts`、`module.ts`、`internal.ts`、`views.ts`。 | 标题虽为 docs，但确实触达 `src`。本轮按要求不进行同步；该改动主要影响 IDE 文档提示、类型文件组织和 lint 规则，不改运行时功能，也不需要 Harmony 原生侧适配。后续若同步上游类型结构，需要单独批次处理，避免和行为修复混合。 | 不需要。 |
| 28 | `be7ef24f76` | 2025-05-20 | `fix: rely on screen dimensions instead of window dimensions (#948)` | 已分析-不需同步 | 未同步 | `useWindowDimensions` 尺寸来源审计 | 无测试工程代码修改。上游把 `src/hooks/useWindowDimensions/index.ts` 从直接导出 RN `useWindowDimensions` 改为自定义 hook：初始值取 `Dimensions.get("screen")`，订阅 RN `Dimensions.addEventListener("change")`，并使用 `e.screen.width/height` 更新。 | 上游问题语义是 iOS 旋转时 RN window 尺寸可能滞后，screen 尺寸更可靠。但该方案随后被 `427374fcb7` / `2a3bcee5ec` 修正为 `WindowDimensionsEvents.windowDidResize` + 实际 window 尺寸，说明单独同步 `be7ef24f76` 是已知中间态。Harmony 不单独进入 `screen` 方案，后续按第 28/75 的最终事件链路处理。 | 不测试中间态。 |
| 29 | `ba2e187a6c` | 2025-05-22 | `fix: keep shadow nodes in sync (#950)` | 已分析-不需同步 | 未同步 | Android native-driver ShadowNode 同步审计 | 无测试工程代码修改。上游 Android 新增 `ThemedReactContext.keepShadowNodesInSync(viewId)`，在键盘动画结束和 `keyboardDidShow` 后发送 `onUserDrivenAnimationEnded`，让 RN Animated/Fabric 侧重新同步 shadow tree，解决 `KeyboardStickyView` / `KeyboardToolbar` 内 `Pressable` 因 native-driver 动画后 ShadowNode 位置不同步而点不到的问题。 | 该 commit 依赖 Android native-driver + RN Animated/Fabric 的 `onUserDrivenAnimationEnded` 机制；Harmony 当前没有同名事件链路，且我们已决定不同步 `820fcb4b9d` 的 KSV plain Animated 改造，保留 Reanimated 版本并做了 Harmony start 阶段修复。因此当前没有等价代码需要同步。若未来重新迁移 plain Animated，再重新审计此链路。 | 不需要。 |
| 30 | `c5c00778f2` | 2025-05-23 | ``fix: support dynamic `bottomOffset` for KeyboardAwareScrollView (#952)`` | 已分析 | 代码仓已推送 | KeyboardAwareScrollView 动态 `bottomOffset` | 已同步测试工程并验证动态 offset 正常；已回写代码仓 `src/components/KeyboardAwareScrollView/index.tsx`，本仓提交 `17d373c` 推送到 `origin/ups`。同步 `useEffect` + `runOnUI` 触发 `bottomOffset` 变化后的重新滚动计算，并结合提前同步的 `852fa4a223` 使用 `performScrollWithPositionRestoration(scrollBeforeKeyboardMovement.value)` 防止二次叠加。 | 功能效果：键盘已打开、焦点输入框仍在当前 ScrollView 内时，业务动态改变 `bottomOffset` 后会立即重新计算滚动距离；从 `50` 切到 `0` 不再继续向上过度滚动。纯 JS/Reanimated 行为修复，无原生侧修改。 | 已在测试工程验证动态 offset 正常。 |
| 31 | `427374fcb7` | 2025-05-25 | `fix: wrong useWindowDimensions on iPad (#957)` | 已分析-可实现待验证 | 未同步 | `useWindowDimensions` 原生 resize 事件链路审计 | 无测试工程代码修改。上游删除 `index.android.ts`，让所有平台统一在 `index.ts` 订阅 `WindowDimensionsEvents.addListener("windowDidResize")`；iOS 原生在 `KeyboardControllerView.layoutSubviews` 中读取 `UIScreen.main.bounds.size`，尺寸变化时发射 `KeyboardController::windowDidResize`，并把该事件加入 iOS supported events。 | 复查后确认 Harmony 不是缺窗口事件：RNOH `RNAbility` 监听 `windowSizeChange`，`RNInstance` 暴露 `subscribeToLifecycleEvents("WINDOW_SIZE_CHANGE")` 并会向 C++ post 同名消息。本库当前只是没有把该事件桥成 `KeyboardController::windowDidResize`，因此直接同步 JS 仍会订阅空事件；但可在 Harmony TurboModule 中订阅生命周期事件并 emit `KeyboardController::windowDidResize`，需要测试工程验证。 | 待测试工程。验证旋转、分屏/折叠、页面尺寸变化时 KAV/KASV/Extender 尺寸同步。 |
| 32 | `e431917472` | 2025-05-26 | ``fix: `KeyboardAvoidingView` incorrect layout (#954)`` | 已分析 | 代码仓已推送 | KeyboardAvoidingView `height` 行为防 0 高度 | 已同步测试工程并回写代码仓 `src/components/KeyboardAvoidingView/index.tsx`，本仓提交 `17d373c` 推送到 `origin/ups`。在 KAV animated style 中先计算 `const height = frame.value.height - bottomHeight`，`behavior="height"` 时只有 `!keyboard.isClosed.value && height > 0` 才返回 `{ height, flex: 0 }`。 | 功能效果：避免 KAV 在键盘打开或 layout 边界情况下返回 `height: 0` / 负数；上游说明 RN 对 `{height:0}` 再切回 `{}` 时样式不会正确恢复，导致布局错误。纯 JS/Reanimated 行为修复，无原生侧修改。 | 已在测试工程验证无明显问题；极端小容器 guard 场景内容不显示属于测试场景压缩效果，重点是关闭后不永久塌陷。 |
| 33 | `331293a9cc` | 2025-06-09 | `fix: 2 memory leaks (#966)` | 已分析-不需同步 | 未同步（已回退 `0b03f32`） | Android memory leak 审计 | 无代码修改。Android 原提交修复 `ModalAttachedWatcher` 和 `WindowDimensionListener` 未 detach 的内存泄漏。Harmony 没有这两个 Android 类，也没有 `ReactApplicationContext` + Android view attach watcher / window dimension listener 的同名持有链路。此前按“同类生命周期风险”尝试把 Harmony `keyboardHeightChange` / `keyboardWillShow` / `keyboardWillHide` 改为稳定 handler、补 TurboModule `__onDestroy__` 和 C++ view 析构清理，但测试发现会导致多类键盘事件异常，说明该改动扩大了原 commit 范围并破坏了 Harmony 当前事件注册/分发时序。 | 最终判断：鸿蒙当前不存在上游 `331293a9cc` 修复的 Android 同类问题，不按该 commit 修改。`0b03f32` 的代码改动已从主仓和测试工程回退；后续若单独发现 Harmony listener 泄漏，需要基于实际日志/复现另开专项修复，不能挂在此 Android commit 下。 | 已在测试工程回退验证方向；由用户反馈“很多事件都不正常”触发撤回。 |

## 第 30-37 / 155 平台专用补充审计

> 这一段补齐主线第 30-37 笔源码 commit 的实际分析结果，避免把 Android-only / iOS-only 简单当作跳过依据。完整状态仍以 `UPGRADE-SOURCE-COMMIT-CHECKLIST-155.md` 为准。

| 源码顺序 | 上游 commit | 结论 | Harmony 等价风险分析 / 处理方案 |
| ---: | --- | --- | --- |
| 30 | `76226df4d9` | 因系统能力缺失不能完全做 | 上游修复独立 overlay window 的 accessibility children 关系。Harmony 无满足键盘上层语义的 overlay host，不存在可正确承接的无障碍转发链路。 |
| 31 | `49898cd5eb` | 已分析-不需同步 | 上游只改 iOS GitHub Actions Xcode 版本和 Xcode baseline plist，不触达库运行时代码、JS API、原生组件或 Harmony 构建链；确认后不改代码。 |
| 32 | `98df8d88e2` | 已分析-不需同步 | iOS 修 `KeyboardGestureArea` 通过 `inputAccessoryView` 扩展键盘时，输入框切换中延迟 `resignFirstResponder` 误关闭新键盘的问题。Harmony `KeyboardGestureAreaComponentInstance.cpp` 走 ArkUI touch，调用 ETS `dismiss/show`，无 UIKit responder/inputAccessoryView 链路；当前不改代码，保留快速切换输入框 + KGA offset 回归项。 |
| 33 | `331293a9cc` | 已分析-不需同步 | Android 修 native/window listener 泄漏；Harmony 无 `ModalAttachedWatcher` / `WindowDimensionListener` 同名链路。此前 `0b03f32` 按同类风险扩展修复后导致键盘事件异常，已回退；本 commit 最终不修改 Harmony 代码。 |
| 34 | `371cf7baef` | 已分析-不需同步 | iOS 26 键盘 view class prefix 变化导致 `KeyboardView.find()` 找不到 host，`onMove` 不触发。Harmony 事件来自 `RNKeyboardControllerTurboModule.ts` 的 `keyboardHeightChange` / `keyboardWillShow/Hide` / `getWindowAvoidArea(TYPE_KEYBOARD)`，再由 `KeyboardControllerViewComponentInstance.cpp` 合成 move 事件，不遍历 UIKit view hierarchy；不改代码。 |
| 35 | `fb95fc1364` | 已分析-不需同步 | JS 只为 `Platform.OS === "ios" && Platform.Version >= 26` 增加 Toolbar floating 样式和 `OPENED_OFFSET=-11`。Harmony 不进入 iOS 26 条件分支，且 Toolbar 已验证正常；当前不为 iOS 临时视觉策略改 Harmony。 |
| 36 | `9de91159df` | 已分析-不需同步 | iOS 把 `CADisplayLink` 从反复创建/销毁改为常驻 pause/resume。Harmony 没有 Swift/CADisplayLink，当前 C++ 只在键盘高度变化后通过 `taskExecutor->runDelayedTask` 合成有限帧，且 `startKeyboardAnimation()` 与析构会取消旧任务；无需同步。 |
| 37 | `c8398fc0e3` | 已分析-不需同步 | Android 清理 ViewManager 构造函数里的未使用 `ReactApplicationContext`。Harmony 通过 RNOH `Package::Context` / `ComponentInstance::Context` 创建 TurboModule 和组件，没有 Java/Kotlin ViewManager 长期持有 application context 的路径；不改代码。 |

## 第 38-39 / 155 新组件能力审计

| 源码顺序 | 上游 commit | 结论 | Harmony API 证据 / 处理方案 |
| ---: | --- | --- | --- |
| 38 | `a57fa4b427` | 代码仓已推送 | `KeyboardBackgroundView` 是视觉组件：iOS 通过私有 `UIKBBackdropView` 匹配键盘材质，Android 通过当前 IME package、系统深浅色和 hardcoded skin 表推断颜色。Harmony 无公开 API 读取当前系统键盘背景色/材质或复用键盘 backdrop，因此不新增 native spec；本次以 JS `View` fallback 导出 `KeyboardBackgroundView`，按 `colorScheme` 给默认浅/深色背景，业务 `style` 可覆盖。该实现与 Android 一样属于模拟键盘背景，和 iOS 私有材质能力不一致。 |
| 39 | `347fef35c0` | 代码仓已推送 | `KeyboardExtender` 的 iOS 真能力依赖 `UIInputView(inputViewStyle: keyboard)` + `inputAccessoryView`，让业务子树成为键盘的一部分并增加键盘高度；Harmony 普通应用侧无等价原生 accessory/panel API。Android 上游也使用 `KeyboardBackgroundView + KeyboardStickyView + useKeyboardAnimation` polyfill，本次 Harmony 采用同样 JS 组合，效果是在键盘上方跟随显示并模拟背景，和 iOS 系统级 keyboard accessory 语义不一致。测试工程已验证 `enabled`、children 按钮交互和背景容器正常。 |

## 第 40-42 / 155 本批同步与审计

| 源码顺序 | 上游 commit | 结论 | Harmony API 证据 / 处理方案 |
| ---: | --- | --- | --- |
| 40 | `60ec0ceab8` | 代码仓已推送 | `useKeyboardState` 新增 selector 参数，能力效果是允许业务只选择 `isVisible`、`height` 等必要片段，避免完整键盘 state 每次变化都触发使用方重渲染。已同步代码仓 `src/hooks/useKeyboardState/index.ts`；测试工程 demo 以黄色 selector 区块和 JSON `selectorIsVisible` 字段验证通过。 |
| 41 | `ecb3595085` | 代码仓已推送 | 上游删除 JS `monkey-patch`，避免 RN 0.80+ 对 `react-native/Libraries/Components/StatusBar/NativeStatusBarManagerAndroid` 深导入警告，Android 原生改用 `StatusBarManager` 覆盖原模块。Harmony 有本地 `StatusBarManagerCompat` TurboModule，但无 Android module override 机制；因此只同步 JS 去依赖和删除未引用 monkey-patch 文件，不改 Harmony 原生 StatusBar/键盘事件实现。 |
| 42 | `46bb921a5d` | 已分析-不需同步 | iOS 修 `shouldIgnoreKeyboardEvents` 在延迟 `resignFirstResponder` 后未复位的问题。Harmony 代码中未发现 `shouldIgnoreKeyboardEvents` / `KeyboardEventsIgnorer` / `InvisibleInputAccessoryView` / `inputAccessoryView` / `resignFirstResponder` / `KeyboardAreaExtender` 链路；`KeyboardGestureArea` 是 ArkUI touch -> ETS `dismiss/show`，不存在 UIKit responder 延迟 detach 导致事件过滤标志残留的问题。 |

## 补充审计规则：平台 only 不能直接跳过

上游 commit 只改 `android/` 或 `ios/`，只能说明改动路径是平台专用，不能直接得出 Harmony 不受影响。

后续处理每个 Android-only / iOS-only commit 时必须补做三步：

1. 看上游 diff 的真实问题语义：崩溃、生命周期、布局测量、键盘时序、窗口/状态栏、内存、构建/codegen 等。
2. 映射 Harmony 是否有同类链路：`harmony/keyboard_controller/src/main/cpp`、`harmony/keyboard_controller/src/main/ets`、RNOH 组件树、ArkUI 节点、Window 键盘事件、codegen/构建配置。
3. 只有确认 Harmony 无对应功能、无对应风险或已经由现有实现覆盖，才能标记“已分析-不需同步”；否则必须进入适配或降级记录。

## 历史补充审计池（72 笔，仅作提示）

> 这一节是早期 `83` 关键清单之外的补充审计池，保留用于提醒 Android-only / iOS-only 风险。后续推进和进度统计必须以 `UPGRADE-SOURCE-COMMIT-CHECKLIST-155.md` 的 `155` 总清单为准；若旧行仍显示 `待补充审计`，以本文“处理记录”和总清单的正式结论为准。

| 上游 commit | 日期 | 分类 | 标题 | Harmony 补充审计点 | 当前结论 |
| --- | --- | --- | --- | --- | --- |
| `59dddfee31` | 2025-04-21 | iOS-only | `fix: App Store compatible icons (#926)` | 已确认仅为 iOS example/App Store 图标资产，Harmony 库功能无影响。 | 已分析-不需同步 |
| `661b39e24c` | 2025-05-16 | 触达 JS/TS | `docs: better JSDoc (#942)` | 已确认触达 JSDoc、lint 和类型组织；本轮按要求不进行同步，后续若同步类型结构需单独批次处理。 | 已分析-不需同步 |
| `49898cd5eb` | 2025-06-03 | iOS-only | `ci: use newer XCode (#964)` | 已确认只改 iOS CI/Xcode baseline 文件，不触达库运行时代码或 Harmony 构建链。 | 已分析-不需同步 |
| `9de91159df` | 2025-06-13 | iOS-only | `perf: pause CADisplayLink instead of re-creation (#980)` | 已确认上游是 iOS `CADisplayLink` pause/resume 优化；Harmony 使用有限 `runDelayedTask` 合成帧且已在析构取消任务，无同类持久 display link 链路。 | 已分析-不需同步 |
| `f963befc1a` | 2025-07-12 | iOS-only | `fix: KeyboardExtender on iOS 26 (#1015)` | 查 KeyboardExtender 平台容器/键盘跟随语义；Harmony 实现该组件时需核对同类窗口层级问题。 | 待补充审计 |
| `8cca8df8a3` | 2025-07-19 | iOS-only | `feat: update swiftformat (#1028)` | 查是否纯格式工具；一般不影响 Harmony，但需确认无源码语义改动。 | 待补充审计 |
| `65586f9726` | 2025-07-22 | iOS-only | `fix: KeyboardBackgroundView build on fabric (#1034)` | 查 Fabric 构建/组件注册语义；Harmony 新增 BackgroundView 时需确认 codegen/native view 注册。 | 待补充审计 |
| `221b7210ec` | 2025-07-24 | Android-only | `fix: do not use BackgroundStyleApplicator directly (#1039)` | 查是否为 RN 内部 API 规避；Harmony 是否也使用不稳定 RNOH/ArkUI 内部 API。 | 待补充审计 |
| `6971c27ab6` | 2025-07-24 | iOS-only | `fix: keyboardAppearanceValue extension iOS < 12 compatibility (#1041)` | 查 keyboardAppearance 降级语义；Harmony 当前缺 appearance API 时需记录降级策略。 | 待补充审计 |
| `6c641a556e` | 2025-07-28 | 触达 JS/TS | `fix: remove maybescroll console debug (#1053)` | 查 JS 是否仍有调试日志；代码仓不能带临时日志。 | 待补充审计 |
| `6d39874aaf` | 2025-07-30 | 触达 JS/TS | `fix: mark KeyboardExtender enabled props as optional in TS (#1058)` | 查公开 TS 类型；Harmony 同步 KeyboardExtender 时需保持 `enabled` 可选性。 | 待补充审计 |
| `63d77f8a00` | 2025-08-01 | iOS-only | `fix: iOS crash caused by the KeyboardControllerView class implementation (#1059)` | 查崩溃根因是否是视图类/生命周期；Harmony KCV C++ component instance 是否有同类销毁态风险。 | 待补充审计 |
| `3ae5e72d8c` | 2025-08-01 | iOS-only | `fix: clang format (#1060)` | 查是否纯格式；若仅 iOS clang-format 可记录不需同步。 | 待补充审计 |
| `93a55c1e96` | 2025-08-05 | iOS-only | `fix: KeyboardExtender auto-resizeable height (#1064)` | 查 Extender 高度自适应语义；Harmony 原生视图若实现 Extender 需同步验收。 | 待补充审计 |
| `5b0b4c35b1` | 2025-08-05 | iOS-only | `fix: attaching KeyboardExtender (#1065)` | 查 Extender attach 时机；Harmony 是否需要窗口可用后再挂载。 | 待补充审计 |
| `b6e3594860` | 2025-08-07 | iOS-only | `fix: keyboardLayoutGuide availability (#1070)` | 查键盘布局 guide 可用性判断；Harmony 对应是 Window avoid area / keyboard info 可用性。 | 待补充审计 |
| `360230ba10` | 2025-08-09 | iOS-only | `fix: KeyboardExtender on iOS 26 with round corners (#1067)` | 查系统键盘圆角/外观依赖；Harmony 缺公开圆角 API 时需降级记录。 | 待补充审计 |
| `c1d18cc677` | 2025-08-09 | Android-only | `fix: StatusBar race conditions (#1074)` | 查状态栏/窗口尺寸竞态；Harmony `useWindowDimensions` 与系统避让模式是否有同类时序问题。 | 待补充审计 |
| `aff3cbe7d1` | 2025-08-12 | 触达 JS/TS | `fix: fix disabling KeyboardAvoidingView (#1078)` | 已同步 KAV `enabled=false` 完整禁用行为，主仓已推送。 | 代码仓已推送 |
| `1d7721b9c7` | 2025-08-13 | iOS-only | `fix: avoid direct FBReactNativeSpec usage (#1084)` | iOS 新架构 spec header/import 稳定性修复；Harmony 使用 codegen-harmony，不编译 `FBReactNativeSpec`，无同类风险。 | 已分析-不需同步 |
| `2efe655939` | 2025-08-13 | Android-only | `fix: add target_compile_reactnative_options (#1085)` | Android CMake 编译选项修复；Harmony native 构建不使用该 Android target/函数。 | 已分析-不需同步 |
| `38217e6671` | 2025-08-27 | Android-only | `fix: clear legacy full screen flag (#1098)` | Android fullscreen flag 与 edge-to-edge/insets 冲突修复；Harmony Window API 路径无 `FLAG_FULLSCREEN` 遗留链路。 | 已分析-不需同步 |
| `1edf41f1cb` | 2025-08-29 | Android-only | `fix: don't use @RequiresApi with old SDK versions (#1107)` | Android annotation/SDK 版本保护修复；Harmony ArkTS 能力判断体系不同。 | 已分析-不需同步 |
| `2a3bcee5ec` | 2025-09-08 | 触达 JS/TS | `fix: KeyboardExtender width on iPad in split mode (#1113)` | 依赖上游 `windowDidResize` 原生尺寸事件；RNOH 已有 `WINDOW_SIZE_CHANGE` 生命周期事件，本库可桥接为 `KeyboardController::windowDidResize`，需真机验证窗口尺寸语义。 | 已分析-可实现待验证 |
| `f2d74b3bc7` | 2025-09-26 | iOS-only | `fix: KeyboardExtender initial mount on Fabric (#1135)` | iOS `inputAccessoryView`/Fabric 首挂载修复；Harmony Extender 是 JS polyfill，无 UIKit accessory attach 链路。 | 已分析-不需同步 |
| `8d3726d387` | 2025-10-01 | Android-only | `chore: react-native 0.80 (#979)` | RN 0.80 dependency/example/Android Kotlin 兼容维护；Harmony 当前是 RNOH/RN 0.82 基线，不反向同步依赖。 | 已分析-不需同步 |
| `4ab7be0c15` | 2025-10-13 | iOS-only | `fix: crash when unrecognized selector sent (#1153)` | iOS `KCTextInputCompositeDelegate` forwarding/selector crash 兜底；Harmony 无 ObjC/Swift delegate forwarding 链路。 | 已分析-不需同步 |
| `6503e23efd` | 2025-10-14 | iOS-only | `fix: iOS 26 + Modal (#1158)` | iOS 26 `KeyboardTrackingView` 在 Modal/window 切换时重新 attach；Harmony 无 UIKit tracking view，键盘事件走 Window avoid area。 | 已分析-不需同步 |
| `3ab39a0edd` | 2025-10-20 | iOS-only | `fix: attach keyboard tracking view in advance and only when window has been changed (#1170)` | 修第 84 的 attach 策略，改用 `willMove(toWindow:)`；Harmony 无对应生命周期。 | 已分析-不需同步 |
| `99be9d3011` | 2025-10-27 | iOS-only | `fix: avoid ensureLayout calls (#1174)` | 撤第 81 的 `ensureLayout`，改 SafeKVC 读 `_comingFromJS` 过滤 selection；Harmony 无 KVC/private delegate 字段。 | 已分析-不需同步 |
| `6e4b14d1e9` | 2025-11-03 | Android-only | `fix: match StatusBar and edge-to-edge interop according to latest spec (#1185)` | Android RN core `edgeToEdgeEnabled` BuildConfig 下忽略 StatusBar background/translucent；Harmony 无该 Gradle flag，现有 `preserveEdgeToEdge` 是本库窗口属性。 | 已分析-不需同步 |
| `0dcf4b4d41` | 2025-11-03 | iOS-only | `fix: non-working release version on iOS 26 (#1184)` | iOS release 冷启动 rootView 未就绪，延后 tracking view attach 到 app active；Harmony 无 iOS rootView tracking view。 | 已分析-不需同步 |
| `b0e60f6b32` | 2025-11-04 | iOS-only | `fix: attaching KeyboardTrackingView (#1193)` | iOS 在 `KeyboardControllerView.didMoveToWindow` 传 window 给 tracking view；Harmony ArkUI component instance 无 UIKit `didMoveToWindow`。 | 已分析-不需同步 |
| `1441ae3b59` | 2025-11-06 | Android-only | `fix: add KeyboardBackgroundView to turbo package (#1197)` | 上游只在 Android RN < 0.74 `TurboPackage` 补注册既有 native `KeyboardBackgroundViewManager`。Harmony 前序已按 Android/default 路线实现：`KeyboardBackgroundView` 是 JS `View` fallback，`KeyboardExtender` 是 `KeyboardBackgroundView + KeyboardStickyView + useKeyboardAnimation` polyfill；本仓没有 Harmony native BackgroundView manager/descriptor，故无需补 `keyboardControllerPackage.cpp` 注册，也不补空原生组件。 | 已分析-不需同步 |
| `70784fd70f` | 2025-12-03 | 触达 JS/TS | `fix: dependencies array changes size between renders (#1229)` | 查 React hooks 依赖数组稳定性；Harmony JS 必须同步或确认已覆盖。 | 待补充审计 |
| `59951035af` | 2025-12-09 | iOS-only | `fix: avoid undesired reloadInputViews during keyboard dismissal (#1235)` | 查 dismiss 过程中是否误触发键盘重载/重开；Harmony hide/keepFocus 需验证。 | 待补充审计 |
| `acbc7ae418` | 2025-12-12 | Android-only | `fix: wrong onEnd event on Samsung devices when keyboard gets shown after interactive dismissal (#1240)` | 查键盘 end 事件时序；Harmony 合成 move/end 是否有同类异常。 | 待补充审计 |
| `afeccd120e` | 2025-12-17 | 原生混合 | `fix: shorter codegen name (#1251)` | 查 codegen name 长度与 Harmony 生成物/包名限制。 | 待补充审计 |
| `e5438be3df` | 2025-12-22 | 触达 JS/TS | `refactor: move KeyboardProvider props in separate file (#1255)` | 查类型组织和导入路径；Harmony JS 分叉需同步或保留兼容。 | 待补充审计 |
| `b5fb83596c` | 2025-12-23 | 触达 JS/TS | `refactor: change conditional code in KeyboardAvoidingView (#1257)` | 查 KAV 条件分支行为是否变化；Harmony JS 需对齐。 | 待补充审计 |
| `838f546c48` | 2025-12-30 | Android-only | `Call correct onInterceptHoverEvent super in onInterceptHoverEvent() (#1260)` | 因系统能力缺失不能完全做：修复依赖已存在的 `OverKeyboardRootViewGroup`；Harmony 无满足键盘上层语义的 overlay root。 | 已完成专项审计 |
| `91df02ecd4` | 2026-01-07 | iOS-only | `fix: navigation in iOS hybrid app (#1266)` | 查导航/混合 App 的窗口查找问题；Harmony 多窗口/页面栈需验证。 | 待补充审计 |
| `68395bf201` | 2026-01-12 | 原生混合 | `fix: windows builds (#1248)` | 查是否构建系统兼容；Harmony 编译链若无对应可记录不需同步。 | 待补充审计 |
| `8041105fea` | 2026-01-15 | iOS-only | `fix: manual did events (#1161)` | 查手动 did 事件时序；Harmony fallback will/did 和合成 move 需验证。 | 待补充审计 |
| `a0d49aeb7a` | 2026-01-27 | 触达 JS/TS | `refactor: add useCombinedRef to separate the code logically (#1287)` | 查 ref 合并行为；KASV/ScrollView ref 稳定性需同步。 | 待补充审计 |
| `ebc5205903` | 2026-01-30 | iOS-only | `fix: wrong onInteractive event when keyboard closed on iOS 26+ with attached KeyboardGestureArea (#1299)` | 查交互式键盘事件过滤；Harmony 虽无真实 interactive，也需确认合成事件不误发。 | 待补充审计 |
| `5ef74d9365` | 2026-02-06 | iOS-only | `refactor: update swiftformat (#1305)` | 查是否纯格式；若无语义改动可记录不需同步。 | 待补充审计 |
| `ac7dee1c27` | 2026-02-09 | 触达 JS/TS | `fix: wrong selection coordinates on focus (#1234)` | 查 selection/focus 时序；Harmony 需验证 focus 后首次 selection 坐标与 layout 同步。 | 待补充审计 |
| `76056c322d` | 2026-02-20 | 触达 JS/TS | `fix: KeyboardAwareScrollView loosing ref identity (#1319)` | 查 KASV ref 身份稳定；Harmony JS 必须同步或确认已覆盖。 | 待补充审计 |
| `d1fb606b0b` | 2026-02-21 | 触达 JS/TS | `fix: enabled prop behavior for KeyboardStickyView (fabric arch) (#1312)` | 查 StickyView `enabled` 行为；Harmony JS 需对齐。 | 待补充审计 |
| `a096e393fd` | 2026-03-04 | 触达 JS/TS | `fix: noop scrollRectToVisible on iOS (#1336)` | 查 JS 侧是否引入平台 noop/测量保护；Harmony 需要确认对应命令不会误调用。 | 待补充审计 |
| `8393ee294f` | 2026-03-05 | Android-only | `fix: memory leak on Android (#1343)` | 查监听注册/销毁；Harmony ETS/C++ observer、delayed task、event emitter 是否清理。 | 待补充审计 |
| `d4f1a4cf91` | 2026-03-07 | 触达 JS/TS | `fix: access before initialization error (#1342)` | 基础 ChatScrollView 已落地，本 helper 初始化顺序修复已有真实文件落点，应作为下一笔纯 JS 修复同步。 | 已分析-可同步 |
| `f5dca3cdf9` | 2026-03-11 | 触达 JS/TS | `fix: KeyboardAvoidingView types union (#1354)` | 查公开 TS 类型；Harmony 类型需对齐。 | 待补充审计 |
| `eb2dfbbe35` | 2026-03-12 | 触达 JS/TS | `fix: handle broken measureInWindow (#1355)` | 已同步上游 JS/native 契约；Harmony 使用 Fabric ShadowTree `dom::measureInWindow`，失败时 reject 并由 JS 回退局部 layout。 | 已同步-已验证 |
| `7a7b6cab6d` | 2026-03-16 | iOS-only | `fix: typo in arch definition (#1367)` | 查是否影响 Fabric 架构定义；Harmony JS 架构检测需确认。 | 待补充审计 |
| `ecd3bbd8e3` | 2026-03-18 | 触达 JS/TS | `fix: could not fing view for tag warning (#1379)` | 已按上游为 KASV `viewPositionInWindow` Promise reject 增加静默降级，保留旧顶部坐标；无原生改动。 | 已同步，按用户要求未单独验证 |
| `bba6afc9eb` | 2026-03-19 | 触达 JS/TS | `perf: don't change currentKeyboardFrame each frame (#1381)` | 查键盘帧对象更新策略；Harmony 合成 move 是否造成无谓 re-render/shared value 抖动。 | 待补充审计 |
| `90dd77289e` | 2026-03-21 | 触达 JS/TS | `fix: KeyboardAwareScrollView regression after optimization (#1387)` | 查 KASV 优化后的回归；Harmony bottomOffset/KASV 必须重点验证。 | 待补充审计 |
| `9acd790895` | 2026-03-23 | 触达 JS/TS | `fix: remove unnecessary code (#1389)` | 查是否纯清理；若无行为改动可记录不需同步。 | 待补充审计 |
| `33a5d7b585` | 2026-03-24 | Android-only | `fix: KeyboardChatScrollView + RefreshControl conflict (#1398)` | Harmony 当前没有 Android `ClippingScrollViewDecoratorView` 对应的原生 decorator，因此没有 RefreshControl child 查找链路；纠正后也不再使用 JS spacer。 | 已分析-不需同步 |
| `af5da35fb0` | 2026-03-26 | iOS-only | `fix: respect UIDesignRequiresCompatibility in KeyboardExtenderCont… (#1402)` | 已审计：iOS `UIInputView`/glass/compat plist 专用，Harmony Extender 是 Android 同级 JS polyfill。 | 已分析-不需同步 |
| `ba41d5d1ea` | 2026-03-27 | 触达 JS/TS | `fix: KeyboardAwareScrollView re-focus after hardware keyboard dismissal (#1403)` | 已审计：依赖上游第 111/113 后的 KASV selection/contentInset 状态机，当前 Harmony 仍保留旧 padding view 路线。 | 已分析-暂不同步 |
| `f83e78c04d` | 2026-03-31 | 触达 JS/TS+原生 | `fix: native rounded keyboard detection (#1415)` | 已审计：Harmony 可按 Android 保守语义返回 `keyboardBorderRadius: 0` 并同步 JS 常量，需先进测试工程验证 Toolbar rounded 判断。 | 已分析-待测试 |
| `7b9813396c` | 2026-03-31 | iOS-only | `fix: call syncUpLayout on iOS when keyboard changes its mode (emoji vs text) (#1417)` | 已审计：iOS responder notification 专用；Harmony 高度变化已由 `keyboardHeightChange` 触发 C++ `syncUpLayout()`，且未同步公开 layout sync/KASV contentInset 链。 | 已分析-不需同步 |
| `75981eeb36` | 2026-04-01 | 触达 JS/TS | `test: cover keyboard resize with unit test (#1418)` | 已审计：只补上游 contentInset/selection KASV 单测，无 Harmony 当前运行时代码可同步。 | 已分析-不需同步 |
| `fbedbc2308` | 2026-04-03 | 触达 JS/TS | `feat: add mode prop for KeyboardAwareScrollView (#1420)` | 已审计：默认 `"insets"` 依赖稳定 contentInset，Harmony 当前只有旧 `"layout"` 行为；不能用 Harmony-only 默认绕开。 | 已分析-暂不同步 |
| `51fbe47b09` | 2026-04-06 | Android-only | `fix: WindowDimensionListener behavior when Activity restarts (#1422)` | 已审计：Android Activity/context listener 重挂专用；Harmony windowDidResize bridge 尚未落地，后续实现第 28/75 时吸收清理/重挂原则。 | 已分析-不需同步 |
| `e979dbf426` | 2026-04-10 | Android-only | `fix: don't modify edge-to-edge mode by default when module toggled on/off (#1412)` | 已审计：Android WindowInsets/DecorView/softInputMode 专用，Harmony 走独立 Window API，不套用。 | 已分析-不需同步 |
| `1d9a9ac21f` | 2026-04-15 | 触达 JS/TS | `feat: freeze in KeyboardChatScrollView as SharedValue (#1434)` | JS API 应保持上游，但 freeze 冻结的是 Chat 的 inset/offset 状态；Harmony 当前没有正确的 contentInset 状态可冻结和恢复，因此该组件属性暂不能宣称支持。 | 因系统能力缺失不能完全做 |
| `b3dc83edd5` | 2026-04-16 | 触达 JS/TS | `fix: close keyboard even if it's partially visible (#1436)` | 已审计并试验后暂不同步：上游把 JS 可见状态前移到 `keyboardWillShow`，但 Harmony 真机 focus 后系统会在约十几到几十毫秒内自动发 `keyboardWillHide`，测试工程未稳定进入 `KeyboardController.dismiss()` 原生调用链；继续同步会改变 `isVisible/state()` 时机，却无法验证 opening dismiss 语义。库和 demo 修改已撤销。 | 已分析-暂不同步 |
| `15111a4213` | 2026-04-17 | 触达 JS/TS | `fix: pass through touches in KeyboardToolbar (#1440)` | 已审计：当前 Harmony Toolbar 外层同样缺 fixed bounds，存在同类触摸穿透风险；需测试工程验证按钮点击和位置。 | 已分析-待测试 |
| `c436fbb9e0` | 2026-04-20 | 触达 JS/TS+Android | `fix: bug with KeyboardChatScrollView + inverted + maintainVisibleScrollPosition + FlashList (#1437)` | 只作用于 inverted + FlashList；Harmony JS 保持上游并继续接收 `inverted`，但原生缺少稳定 top inset/负滚动范围，无法达到该组合的等价语义。 | 因系统能力缺失不能完全做 |
| `1048f5238a` | 2026-04-27 | Android-only | `fix: resolve view crash (#1444)` | Android `resolveView` 专用；Harmony 的 `viewPositionInWindow` 已在 ShadowNode/revision 缺失时 reject，JS catch 回退，已具备等价失败保护。 | 已分析-不需同步 |
| `ce8ccaa5f7` | 2026-04-28 | 触达 JS/TS | `feat: add onContentInsetChange callback (#1445)` | 上游 JS 可以计算并回调目标 inset，但 Harmony 原生没有实际应用同一 inset；在补齐原生底座前对外回调会表达未真正生效的状态，不能宣称等价支持。 | 因系统能力缺失不能完全做 |
| `21a939f30d` | 2026-04-29 | 触达 JS/TS | `feat: migrate KeyboardStickyView to reanimated (#1307)` | 已审计：Harmony 当前 `KeyboardStickyView` 已是 Reanimated 实现，本提交目标已覆盖。 | 已分析-不需同步 |
| `5dc56430d7` | 2026-05-04 | 触达 JS/TS | `feat: expose onEndVisible callback from KeyboardChatScrollView (#1450)` | 独立公式可以计算滚动位置是否在内容末端，但键盘场景下 Harmony 的 viewport、offset、contentSize 与目标 inset 状态不一致，无法保证回调等价表达“Chat 末尾可见”；作为 Chat 专属属性暂不支持。 | 因系统能力缺失不能完全做 |
| `1a6450ef35` | 2026-05-15 | iOS-only | `fix: use weak reference to keyboard view on iOS (#1465)` | 已审计：iOS keyboard view hierarchy 缓存专用；Harmony 不定位或持有系统键盘 view。 | 已分析-不需同步 |
| `cb011a80ff` | 2026-05-16 | iOS-only | `fix: crash on catalyst (#1464)` | 已审计：Mac Catalyst + UIKit swizzle guard 专用，Harmony 无 ObjC/UIKit ClippingScrollView 路线。 | 已分析-不需同步 |
| `38076a9df8` | 2026-05-21 | Android-only | `fix: delay onStart event (#1461)` | 已审计：Android AOSP `WindowInsetsAnimation.Callback.onStart` 重入 crash 专用；Harmony C++ 合成事件不在该系统回调窗口内。 | 已分析-不需同步 |

## 处理记录

### 2026-07-24

- 已确认 `2a923a6962` 属于 JS 架构检测同步，不代表 Harmony 支持 `OverKeyboardView` overlay 原生能力。
- 已确认 `b8a4de91fd` 只同步源码类型改动，不同步 RN 0.78 依赖升级。
- 已同步并推送 `2b21634df9`；已分析 `1714227edd`、`3a366a3a3b` 为不需同步。
- 已在测试工程同步 `8eb6a7be14`；已补充审计 `41d798532d` 并记录当前不需同步；已确认 `18e550c898` 为 release commit，不需同步。
- 已同步并推送 `8eb6a7be14`、`5b65344e6b`、`efd038cba2`、`9080f313be`、`cb8161cf00`。
- 当时将 `e1a4cc4e58`、`03937a761c`、`87e47e556f` 记录为当前不需同步；后续真机专项原型已将 `03937a761c` 纠正为“因系统能力缺失不能完全做”，`87e47e556f` 仍为 iOS 路径修复、不需同步。
- 已同步并验证 `b27ed8a284` + `0c6ae77362` 的 `useKeyboardState`；`820fcb4b9d` 的 `KeyboardStickyView` plain Animated 变更在测试工程观察到问题，已回退测试工程修改，暂不提交；已补充审计 `13dfb32c54` 为 iOS delegate 专用，不需 Harmony 代码同步。
- 当时将 `c44e7e7e31`、`033e043774` 记录为平台专用且无对应链路；后续真机专项原型已将 `c44e7e7e31` 纠正为“因系统能力缺失不能完全做”。已同步并推送 `9011a90a78`、`0071eb058d`，本仓提交 `c2a9838`。
- 当时未同步 `c8a08ce2b8`、`59dddfee31`、`661b39e24c`；后续真机专项原型已将 `c8a08ce2b8` 纠正为“因系统能力缺失不能完全做”，其旋转修复没有可承接的键盘上层基础宿主。
- 已分析 `be7ef24f76`、`ba2e187a6c`、`c5c00778f2`、`427374fcb7`、`e431917472`：尺寸 hook 链路暂不同步；Android ShadowNode 同步当前不需同步；`c5c00778f2`、`e431917472` 进入测试工程同步候选。
- 已验证并推送 `c5c00778f2`、`e431917472`；因动态 `bottomOffset` 测试复现 over-scrolling，提前同步并验证 `852fa4a223`，本仓提交 `17d373c`。
- 已补充审计第 `30-37 / 155` 笔；后续真机专项原型已将其中 `76226df4d9` 纠正为“因系统能力缺失不能完全做”，其无障碍修复依赖不存在的键盘上层 overlay host。其余条目维持各自审计结论；`331293a9cc` 曾按同类风险提交 `0b03f32`，测试发现事件异常后已回退，最终记录为 Harmony 无同类问题。
- 已同步并验证第 `38-39 / 155` 笔：`KeyboardBackgroundView` 在 Harmony 以 JS `View` fallback 模拟键盘背景；`KeyboardExtender` 采用与 Android 一致的 `KeyboardBackgroundView + KeyboardStickyView + useKeyboardAnimation` polyfill。文档明确该方案和 iOS 原生 accessory / 私有键盘背景材质能力不一致。
- 已同步并推送第 `40-41 / 155` 笔：`useKeyboardState(selector)` 与去除 JS `monkey-patch` 深导入依赖；已审计第 `42 / 155` 笔 iOS `shouldIgnoreKeyboardEvents` 复位问题，Harmony 无同类 UIKit responder/accessory 状态机，当前不改代码。
- 已审计第 `43 / 155` 笔 Android `StatusBarModule` 反射修复，Harmony 使用本地 `StatusBarManagerCompat` TurboModule，无 Android Kotlin internal/reflection 同类问题，当前不改代码。
- 已分析第 `44 / 155` 笔 `keyboardAppearance` 外观来源改造，实测发现能力依赖 RNOH 框架 `TextInputComponentJSIBinder` 暴露 `keyboardAppearance` native prop；当前鸿蒙框架未暴露该接口，JS 传入 `light/dark` 时本库 C++ 只能读到 `default`。按“不修改框架代码”的策略，本仓不适配该 commit，记录为鸿蒙当前无可用暴露接口。
- 已同步并推送第 `45 / 155` 笔 KASV selection 驱动滚动：用 `onSelectionChange` 的 caret y 替代 `onChangeText` 主驱动；Harmony 原生已通过 `FocusedInputSelectionChanged` 提供 `selection.end.y`，本轮仅需 JS 侧同步。测试确认无明显问题。
- 已分析第 `46 / 155` 笔 iOS 26 `KeyboardExtender` 视觉修复：依赖 UIKit `UIInputView`/`inputAccessoryView`/`UIGlassEffect`，Harmony 当前走 Android 同级 polyfill，无同类原生链路，不同步。
- 已同步并推送第 `47-48 / 155` 笔 `preload`：JS 增加 `KeyboardController.preload()` 和 `KeyboardProvider preload` 默认调用；Harmony 原生采用 Android 同策略 no-op，避免真实调用 `showSoftKeyboard()` 拉起键盘。测试确认 `preload()` 调用和默认 `KeyboardProvider` 挂载不会自动弹键盘，事件无异常。
- 已分析第 `49 / 155` 笔 iOS `swiftformat`，纯 Swift 格式化，Harmony 不同步。
- 已分析并撤销第 `50 / 155` 笔 KASV full screen input support 的测试工程同步：Harmony `selection.end.y` 与 Android/iOS 上游依赖的 caret 局部坐标语义不一致，且缺少可替代读取 TextArea 内 caret 可视 rect / scroll offset 的公开 API；同步该大输入框逻辑会引入滚动异常，因此标记为系统能力缺失不能完全做。
- 已补充审计第 `51-57 / 155` 笔平台原生变更：`e6679bde41` 是 iOS UITextInput delegate 延后一帧替换，Harmony 无 UIKit delegate 替换/恢复链路；`65586f9726` 是 iOS `KeyboardBackgroundViewCls` Fabric 编译补丁，Harmony 当前 BackgroundView 为 JS fallback，无 native class provider；`d6b93dc2d8` 是 Android `WindowDimensionsListener` reload 后清空引用，Harmony 没有 Android listener 对象，但本轮复查确认 RNOH 已有通用 `WINDOW_SIZE_CHANGE` 生命周期事件，可作为第 28/75 的实现基础，不能扩展到键盘事件 listener；`221b7210ec` 是 Android 避免直接用 `BackgroundStyleApplicator` 内部 API，Harmony JS fallback 只用 RN `View` style；`6971c27ab6` 是 iOS < 12 `userInterfaceStyle` 编译保护，Harmony 无 Swift extension且 `keyboardAppearance` 已记录不改框架；`8a2bc0814b` 是 iOS native `OverKeyboardView` conditional mount/touch/recycle 修复，因 Harmony 缺少键盘上层基础宿主而标记为“因系统能力缺失不能完全做”；`e736db9971` 是 iOS 键盘 view locator 重构，Harmony 通过 Window avoid area / 键盘事件取高度，不查系统键盘 view hierarchy。
- 已审计第 `58-59 / 155` 笔 JS/TS 小改：`6c641a556e` 删除 KASV `console.debug`，当前代码仓和测试工程均无该日志；`6d39874aaf` 把 `KeyboardExtenderProps.enabled` 标为可选，当前 `src/types.ts` 已随前序 `KeyboardExtender` 实现对齐并推送。
- 已补充审计第 `60-69 / 155` 笔平台原生变更：`63d77f8a00` / `3ae5e72d8c` 只修 iOS `KeyboardControllerView.mm` 事件分支和格式；Harmony event emitter 为独立 C++ 方法，无同类 ObjC block 嵌套 crash。`e4e6e6e637` / `003b3c2688` / `b6e3594860` 围绕 iOS 26 `KeyboardTrackingView`、`keyboardLayoutGuide` 和 interactive dismissal，Harmony 当前使用 Window avoid area / 键盘事件和合成帧，不查系统键盘 view，也无真实 interactive 逐帧 API。`93a55c1e96` / `5b0b4c35b1` / `360230ba10` 都是 iOS `KeyboardExtender` 的 `inputAccessoryView` 高度、attach 和玻璃圆角修复，Harmony 当前按 Android 同级 JS polyfill 实现，不存在 `UIInputView` / `reloadInputViews` / `UIGlassEffect` 链路。`af6d19d51d` 是 iOS observer 文件拆分。`c1d18cc677` 是 Android StatusBar/EdgeToEdge registry 竞态，Harmony StatusBarManagerCompat 直接走 Window API，不存在 Android rootView tag 查找竞态。
- 已在测试工程同步第 `70 / 155` 笔 KAV `enabled=false` 修复：`KeyboardAvoidingView` 的 `useAnimatedStyle` 在 `!enabled` 时直接返回 `{}`，禁用后彻底关闭 `height/position/padding/translate-with-padding` 所有避让样式；测试工程同时恢复 KAV demo 中 `padding` 和 `translate-with-padding` 按钮，便于四种 behavior 回归。
- 已验证并回写第 `70 / 155` 笔 KAV `enabled=false` 修复：用户确认测试无问题，代码仓同步 `src/components/KeyboardAvoidingView/index.tsx`，功能效果是 `enabled=false` 时完全关闭所有避让行为，而不是仅把部分 offset 置 0。
- 已补充审计第 `71-75 / 155` 笔：`1d7721b9c7` 是 iOS `FBReactNativeSpec` import/header 稳定性修复，Harmony codegen-harmony 无同类链路；`2efe655939` 是 Android CMake `target_compile_reactnative_options`，Harmony native build 不使用该 Android target；`38217e6671` 是 Android legacy fullscreen flag 清理，Harmony 直接走 Window API，无 `FLAG_FULLSCREEN` 残留链路；`1edf41f1cb` 是 Android `@RequiresApi`/SDK annotation 兼容，Harmony ArkTS 版本保护体系不同；`2a3bcee5ec` 依赖 `windowDidResize`，经复查 RNOH `WINDOW_SIZE_CHANGE` 可承接，改为可实现待验证。
- 已同步并验证第 `76 / 155` 笔 compound `KeyboardToolbar`：主仓新增 `KeyboardToolbar.Background/Content/Prev/Next/Done`、compound context/components/constants，并保留 legacy props 的 deprecated 兼容入口。测试工程 `KeyboardToolbar` demo 保留 `legacy`、`compound`、`prevent` 三种模式；用户已确认 Toolbar 可用。针对运行时 `cannot read property background of undefined`，主仓和测试工程都把 `appearance !== "dark"` 统一映射为 `light`，避免 Harmony 返回 `default` 时访问不存在的 `theme.default`。
- 已补充审计第 `77-78 / 155` 笔：`0a97fd68ba` 是 Android double keyboard height/insets 修复，Harmony 高度来自 Window avoid area / 键盘高度事件，未走 Android `WindowInsetsCompat` 叠加路径；`f2d74b3bc7` 是 iOS `KeyboardExtender` Fabric 初挂载修复，Harmony Extender 是 JS polyfill，无 UIKit `inputAccessoryView` attach 链路；均不改代码。
- 已同步并验证第 `79 / 155` 笔 `dismiss({ animated })` API 形状：JS `DismissOptions` 增加 `animated`，`module.ts` 默认 `animated=true` 并把 `(keepFocus, animated)` 传给 native spec；Harmony ETS 签名接收 `animated`，但由于当前无公开 API 可保证无动画隐藏软键盘，仍降级调用 `stopInputSession()`。测试工程 `KeyboardController core API` demo 增加持久 `dismiss_log`，用于观察 `dismiss_result` 从 `pending animated=false keepFocus=true` 到 `resolved animated=false keepFocus=true`；用户已确认原本一帧状态太快，保留 log 更适合观察。
- 已补充审计第 `80 / 155` 笔 RN 0.80 维护提交：主体是 dependency/example/Gradle/lockfile 和 Android Kotlin 兼容维护；当前 Harmony 包是 RNOH/RN 0.82 基线，不反向同步 RN 0.80 依赖和 example 样式。
- 已补充审计第 `81-86 / 155` 笔 iOS selection / `KeyboardTrackingView` / SafeKVC 线索：第 81 针对 iOS multiline selection 先引入 `ensureLayout`，但 Harmony selection 坐标来自 ArkUI caret offset，且第 86 已撤销该方案；第 82 是 ObjC/Swift composite delegate selector crash 兜底，Harmony 无 UIKit delegate forwarding；第 83-85 都是 iOS 26 `keyboardLayoutGuide` / `KeyboardTrackingView` 生命周期和 Modal attach 策略，Harmony 通过 ETS Window keyboard events / avoid area 和 C++ 合成帧，不挂 UIKit tracking view；第 86 改 SafeKVC 读取 `_comingFromJS` 过滤 selection，Harmony 无 KVC/private delegate 字段。因此整组不改代码。
- 已补充审计第 `87 / 155` 笔 Android StatusBar edge-to-edge 互操作：上游依赖 Gradle `edgeToEdgeEnabled` 生成 Android `BuildConfig.IS_EDGE_TO_EDGE_ENABLED`，RN core edge-to-edge 开启时忽略 StatusBar `backgroundColor/translucent`。Harmony 没有该 Android BuildConfig 信号；当前 `KeyboardControllerView.preserveEdgeToEdge` 调 `setWindowLayoutFullScreen`，`StatusBarManagerCompat` 是普通 Window system bar property 兼容模块，不做无来源的 Harmony-only 状态判断。
- 已补充审计第 `88-89 / 155` 笔 iOS 26 tracking view attach 修复：上游从 app active 延后 attach 到 `didMoveToWindow` 传 window，均依赖 UIKit `KeyboardTrackingView`；Harmony 无此视图和生命周期，不同步。
- 已修正并补强第 `90 / 155` 笔 Android `KeyboardBackgroundView` TurboPackage 注册审计：该提交只给 Android RN < 0.74 的 `TurboPackage` 暴露既有 native manager；Harmony 在第 `38-39 / 155` 笔已明确走 Android/default 的 JS polyfill 路线，即 `KeyboardBackgroundView` 用 `View` fallback、`KeyboardExtender` 用 `KeyboardBackgroundView + KeyboardStickyView + useKeyboardAnimation` 组合，因此不是漏注册 native manager，当前不需要代码改动。
- 已复核第 `91 / 155` 笔动态 `bottomOffset` over-scrolling：上游 `performScrollWithPositionRestoration` 已在早前 `17d373c` 提前同步并验证，本轮不重复改代码。
- 已分析第 `92 / 155` 笔 Reanimated 私有 API 重构：Harmony 分支没有上游要删除的 `event-handler.*` / `event-mappings.ts`，也没有 `react-native-reanimated/src/core` 深导入；当前 `src/reanimated.native.ts` 已用 `useEvent/useHandler` 封装事件，目标风险已覆盖，不需要同步代码。
- 已提交并推送第 `93`、`95 / 155` 笔为 `a31f12f8 同步 useAnimatedKeyboard 兼容接口`：第 93 新增 `src/compat.ts`、`KeyboardState` 常量和 `useAnimatedKeyboard()`，并把内部键盘状态类型改为 `IKeyboardState`；第 95 补 `dismiss(options?: Partial<DismissOptions>)` 注释。第 94 经实测确认依赖当前 selection/caret 坐标，Harmony 原生现阶段不能稳定等价上报，因此明确标记 `KeyboardAwareScrollViewRef.assureFocusedInputVisible()` 接口当前不支持，并撤销 JS command/spec/bindings/context/KASV ref 方法、C++/ArkTS `layoutDidSynchronize` 转发和相关 demo；测试工程和 demo 备份仓已移除 `assureFocusedInputVisible` / `toggle validation + assure` 入口。
- 已分析并同步第 `96-100 / 155` 笔中的可做项：第 96 `70784fd70f` 是纯 JS KASV deps 浅拷贝 warning 修复，Harmony 当前同文件同样复用同一 deps，已同步主仓和测试包 `src` / `lib/commonjs` / `lib/module`；第 97 `59951035af` 是 iOS `KeyboardExtenderContainerView` / `UIResponder.reloadInputViews()` 浮点高度比较修复，Harmony 无 UIKit accessory 链路，不同步；第 98 `acbc7ae418` 是 Android Samsung interactive dismissal 后 `onEnd` 延后一帧读取 `WindowInsetsCompat`，Harmony 不走 Android `WindowInsetsAnimationCallback`，不改 demo；第 99 `afeccd120e` 是 Android/iOS Fabric codegen 名称和 `common/cpp` 路径缩短，Harmony 使用 `rnoh_keyboard_controller` 和手写 `harmony/keyboard_controller/src/main/cpp` 组件注册，单改 codegen name 有破坏风险，不同步；第 100 `e5438be3df` 是 `KeyboardProviderProps` 类型拆分，已按现有 `src/types.ts` 单文件结构最小同步主仓和测试包 `src` / `lib/typescript` 类型产物。本批已提交为 `fd5f3e1a` 并推送。
- 已分析并同步第 `101-105 / 155` 笔中的可做项：第 101 `b5fb83596c` 是纯 JS KAV 条件翻转，从 `OS === "android"` 改为 `OS !== "ios"`。按平台语义优先级复核，Harmony 不能走 iOS 的起始帧目标值语义，因为当前没有 iOS `KeyboardTrackingView` / `keyboardLayoutGuide` / UIKit 协同 layout animation；Harmony C++ 合成 `onMove` 帧，更接近 Android/非 iOS 的逐帧更新，所以已同步主仓 `src/components/KeyboardAvoidingView/hooks.ts`。本轮已补同步测试工程两份 JS 包 `src` / `lib/module` / `lib/commonjs`，用户确认 KAV demo 验证无问题。第 102 `838f546c48` 是 Android `OverKeyboardRootViewGroup.onInterceptHoverEvent` super 调用修复，依赖已存在的键盘上层 overlay root；Harmony 因系统能力缺失不能完整建立该宿主，因此不能单独同步 hover 修复；第 103 `91df02ecd4` 是 iOS hybrid app 中 `KeyboardTrackingView` 挂载 top controller 修复，Harmony 无 UIKit controller/tracking view 链路，不同步；第 104 `68395bf201` 是 Android/iOS Fabric codegen 名继续缩短到 `RNKC` 和 Windows CI，Harmony 手写 C++ 注册且第 99 已不改 codegen name，不同步；第 105 `8041105fea` 是 iOS 手动 did 事件方案，Harmony 已基于 ArkTS will/did 和 keyboardHeightChange 发事件，无 iOS `KeyboardMovementObserver`/`CADisplayLink` 链路，不同步。
- 已分析第 `106-110 / 155` 笔，并专项验证第 108 的 Harmony decorator：第 106 `3589930ca5` 是 Android RN 0.70 编译兼容，把 `ThemedReactContext.keepShadowNodesInSync()` 内直接 `emitDeviceEvent` 改为库内 `emitEvent` extension；Harmony 没有 Android `ThemedReactContext` / Android `keepShadowNodesInSync` / `onUserDrivenAnimationEnded` 链路，ETS 侧 `ctx.rnInstance.emitDeviceEvent` 属于 RNOH TS API，不同步。第 107 `a0d49aeb7a` 新增 `useCombinedRef` 并清理 KASV 内联 ref callback，是可同步纯 JS refactor，不新增接口也不涉及此前不支持的 `assureFocusedInputVisible()`；本轮已同步测试工程两份 JS 包 `src` / `lib/module` / `lib/commonjs` / `lib/typescript`，用户确认 KASV 基础运行验证无问题，已回写主仓 `src/components/hooks/useCombinedRef.ts` 和 `src/components/KeyboardAwareScrollView/index.tsx`，提交为 `ff355e62` 并推送。第 108 `36abe0b8b3` 新增 Android `ClippingScrollViewDecoratorView`，测试工程已按 Harmony native decorator 路线验证：`setClip(false)` 会越界；`clip=true + setPadding(bottom)` 能看到拖拽底部空白，但 `contentSize` 不增加，松手回弹到真实 max offset；给最后 child 加 margin 又不生效。因此当前不改 RNOH core 时不能完整支持上游稳定 bottom inset。第 109 `167a000a9c` 新增 `ScrollViewWithBottomPadding`，依赖第 108 的稳定 `contentInsetBottom`，暂不同步。第 110 `ebc5205903` 是 iOS 26 `KeyboardGestureArea` + `InvisibleAccessoryView` detach 期间 KVO 误发 interactive 的过滤，Harmony 无 `KeyboardEventsIgnorer` / `InvisibleAccessoryView` / `KeyboardTrackingView.interactive` 链路，不同步。
- 已分析第 `111-115 / 155` 笔：第 111 `423dbef67c` 把 `KeyboardAwareScrollView` 从底部假 padding view 切到 `ScrollViewWithBottomPadding` / `contentInset` 路线，并新增 `useScrollState`、`ghostViewSpace`、`removeGhostPadding`；该路线强依赖第 108/109 稳定 bottom inset，而 Harmony 第 108 已实测 root padding 不进入 `contentSize`、松手回弹，因此暂不同步，继续保留当前已验证的 padding view 路线。第 112 `5ef74d9365` 仅重跑 SwiftFormat，不同步。第 113 `ac7dee1c27` 由 iOS focus 后下一帧补发 selection 与 JS `pendingSelectionForFocus` 组成，修的是上游新 KASV 在 focus `onStart` 中使用 stale `lastSelection` 的问题；当前 Harmony 已撤销第 94 公共 ref/command 能力，KASV focus 时仍使用 `input.value` 全输入框布局兜底，不会读取 stale selection；且 Harmony focus 后当前 selection/caret 坐标仍未稳定证明可用，因此暂不同步，后续若重新评估需先做 native 延后一帧 caret 读取试验。第 114 `6c25251470` 是 iOS `KeyboardMovementObserver` manual did / interactive dismissal 调度修复，依赖 UIKit `KeyboardTrackingView`、`KeyboardEventsIgnorer`、`KeyboardAreaExtender` 和 `inputAccessoryView.reloadInputViews()`，Harmony 无同类链路，不同步。第 115 `76056c322d` 修第 94 引入的 `assureFocusedInputVisible()` ref identity 问题；Harmony 当前不导出该 ref 接口，主仓 KASV 无 `useImperativeHandle`，不存在该问题，不同步。
- 已分析第 `116-120 / 155` 笔：第 116 `d1fb606b0b` 是上游 plain RN `Animated` 版 `KeyboardStickyView` 的 Fabric enabled=false graph 修复，Harmony 当前 Reanimated 版本不存在该问题。第 117 `11c0dbfba8` 的基础 `KeyboardChatScrollView` 已同步主仓，并已追加纠正：公共 API、状态机、`ScrollViewWithBottomPadding`、`useScrollState` 和事件注册 helper 均保持上游实现，不再使用 Harmony JS spacer 或替代算法。先前 spacer 真机结果只证明 workaround 可改变布局，不再计为正式组件支持；当前原生 `contentInset` / stable scroll range 缺失使基础组件不能获得等价语义。第 118 `a096e393fd` 是 iOS UIKit swizzle，不影响 Harmony。第 119 `83e34b7069` 继续受相同原生 inset 缺口影响。第 120 `8393ee294f` 只修 Android WindowDimensionListener 引用清理，Harmony 当前无直接同步点。
- 已完成第 `123-125 / 155` 笔：第 123 `66e62784c3` 的 KAV `automaticOffset`、第 124 `f5dca3cdf9` 的 props 类型 union 修复、第 125 `eb2dfbbe35` 的 `viewPositionInWindow` 已作为一个完整功能同步。Harmony 通过 Fabric ShadowTree 测量普通 tag，未修改上游 JS 渲染结构；测试工程真机验证通过。
- 已验证并回写第 `126 / 155` 笔 `6872736017`：KASV 在 `onLayout` 中通过既有 `viewPositionInWindow` 保存真实窗口顶部，顶部回滚条件改为 `point < scrollViewPageY`。固定 Header demo 验证输入框被组件顶部遮挡后重新聚焦会自动恢复可见，`bottomOffset=0/40/80` 保持有效；该提交不新增公开属性或接口。
- 已验证并回写第 `127 / 155` 笔 `4a32305f7e`：JS/TS 新增 `KeyboardToolbar.Group`、类型和 codegen spec；Harmony 新增 `KeyboardToolbarGroupView` descriptor/binder/instance 并注册到 Fabric package。C++ 导航扫描会跳过 Group，组内以最近 Group 为边界，真实焦点使用包含 Group 的深度 DFS，避免组内输入被布局、selection 或键盘事件链漏掉。测试工程保留 `KeyboardToolbar/Group.tsx` 独立 demo，TypeScript、Prettier、Harmony codegen、Metro、CMake/Ninja 和签名 HAP 构建均通过。
- 已分析第 `131-135 / 155` 笔：第 131 `bba6afc9eb`、133 `90dd77289e`、134 `9acd790895` 仍属于 KASV contentInset 优化链，Harmony KASV 未切换该路线，不同步。第 132 `2f82c434c9` 已按上游同步 `flexGrow/flexShrink` 自适应父容器修复，测试工程确认无约束父容器高度非零且随内容变化。第 135 `b28ef3b04d` 依赖尚未同步的 `extraContentPadding/blankSpace`，等对应属性进入测试工程后一起验证。
- 已分析第 `136-140 / 155` 笔：第 136 `33a5d7b585` 是 Android decorator 在 RefreshControl 下递归找真实 `ReactScrollView`；Harmony 当前没有对应原生 decorator，不再使用 JS spacer，因此没有可同步的 native child 查找链路。第 137 `af5da35fb0` 是 iOS 26 compat 专用；第 138 `ba41d5d1ea` 仍依赖未同步的 KASV contentInset/selection 状态机；第 139 `f83e78c04d` 的 keyboardBorderRadius 可按 Android 返回 0，待测试；第 140 `7b9813396c` 是 iOS responder 模式切换专用。
- 已分析第 `141-145 / 155` 笔：第 141 `75981eeb36` 仅新增上游 KASV resize 测试；第 142 `fbedbc2308` 的 KASV `mode` 默认 insets 仍缺 Harmony 等价底座；第 143 `51fbe47b09` 和第 144 `e979dbf426` 分别是 Android Activity listener 与 edge-to-edge 专用。第 145 `1d9a9ac21f` 的 SharedValue `freeze` JS API 后续仍应按上游同步，但该属性冻结/恢复的是当前无法正确形成的 contentInset/offset 状态，因此 Harmony 功能结论统一为暂不支持。
- 已分析第 `146-150 / 155` 笔：第 146 `b3dc83edd5` 的 opening dismiss 已在 Harmony 真机试验后撤销；第 147 `15111a4213` Toolbar 固定 touch bounds 待测试；第 148 `c436fbb9e0` 只修 inverted + FlashList，Harmony JS 不禁用 `inverted`，但原生 top inset/负滚动范围缺失使该组合不能等价实现；第 149 `1048f5238a` 是 Android 专用防崩溃，Harmony 已在自身 `viewPositionInWindow` 中实现 reject + JS fallback；第 150 `ce8ccaa5f7` 的回调 JS 可计算目标 inset，但原生未实际应用，当前不能对外承诺等价状态。
- 已分析第 `151-155 / 155` 笔：第 151 `21a939f30d` 的 Reanimated StickyView 目标已由 Harmony 当前实现覆盖；第 152 `5dc56430d7` 使用的 Reanimated event registration 本身兼容，但 `onEndVisible` 作为 Chat 属性依赖组件产生正确的 viewport/offset/contentSize 关系，当前 contentInset 闭环缺失时不能保证语义，因此统一暂记为不支持，不再列为独立待验证能力。第 153、154 分别是 iOS weak keyboard view 与 Catalyst swizzle guard；第 155 是 Android IME onStart 重入修复，均不适用于当前 Harmony 链路。

## 下一步

`1.16.5..1.21.8` 的 `155` 笔源码相关 commit 已全量审计完毕。KAV `automaticOffset/viewPositionInWindow`、第 `126 / 155` KASV 顶边自动检测、第 `130 / 155` 测量失败降级和第 `132 / 155` 自适应父容器布局修复已完成；后续实际候选为第 `139 / 155` keyboardBorderRadius 和第 `147 / 155` Toolbar touch bounds。ChatScrollView 仍受 contentInset 闭环缺失阻塞，不因本次普通 Fabric tag 测量恢复而改变结论。
