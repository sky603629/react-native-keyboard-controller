# 上游 `1.16.5..1.21.8` Commit 同步台账

> 用途：记录每个上游 commit 是否已经分析、是否已经同步代码、如果修改则修改了什么功能和内容、同步/不同步原因、是否需要测试工程验证。
>
> 规则：以后每处理一个 commit，都先更新本文；不要只凭聊天记录判断进度。

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

截至第 `45 / 155` 笔源码相关 commit 已按时间顺序完成证据审计，最后分析的是 `5ab201112c`。下一笔是第 `46 / 155` 笔：`f963befc1a`。

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
| 14 | `03937a761c` | 2025-03-25 | `feat: OverKeyboardView with custom ShadowNode (#863)` | 已分析-不需同步 | 未同步 | OverKeyboardView 原生 overlay / Fabric ShadowNode 审计 | 无测试工程代码修改。上游主体是 Android/iOS/Fabric 自定义 ShadowNode、CMake、podspec、codegen `interfaceOnly` 和 OverKeyboardView JS 可见性/布局配合。Harmony 当前包没有 Android/iOS/podspec，原生注册列表也没有 `RNKCOverKeyboardView`；`KeyboardControllerView` / `KeyboardGestureArea` 使用 Harmony 自己的 `RNKeyboard...` 组件名和 codegen-harmony 产物，不能直接套用上游 `interfaceOnly` 改动。补充 API 证据：DevEco 离线 API `arkts-apis-window-e.md` 只定义应用子窗口、全局悬浮窗、模态窗、主窗口等 `WindowType`，`AvoidAreaType.TYPE_KEYBOARD` 定义为“固定态软键盘区域”；`arkts-apis-window-window.md` 提供 `getWindowAvoidArea(TYPE_KEYBOARD)`、`keyboardHeightChange`、API 20+ `keyboardWillShow/Hide`，这些都是键盘区域/事件读取能力；`arkts-apis-window-windowstage.md` 提供 `createSubWindow` / `createSubWindowWithOptions` 创建应用子窗口。未发现公开 API 可把业务 RN 子树挂载到系统输入法窗口层级、随键盘窗口 attach/detach，或取得键盘窗口宿主 view 用来实现上游 `OverKeyboardView` 的同类 overlay。 | Harmony 当前没有 OverKeyboardView 原生 overlay 能力。仅同步 JS 可见性无法让组件可用，反而可能掩盖原生未实现问题；本轮记录为不需同步。若未来要实现，实际路线不是小 patch：需要新增 `RNKCOverKeyboardView` Harmony 原生组件，或基于 `WindowStage.createSubWindow*` 做应用子窗口方案，并解决 RN 子树挂载、窗口层级、键盘显隐/高度跟随、生命周期和焦点输入法交互。 | 暂不需要；若后续启用 OverKeyboardView demo，应验证 `visible=false` 时 children 不挂载、`visible=true` 时 overlay 尺寸正确、键盘显示/隐藏/旋转时 overlay 跟随且不抢焦点。 |
| 15 | `87e47e556f` | 2025-03-28 | `Update correct path of RNKCOverKeyboardViewComponentDescriptor.h in … (#886)` | 已分析-不需同步 | 未同步 | iOS Fabric header path 审计 | 无代码修改。上游只改 iOS `.mm` import 路径和 podspec `header_dir`，从 `<reactnativekeyboardcontroller/...>` 改为 `<react/renderer/components/reactnativekeyboardcontroller/...>`。 | 本仓无 `ios/`、无 `react-native-keyboard-controller.podspec`，Harmony C++ include 路径使用本地 codegen-harmony 头文件，不存在该 iOS header_dir 问题。 | 不需要。 |
| 16 | `d379ff5f21` | 2025-03-29 | `fix: KeyboardAvoidingView + autoFocus + native-stack (#880)` | 已分析 | 代码仓已推送 | KeyboardAvoidingView 初始 frame 更新规则 | 已同步测试工程 JS 三处：`src/components/KeyboardAvoidingView/index.tsx`、`lib/module/.../index.js`、`lib/commonjs/.../index.js`；已回写代码仓 `src/components/KeyboardAvoidingView/index.tsx`，本仓提交 `f75a60a` 推送到 `origin/ups`。`onLayoutWorklet` 改为当 `keyboard.isClosed`、`initialFrame` 为空或 `behavior !== "height"` 时刷新 `initialFrame`，依赖项增加 `behavior`；测试工程 KAV demo 增加 `autoFocus position` 按钮。 | 功能效果：没有新增接口/props，是内部行为能力增强。`position` / `padding` / `translate-with-padding` 这类非 `height` 行为，在 autoFocus 或导航首帧布局变化后继续刷新 frame，避免用旧 frame 计算键盘避让；`height` 行为保持打开键盘时不重写初始高度，避免高度越算越小。纯 JS/Reanimated 行为修复，无原生侧对应修改。 | 已验证 `autoFocus position` 可生效。 |
| 17 | `b27ed8a284` | 2025-04-01 | `feat: useKeyboardState (#894)` | 已分析 | 代码仓已推送 | 新增 `useKeyboardState` hook / `KeyboardController.state()` 默认状态 | 已同步代码仓 `src/hooks/index.ts`、新增 `src/hooks/useKeyboardState/index.ts`，并修改 `src/module.ts` / `src/types.ts`；已同步测试工程 `@react-native-ohos` 包源码和 ESM/CJS/d.ts 产物；测试 demo 新增 `Hook_useKeyboardState` 入口。本仓提交 `ce10428` 已推送到 `origin/ups`。 | 功能效果：业务可通过 `useKeyboardState()` 响应键盘显隐并获得 `height/duration/timestamp/target/type/appearance/isVisible`；`KeyboardController.state()` 从 nullable 改为始终返回默认键盘事件数据，避免调用方初始态必须判空。纯 JS hook/API 改动，依赖现有 Harmony `keyboardDidShow/keyboardDidHide` 事件回流，无新增原生接口。 | 已验证 `Hook_useKeyboardState` 没问题。 |
| 18 | `13dfb32c54` | 2025-04-04 | `fix: set delegate to all UITextView (#902)` | 已分析-不需同步 | 未同步 | iOS UITextView delegate 兼容审计 | 无测试工程代码修改。上游只改 iOS：把 `RCTUITextView+DelegateManager` 改为 `UITextView+DelegateManager`，并让 `FocusedInputObserver.swift` 对基础 `UITextView` 设置 force delegate，解决三方 UITextView 子类和 dynamic frameworks 下 `RCTUITextView` 内部类不可见问题。 | Harmony 没有 UIKit `UITextView` / `RCTUITextView` / Objective-C category / Swift delegate 链路；当前 focused input 事件来自 Harmony ArkTS/C++ 节点与 RNOH 事件，不存在“只对 RN 私有 TextView 子类设置 delegate”这条路径。本批不补原生空实现。 | 不需要单独 demo；后续 focused input 回归仍覆盖普通 TextInput、TextArea/多行、三方输入组件场景。 |
| 19 | `820fcb4b9d` | 2025-04-07 | `feat: KeyboardStickyView using plain Animated (#898)` | 已分析 | 未同步 | KeyboardStickyView 动画实现切换 | 已尝试同步到测试工程 `@react-native-ohos` 包源码和 ESM/CJS 产物，但测试观察到表现有问题；已按要求回退测试工程和代码仓 `KeyboardStickyView` 修改，保留 Reanimated 版本用于对比验证。 | 功能效果目标：减少 StickyView/Toolbar 对 Reanimated worklet 的依赖，规避上游记录的 RN/Reanimated 组合下影响其它组件行为的问题。当前 Harmony 测试表现需继续分析，暂不提交。纯 JS 动画实现替换，无原生侧对应修改。 | 待重新验证和分析问题原因后再同步。 |
| 20 | `0c6ae77362` | 2025-04-09 | `fix: useKeyboardState tweaks (#907)` | 已分析 | 代码仓已推送 | `useKeyboardState` 与 `state()` 语义修正 | 已与 `b27ed8a284` 合并同步到代码仓和测试工程：`KeyboardController.state()` 只返回 `KeyboardEventData`，不再包含 `isVisible`；`useKeyboardState()` 内部通过 `KeyboardController.isVisible()` 聚合出最终 `KeyboardState`。 | 功能效果：保持 imperative 的 `KeyboardController.state()` 和 reactive 的 `useKeyboardState()` 解耦；hook 仍向业务返回完整聚合状态。纯 JS 类型/逻辑修正，无原生侧对应修改。 | 已随 `Hook_useKeyboardState` demo 验证。 |
| 21 | `c44e7e7e31` | 2025-04-11 | ``fix: `OverKeyboardView` crash (#913)`` | 已分析-不需同步 | 未同步 | OverKeyboardView touch 防崩审计 | 无测试工程代码修改。上游仅改 Android `OverKeyboardViewGroup.kt`：在 `onInterceptTouchEvent` / `onTouchEvent` 中把 `jsTouchDispatcher.handleTouchEvent` 和 `jsPointerDispatcher.handleMotionEventCompat` 包进 `try/catch`，捕获 `RuntimeException` 并记录 warn。 | 上游问题语义是 Android overlay root view 尚未完全布局或 touch 落到非 overlay 区域时，touch 事件向 RootView 传播触发 `ViewRootImpl`/`ViewGroup` 类型链路崩溃。Harmony 当前没有注册 `RNKCOverKeyboardView` 原生 overlay，前序 `03937a761c` 已记录系统能力/原生组件缺口；本 commit 没有可直接同步的 Harmony touch dispatch 代码，也不新增空实现。 | 当前不测；未来若实现 Harmony OverKeyboardView，需要把“快速点击/未布局完成/非 overlay 区域触摸不崩溃”列入验收。 |
| 22 | `033e043774` | 2025-04-15 | `fix: Square payment SDK integration (#900)` | 已分析-不需同步 | 未同步 | iOS Square SDK delegate 兼容审计 | 无测试工程代码修改。上游只改 iOS：`KCTextInputCompositeDelegate` 增加 `canSubstituteTextFieldDelegate`，当原 delegate 类型名包含 `SQIPTextFieldInputModifier` 时不替换 delegate；`FocusedInputObserver` 在替换 `UITextField.delegate` 前调用该判断。 | 上游问题语义是 Square SDK 私有 `SQIPTextFieldInputModifier` 会把输入事件转发回注入的 keyboard-controller delegate，导致递归/崩溃。Harmony 没有 UIKit `UITextFieldDelegate` / Objective-C/Swift delegate 注入链路，当前 focused input 监听走 Harmony/RNOH 节点事件与 ArkUI 输入组件，不存在该私有 iOS delegate 递归路径。 | 不需要单独 demo；若未来接入鸿蒙支付 SDK 的自定义输入控件，再按实际控件事件链路做兼容审计。 |
| 23 | `9011a90a78` | 2025-04-17 | `fix: disabling KeyboardAwareScrollView didn't prevent useAnimatedStyle flickering (#921)` | 已分析 | 代码仓已推送 | KeyboardAwareScrollView `enabled=false` 禁用语义 | 已同步测试工程 `@react-native-ohos` 包源码和 ESM/CJS 产物；已回写代码仓 `src/components/KeyboardAwareScrollView/index.tsx`，本仓提交 `c2a9838` 已推送到 `origin/ups`。`KeyboardAwareScrollView` 在 `enabled=false` 时不再渲染底部 `<Reanimated.View style={view} />`。 | 功能效果：禁用 KASV 时彻底移除由 `useAnimatedStyle` 驱动的 padding view，避免 Fabric/Reanimated style 值不同步导致页面闪烁或 modal 高度异常；启用时原键盘避让逻辑保持不变。纯 JS 侧修改，无原生侧对应修改。 | 已确认没什么问题。后续回归：`enabled=false` 时页面不应产生额外底部 padding / flicker；`enabled=true` 时输入框聚焦仍能正常滚动避让。 |
| 24 | `0071eb058d` | 2025-04-17 | `fix: findNodeHandle on the web platform (#912)` | 已分析 | 代码仓已推送 | `findNodeHandle` 平台封装 | 已同步测试工程 `@react-native-ohos` 包源码、ESM/CJS 和 d.ts 产物；已回写代码仓新增 `src/utils/findNodeHandle/index.native.ts`、`src/utils/findNodeHandle/index.ts`，并让 KASV 改为从该 util 导入，本仓提交 `c2a9838` 已推送到 `origin/ups`。当前鸿蒙分支 `src/internal.ts` 没有上游 `useEventHandlerRegistration/findNodeHandle` 使用点，因此不额外添加未使用 import。 | 功能效果：native/Harmony 解析到 `.native` 时仍使用 RN `findNodeHandle`，行为不变；web 默认入口直接返回 handle/null，避免新版 `react-native-web` 抛错。对鸿蒙是结构对齐和未来兼容，非原生能力变更。 | 已确认没什么问题。后续回归：KASV 聚焦滚动仍能正确识别 ScrollView target；打包解析 `utils/findNodeHandle` 不报错。 |
| 25 | `c8a08ce2b8` | 2025-04-18 | `fix: OverKeyboardView rotation (fabric) (#918)` | 已分析-不需同步 | 未同步 | OverKeyboardView 旋转尺寸审计 | 无测试工程代码修改。上游仅改 Android `OverKeyboardViewGroup.kt`：把 `stateWrapper` 代理到 `hostView`，在 `OverKeyboardRootViewGroup.onAttachedToWindow/onDetachedFromWindow/onSizeChanged` 中调用 `stretchTo(width,height)` 更新 `screenWidth/screenHeight`，解决 Fabric 下设备旋转后 overlay ShadowNode 尺寸不同步。 | 上游问题语义是 Android 原生 overlay 已存在但旋转时 ShadowNode layout 没更新。Harmony 当前没有注册/实现 `RNKCOverKeyboardView` 原生 overlay，前序已确认不新增空原生实现；因此没有可落地的 Harmony 代码路径。未来若实现 OverKeyboardView，需把窗口/节点尺寸变化同步列为原生组件验收项。 | 不需要。 |
| 26 | `59dddfee31` | 2025-04-21 | `fix: App Store compatible icons (#926)` | 已分析-不需同步 | 未同步 | iOS example 图标资产 | 无测试工程代码修改。上游批量替换 iOS example / FabricExample / KeyboardControllerNative 的 AppIcon PNG 和 `Contents.json`，并调整 `cspell.json`。 | 该 commit 解决 App Store 提交时 iOS 图标透明边缘问题，只影响上游 iOS 示例工程发布资产；Harmony 库功能、JS API、ETS/C++ 原生实现均不受影响。 | 不需要。 |
| 27 | `661b39e24c` | 2025-05-16 | `docs: better JSDoc (#942)` | 已分析-不需同步 | 未同步 | JSDoc / 类型组织审计 | 无测试工程代码修改。上游增加 `eslint-plugin-jsdoc` 规则，补充大量公开 API JSDoc，并把 `src/types.ts` 拆成 `src/types/index.ts`、`hooks.ts`、`module.ts`、`internal.ts`、`views.ts`。 | 标题虽为 docs，但确实触达 `src`。本轮按要求不进行同步；该改动主要影响 IDE 文档提示、类型文件组织和 lint 规则，不改运行时功能，也不需要 Harmony 原生侧适配。后续若同步上游类型结构，需要单独批次处理，避免和行为修复混合。 | 不需要。 |
| 28 | `be7ef24f76` | 2025-05-20 | `fix: rely on screen dimensions instead of window dimensions (#948)` | 已分析 | 未同步 | `useWindowDimensions` 尺寸来源审计 | 无测试工程代码修改。上游把 `src/hooks/useWindowDimensions/index.ts` 从直接导出 RN `useWindowDimensions` 改为自定义 hook：初始值取 `Dimensions.get("screen")`，订阅 RN `Dimensions.addEventListener("change")`，并使用 `e.screen.width/height` 更新。 | 上游问题语义是 iOS 旋转时 RN window 尺寸可能滞后，screen 尺寸更可靠。但该方案随后被 `427374fcb7` 修正为 `WindowDimensionsEvents.windowDidResize`，说明单独同步 `be7ef24f76` 是已知中间态。Harmony 当前 `index.ts` 仍走 RN `useWindowDimensions`，`index.android.ts` 才走 `WindowDimensionsEvents`；需结合 Harmony resolver 和原生 resize 事件能力一起判断，暂不同步。 | 不测试中间态。 |
| 29 | `ba2e187a6c` | 2025-05-22 | `fix: keep shadow nodes in sync (#950)` | 已分析-不需同步 | 未同步 | Android native-driver ShadowNode 同步审计 | 无测试工程代码修改。上游 Android 新增 `ThemedReactContext.keepShadowNodesInSync(viewId)`，在键盘动画结束和 `keyboardDidShow` 后发送 `onUserDrivenAnimationEnded`，让 RN Animated/Fabric 侧重新同步 shadow tree，解决 `KeyboardStickyView` / `KeyboardToolbar` 内 `Pressable` 因 native-driver 动画后 ShadowNode 位置不同步而点不到的问题。 | 该 commit 依赖 Android native-driver + RN Animated/Fabric 的 `onUserDrivenAnimationEnded` 机制；Harmony 当前没有同名事件链路，且我们已决定不同步 `820fcb4b9d` 的 KSV plain Animated 改造，保留 Reanimated 版本并做了 Harmony start 阶段修复。因此当前没有等价代码需要同步。若未来重新迁移 plain Animated，再重新审计此链路。 | 不需要。 |
| 30 | `c5c00778f2` | 2025-05-23 | ``fix: support dynamic `bottomOffset` for KeyboardAwareScrollView (#952)`` | 已分析 | 代码仓已推送 | KeyboardAwareScrollView 动态 `bottomOffset` | 已同步测试工程并验证动态 offset 正常；已回写代码仓 `src/components/KeyboardAwareScrollView/index.tsx`，本仓提交 `17d373c` 推送到 `origin/ups`。同步 `useEffect` + `runOnUI` 触发 `bottomOffset` 变化后的重新滚动计算，并结合提前同步的 `852fa4a223` 使用 `performScrollWithPositionRestoration(scrollBeforeKeyboardMovement.value)` 防止二次叠加。 | 功能效果：键盘已打开、焦点输入框仍在当前 ScrollView 内时，业务动态改变 `bottomOffset` 后会立即重新计算滚动距离；从 `50` 切到 `0` 不再继续向上过度滚动。纯 JS/Reanimated 行为修复，无原生侧修改。 | 已在测试工程验证动态 offset 正常。 |
| 31 | `427374fcb7` | 2025-05-25 | `fix: wrong useWindowDimensions on iPad (#957)` | 已分析 | 未同步 | `useWindowDimensions` 原生 resize 事件链路审计 | 无测试工程代码修改。上游删除 `index.android.ts`，让所有平台统一在 `index.ts` 订阅 `WindowDimensionsEvents.addListener("windowDidResize")`；iOS 原生在 `KeyboardControllerView.layoutSubviews` 中读取 `UIScreen.main.bounds.size`，尺寸变化时发射 `KeyboardController::windowDidResize`，并把该事件加入 iOS supported events。 | 上游问题语义是 `be7ef24f76` 的 RN `Dimensions` screen 监听仍不能覆盖 iPad 场景，于是改为由原生 `KeyboardControllerView` layout 变化发 `windowDidResize`。Harmony 当前 `Type.ts` / `supportListeners()` 白名单没有 `KeyboardController::windowDidResize`，ETS/C++ 也未发现该事件发射路径；直接同步 JS 会订阅一个收不到的事件，可能让尺寸 hook 初始值停留在 `Dimensions.get("screen")` 且后续不更新。暂不同步，需单独设计 Harmony window resize 事件后再处理。 | 暂不测试。后续若实现，应验证旋转、分屏/折叠、页面尺寸变化时 KAV/KASV/OverKeyboardView 尺寸同步。 |
| 32 | `e431917472` | 2025-05-26 | ``fix: `KeyboardAvoidingView` incorrect layout (#954)`` | 已分析 | 代码仓已推送 | KeyboardAvoidingView `height` 行为防 0 高度 | 已同步测试工程并回写代码仓 `src/components/KeyboardAvoidingView/index.tsx`，本仓提交 `17d373c` 推送到 `origin/ups`。在 KAV animated style 中先计算 `const height = frame.value.height - bottomHeight`，`behavior="height"` 时只有 `!keyboard.isClosed.value && height > 0` 才返回 `{ height, flex: 0 }`。 | 功能效果：避免 KAV 在键盘打开或 layout 边界情况下返回 `height: 0` / 负数；上游说明 RN 对 `{height:0}` 再切回 `{}` 时样式不会正确恢复，导致布局错误。纯 JS/Reanimated 行为修复，无原生侧修改。 | 已在测试工程验证无明显问题；极端小容器 guard 场景内容不显示属于测试场景压缩效果，重点是关闭后不永久塌陷。 |
| 33 | `331293a9cc` | 2025-06-09 | `fix: 2 memory leaks (#966)` | 已分析-不需同步 | 未同步（已回退 `0b03f32`） | Android memory leak 审计 | 无代码修改。Android 原提交修复 `ModalAttachedWatcher` 和 `WindowDimensionListener` 未 detach 的内存泄漏。Harmony 没有这两个 Android 类，也没有 `ReactApplicationContext` + Android view attach watcher / window dimension listener 的同名持有链路。此前按“同类生命周期风险”尝试把 Harmony `keyboardHeightChange` / `keyboardWillShow` / `keyboardWillHide` 改为稳定 handler、补 TurboModule `__onDestroy__` 和 C++ view 析构清理，但测试发现会导致多类键盘事件异常，说明该改动扩大了原 commit 范围并破坏了 Harmony 当前事件注册/分发时序。 | 最终判断：鸿蒙当前不存在上游 `331293a9cc` 修复的 Android 同类问题，不按该 commit 修改。`0b03f32` 的代码改动已从主仓和测试工程回退；后续若单独发现 Harmony listener 泄漏，需要基于实际日志/复现另开专项修复，不能挂在此 Android commit 下。 | 已在测试工程回退验证方向；由用户反馈“很多事件都不正常”触发撤回。 |

## 第 30-37 / 155 平台专用补充审计

> 这一段补齐主线第 30-37 笔源码 commit 的实际分析结果，避免把 Android-only / iOS-only 简单当作跳过依据。完整状态仍以 `UPGRADE-SOURCE-COMMIT-CHECKLIST-155.md` 为准。

| 源码顺序 | 上游 commit | 结论 | Harmony 等价风险分析 / 处理方案 |
| ---: | --- | --- | --- |
| 30 | `76226df4d9` | 已分析-不需同步 | Android 修 `OverKeyboardView` accessibility 崩溃：overlay children 位于 `WindowManager.addView` 的独立 window，却被 host 暴露给无障碍树，TalkBack descendant 校验失败。Harmony 包注册表 `keyboardControllerPackage.cpp/.h` 只有 `RNKeyboardControllerView` / `RNKeyboardGestureArea`，当前没有 `RNKCOverKeyboardView`、overlay host 或 accessibility children 转发链路；不改代码。未来实现 `OverKeyboardView` 时必须覆盖无障碍遍历。 |
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

> 这一节是早期 `83` 关键清单之外的补充审计池，保留用于提醒 Android-only / iOS-only 风险。后续推进和进度统计必须以 `UPGRADE-SOURCE-COMMIT-CHECKLIST-155.md` 的 `155` 总清单为准。

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
| `aff3cbe7d1` | 2025-08-12 | 触达 JS/TS | `fix: fix disabling KeyboardAvoidingView (#1078)` | 查 KAV `enabled=false` 行为；Harmony JS 必须同步或验证。 | 待补充审计 |
| `1d7721b9c7` | 2025-08-13 | iOS-only | `fix: avoid direct FBReactNativeSpec usage (#1084)` | 查 codegen/深导入稳定性；Harmony 是否有等价 RNOH spec 直连风险。 | 待补充审计 |
| `2efe655939` | 2025-08-13 | Android-only | `fix: add target_compile_reactnative_options (#1085)` | 查构建参数修复语义；Harmony CMake/Native build 是否需要等价编译选项。 | 待补充审计 |
| `38217e6671` | 2025-08-27 | Android-only | `fix: clear legacy full screen flag (#1098)` | 查全屏/窗口 flag 与键盘避让关系；Harmony 系统避让模式切换是否需恢复旧状态。 | 待补充审计 |
| `1edf41f1cb` | 2025-08-29 | Android-only | `fix: don't use @RequiresApi with old SDK versions (#1107)` | 查 API 版本隔离；Harmony ArkTS/C++ API 版本判断是否完整。 | 待补充审计 |
| `2a3bcee5ec` | 2025-09-08 | 触达 JS/TS | `fix: KeyboardExtender width on iPad in split mode (#1113)` | 查 JS 尺寸/窗口宽度使用；Harmony 平板/分屏/折叠屏场景是否有同类宽度问题。 | 待补充审计 |
| `f2d74b3bc7` | 2025-09-26 | iOS-only | `fix: KeyboardExtender initial mount on Fabric (#1135)` | 查 Fabric 初挂载时机；Harmony native component 首次 mount 是否有空节点/延迟布局问题。 | 待补充审计 |
| `8d3726d387` | 2025-10-01 | Android-only | `chore: react-native 0.80 (#979)` | 查是否仅依赖升级；若含 Android 源码兼容点，映射 Harmony RN 0.82 基线。 | 待补充审计 |
| `4ab7be0c15` | 2025-10-13 | iOS-only | `fix: crash when unrecognized selector sent (#1153)` | 查崩溃根因是否为可选方法/版本 API；Harmony API 调用是否做能力判断。 | 待补充审计 |
| `6503e23efd` | 2025-10-14 | iOS-only | `fix: iOS 26 + Modal (#1158)` | 查 Modal 场景键盘/窗口坐标；Harmony 弹窗、子窗口或页面栈是否有同类问题。 | 待补充审计 |
| `3ab39a0edd` | 2025-10-20 | iOS-only | `fix: attach keyboard tracking view in advance and only when window has been changed (#1170)` | 查 tracking view attach 条件；Harmony 跟随键盘视图挂载时机需对应验证。 | 待补充审计 |
| `99be9d3011` | 2025-10-27 | iOS-only | `fix: avoid ensureLayout calls (#1174)` | 查强制布局调用副作用；Harmony `syncUpLayout` / area-change 是否会造成重复测量或抖动。 | 待补充审计 |
| `6e4b14d1e9` | 2025-11-03 | Android-only | `fix: match StatusBar and edge-to-edge interop according to latest spec (#1185)` | 查 edge-to-edge / 状态栏 / window insets；Harmony 系统避让模式和窗口高度是否同类。 | 待补充审计 |
| `0dcf4b4d41` | 2025-11-03 | iOS-only | `fix: non-working release version on iOS 26 (#1184)` | 查是否为 iOS release/runtime 条件；Harmony 若无对应发布路径可记录不需同步。 | 待补充审计 |
| `b0e60f6b32` | 2025-11-04 | iOS-only | `fix: attaching KeyboardTrackingView (#1193)` | 查 tracking view attach 生命周期；Harmony 等价视图需验证窗口变化和销毁态。 | 待补充审计 |
| `1441ae3b59` | 2025-11-06 | Android-only | `fix: add KeyboardBackgroundView to turbo package (#1197)` | 查新原生组件是否进入包注册；Harmony 新增 BackgroundView 时需同步注册。 | 待补充审计 |
| `70784fd70f` | 2025-12-03 | 触达 JS/TS | `fix: dependencies array changes size between renders (#1229)` | 查 React hooks 依赖数组稳定性；Harmony JS 必须同步或确认已覆盖。 | 待补充审计 |
| `59951035af` | 2025-12-09 | iOS-only | `fix: avoid undesired reloadInputViews during keyboard dismissal (#1235)` | 查 dismiss 过程中是否误触发键盘重载/重开；Harmony hide/keepFocus 需验证。 | 待补充审计 |
| `acbc7ae418` | 2025-12-12 | Android-only | `fix: wrong onEnd event on Samsung devices when keyboard gets shown after interactive dismissal (#1240)` | 查键盘 end 事件时序；Harmony 合成 move/end 是否有同类异常。 | 待补充审计 |
| `afeccd120e` | 2025-12-17 | 原生混合 | `fix: shorter codegen name (#1251)` | 查 codegen name 长度与 Harmony 生成物/包名限制。 | 待补充审计 |
| `e5438be3df` | 2025-12-22 | 触达 JS/TS | `refactor: move KeyboardProvider props in separate file (#1255)` | 查类型组织和导入路径；Harmony JS 分叉需同步或保留兼容。 | 待补充审计 |
| `b5fb83596c` | 2025-12-23 | 触达 JS/TS | `refactor: change conditional code in KeyboardAvoidingView (#1257)` | 查 KAV 条件分支行为是否变化；Harmony JS 需对齐。 | 待补充审计 |
| `838f546c48` | 2025-12-30 | Android-only | `Call correct onInterceptHoverEvent super in onInterceptHoverEvent() (#1260)` | 查是否仅 Android hover 分发；Harmony 若无同类手势/hover 拦截可记录不需同步。 | 待补充审计 |
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
| `d4f1a4cf91` | 2026-03-07 | 触达 JS/TS | `fix: access before initialization error (#1342)` | 查初始化顺序；Harmony JS 需同步或确认无同类访问时机。 | 待补充审计 |
| `f5dca3cdf9` | 2026-03-11 | 触达 JS/TS | `fix: KeyboardAvoidingView types union (#1354)` | 查公开 TS 类型；Harmony 类型需对齐。 | 待补充审计 |
| `eb2dfbbe35` | 2026-03-12 | 触达 JS/TS | `fix: handle broken measureInWindow (#1355)` | 查测量失败保护；Harmony `viewPositionInWindow` / layout sync 需同类降级。 | 待补充审计 |
| `7a7b6cab6d` | 2026-03-16 | iOS-only | `fix: typo in arch definition (#1367)` | 查是否影响 Fabric 架构定义；Harmony JS 架构检测需确认。 | 待补充审计 |
| `ecd3bbd8e3` | 2026-03-18 | 触达 JS/TS | `fix: could not fing view for tag warning (#1379)` | 查 tag 找不到时的日志/保护；Harmony parent ScrollView / focused input 查找需同类防御。 | 待补充审计 |
| `bba6afc9eb` | 2026-03-19 | 触达 JS/TS | `perf: don't change currentKeyboardFrame each frame (#1381)` | 查键盘帧对象更新策略；Harmony 合成 move 是否造成无谓 re-render/shared value 抖动。 | 待补充审计 |
| `90dd77289e` | 2026-03-21 | 触达 JS/TS | `fix: KeyboardAwareScrollView regression after optimization (#1387)` | 查 KASV 优化后的回归；Harmony bottomOffset/KASV 必须重点验证。 | 待补充审计 |
| `9acd790895` | 2026-03-23 | 触达 JS/TS | `fix: remove unnecessary code (#1389)` | 查是否纯清理；若无行为改动可记录不需同步。 | 待补充审计 |
| `33a5d7b585` | 2026-03-24 | Android-only | `fix: KeyboardChatScrollView + RefreshControl conflict (#1398)` | 查 ChatScrollView 与下拉刷新冲突语义；Harmony 实现 ChatScrollView 时需验证。 | 待补充审计 |
| `af5da35fb0` | 2026-03-26 | iOS-only | `fix: respect UIDesignRequiresCompatibility in KeyboardExtenderCont… (#1402)` | 查兼容模式下 Extender 布局；Harmony 是否有类似兼容显示模式。 | 待补充审计 |
| `ba41d5d1ea` | 2026-03-27 | 触达 JS/TS | `fix: KeyboardAwareScrollView re-focus after hardware keyboard dismissal (#1403)` | 查硬件键盘/重新聚焦后的 KASV 状态；Harmony 需验证 keyboard height 0 后 refocus。 | 待补充审计 |
| `7b9813396c` | 2026-03-31 | iOS-only | `fix: call syncUpLayout on iOS when keyboard changes its mode (emoji vs text) (#1417)` | 查键盘模式切换高度/布局变化；Harmony 输入法切换文本/表情时是否触发布局同步。 | 待补充审计 |
| `75981eeb36` | 2026-04-01 | 触达 JS/TS | `test: cover keyboard resize with unit test (#1418)` | 查测试覆盖的行为是否 Harmony 也应验证：键盘 resize / emoji 切换 / 高度变化。 | 待补充审计 |
| `51fbe47b09` | 2026-04-06 | Android-only | `fix: WindowDimensionListener behavior when Activity restarts (#1422)` | 查窗口尺寸监听重启/重载；Harmony ArkTS window listener 是否在页面/JS reload 后恢复。 | 待补充审计 |
| `e979dbf426` | 2026-04-10 | Android-only | `fix: don't modify edge-to-edge mode by default when module toggled on/off (#1412)` | 查启停观察器时是否恢复系统避让/窗口模式；Harmony start/stop observer 必须验证恢复原值。 | 待补充审计 |
| `15111a4213` | 2026-04-17 | 触达 JS/TS | `fix: pass through touches in KeyboardToolbar (#1440)` | 查 Toolbar 触摸穿透行为；Harmony UI 需对齐。 | 待补充审计 |
| `c436fbb9e0` | 2026-04-20 | 触达 JS/TS | `fix: bug with KeyboardChatScrollView + inverted + maintainVisibleScrollPosition + FlashList (#1437)` | 查 ChatScrollView 倒置列表/FlashList 兼容；Harmony 实现 ChatScrollView 时需纳入验收。 | 待补充审计 |
| `1048f5238a` | 2026-04-27 | Android-only | `fix: resolve view crash (#1444)` | 查原生 view 生命周期崩溃根因；Harmony native component 销毁/空节点保护需核对。 | 待补充审计 |
| `1a6450ef35` | 2026-05-15 | iOS-only | `fix: use weak reference to keyboard view on iOS (#1465)` | 查循环引用/悬垂引用；Harmony C++ weak/shared pointer、event emitter 生命周期需核对。 | 待补充审计 |
| `cb011a80ff` | 2026-05-16 | iOS-only | `fix: crash on catalyst (#1464)` | 查是否为 Catalyst 专用；若无 Harmony 对应运行形态可记录不需同步。 | 待补充审计 |

## 处理记录

### 2026-07-24

- 已确认 `2a923a6962` 属于 JS 架构检测同步，不代表 Harmony 支持 `OverKeyboardView` overlay 原生能力。
- 已确认 `b8a4de91fd` 只同步源码类型改动，不同步 RN 0.78 依赖升级。
- 已同步并推送 `2b21634df9`；已分析 `1714227edd`、`3a366a3a3b` 为不需同步。
- 已在测试工程同步 `8eb6a7be14`；已补充审计 `41d798532d` 并记录当前不需同步；已确认 `18e550c898` 为 release commit，不需同步。
- 已同步并推送 `8eb6a7be14`、`5b65344e6b`、`efd038cba2`、`9080f313be`、`cb8161cf00`。
- 已分析 `e1a4cc4e58`、`03937a761c`、`87e47e556f` 为当前不需同步；已在测试工程同步 `d379ff5f21`，等待验证。
- 已同步并验证 `b27ed8a284` + `0c6ae77362` 的 `useKeyboardState`；`820fcb4b9d` 的 `KeyboardStickyView` plain Animated 变更在测试工程观察到问题，已回退测试工程修改，暂不提交；已补充审计 `13dfb32c54` 为 iOS delegate 专用，不需 Harmony 代码同步。
- 已补充审计 `c44e7e7e31`、`033e043774` 为平台专用且当前 Harmony 无对应可落地链路；已同步并推送 `9011a90a78`、`0071eb058d`，本仓提交 `c2a9838`。
- 按要求不进行同步 `c8a08ce2b8`、`59dddfee31`、`661b39e24c`；已补充不需同步原因并推进总清单到 `24 / 155`。
- 已分析 `be7ef24f76`、`ba2e187a6c`、`c5c00778f2`、`427374fcb7`、`e431917472`：尺寸 hook 链路暂不同步；Android ShadowNode 同步当前不需同步；`c5c00778f2`、`e431917472` 进入测试工程同步候选。
- 已验证并推送 `c5c00778f2`、`e431917472`；因动态 `bottomOffset` 测试复现 over-scrolling，提前同步并验证 `852fa4a223`，本仓提交 `17d373c`。
- 已补充审计第 `30-37 / 155` 笔：`76226df4d9`、`49898cd5eb`、`98df8d88e2`、`331293a9cc`、`371cf7baef`、`fb95fc1364`、`9de91159df`、`c8398fc0e3` 经 Harmony 对应链路分析后当前不需同步；其中 `331293a9cc` 曾按同类风险提交 `0b03f32`，测试发现事件异常后已回退，最终记录为 Harmony 无同类问题。
- 已同步并验证第 `38-39 / 155` 笔：`KeyboardBackgroundView` 在 Harmony 以 JS `View` fallback 模拟键盘背景；`KeyboardExtender` 采用与 Android 一致的 `KeyboardBackgroundView + KeyboardStickyView + useKeyboardAnimation` polyfill。文档明确该方案和 iOS 原生 accessory / 私有键盘背景材质能力不一致。
- 已同步并推送第 `40-41 / 155` 笔：`useKeyboardState(selector)` 与去除 JS `monkey-patch` 深导入依赖；已审计第 `42 / 155` 笔 iOS `shouldIgnoreKeyboardEvents` 复位问题，Harmony 无同类 UIKit responder/accessory 状态机，当前不改代码。
- 已审计第 `43 / 155` 笔 Android `StatusBarModule` 反射修复，Harmony 使用本地 `StatusBarManagerCompat` TurboModule，无 Android Kotlin internal/reflection 同类问题，当前不改代码。
- 已分析第 `44 / 155` 笔 `keyboardAppearance` 外观来源改造，实测发现能力依赖 RNOH 框架 `TextInputComponentJSIBinder` 暴露 `keyboardAppearance` native prop；当前鸿蒙框架未暴露该接口，JS 传入 `light/dark` 时本库 C++ 只能读到 `default`。按“不修改框架代码”的策略，本仓不适配该 commit，记录为鸿蒙当前无可用暴露接口。
- 已在测试工程同步第 `45 / 155` 笔 KASV selection 驱动滚动：用 `onSelectionChange` 的 caret y 替代 `onChangeText` 主驱动，并清理测试工程 KASV 旧调试日志，待验证。

## 下一步

当前待测试候选：第 `45 / 155` 笔 `5ab201112c`。下一笔主线源码审计是第 `46 / 155` 笔 `f963befc1a`。第 `91 / 155` 笔 `852fa4a223` 已提前同步，后续走到该位置时只需复核记录和回归。
