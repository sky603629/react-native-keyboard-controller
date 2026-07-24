# 上游源码相关 Commit 总清单（155 笔）

本文是当前升级任务的唯一总进度清单。凡是讨论“当前同步进度”“后续第几笔 commit”，都以本文的 `1..155` 顺序为准。

## 统计口径

范围：上游 `1.16.5..1.21.8`

统计命令：

```powershell
git -C "E:\Devsoftware\kbc\react-native-keyboard-controller" rev-list --count 1.16.5..1.21.8 -- src android ios cpp
git -C "E:\Devsoftware\kbc\react-native-keyboard-controller" log --reverse --date=short --pretty=format:"%h %ad %s" 1.16.5..1.21.8 -- src android ios cpp
```

统计结果：`155` 笔。

说明：

- `src/android/ios/cpp` 是“库功能相关”的主统计口径。
- 纯 release / package version / example lockfile 不进入 `155` 总数，但可以在详细台账中作为处理记录补充。
- Android-only / iOS-only 仍必须做 Harmony 等价风险审计，不能只按平台目录直接跳过。
- `UPGRADE-COMMIT-ORDER-PLAN.md` 中旧的 `83` 笔清单只是早期关键实施清单，不再作为总进度分母。

## 当前进度

截至第 `45 / 155` 笔按时间顺序完成证据审计；另提前同步第 `91 / 155` 笔 `852fa4a223`：

| 指标 | 数量 |
| --- | ---: |
| 总源码相关 commit | 155 |
| 已按顺序处理到 | 45 |
| 当前进度 | 29.0% |
| 已代码同步并推送 | 20 |
| 已分析-待测试 | 0 |
| 测试工程已同步 | 2 |
| 已分析-不需同步 | 21 |
| 已分析-暂不同步 | 3 |
| 因系统能力缺失不能完全做 | 0 |
| 未分析 | 109 |
| 其中提前同步 | 1 |

当前位置：

- 最后一笔已分析：`45` / `155`，`5ab201112c`
- 下一笔待处理：`46` / `155`，`f963befc1a`
- 已提前同步：`91` / `155`，`852fa4a223`，用于修复动态 `bottomOffset` over-scrolling，不推进主线顺序游标

## 状态说明

| 状态 | 含义 |
| --- | --- |
| 未分析 | 还没有看上游 diff。 |
| 已分析-待测试 | 已确认需要适配，先同步到测试工程验证。 |
| 测试工程已同步 | 已同步到测试工程，等待验证或回写代码仓。 |
| 代码仓已推送 | 已提交并推送到 `origin/ups`。 |
| 已分析-不需同步 | 已做 Harmony 等价风险审计，确认当前不需要代码改动。 |
| 已分析-暂不同步 | 已分析且尝试过，但因 Harmony 当前表现或策略原因暂不进入代码仓。 |
| 因系统能力缺失不能完全做 | 已确认 Harmony 缺公开 API 或系统能力，只能降级/记录。 |

## 155 笔总清单

| 顺序 | Commit | 日期 | 影响区域 | 标题 | 当前状态 | 实际分析结果 / 处理方案 |
| ---: | --- | --- | --- | --- | --- | --- |
| 1 | `2a923a6962` | 2025-02-25 | JS/TS | refactor: use `nativeFabricUIManager` instead of `IS_FABRIC` to detect new architecture in JS (#832) | 代码仓已推送 | 已同步 `src/architecture.ts` 与 `OverKeyboardView` 架构检测引用；Harmony 无 `OverKeyboardView` 原生实现，本提交只保留 JS 架构判断，不补原生空实现。 |
| 2 | `b8a4de91fd` | 2025-02-26 | JS/TS | chore: react-native 0.78 (#812) | 代码仓已推送 | 仅同步库源码有效类型修改：`KeyboardToolbar` 的 `JSX.Element` 改为 `React.JSX.Element`；RN 版本、lockfile、example 维护不进入 Harmony 包。 |
| 3 | `2b21634df9` | 2025-02-27 | JS/TS | fix: set shared values in layout effects (#838) | 代码仓已推送 | 已同步 KAV shared values 初始化：默认值稳定化，并在 `useLayoutEffect` 内读取 Reanimated 值后写入，避免 render 阶段读 shared value 和初始化窗口期状态滞后。 |
| 4 | `1714227edd` | 2025-02-27 | Android | fix: exclude `StatusBar` height from `useWindowDimensions` if `StatusBar` is not translucent (#837) | 已分析-不需同步 | Android 修 `WindowDimensionListener` 的 `content.height + marginTop`。Harmony 键盘高度取 `WindowAvoidArea.TYPE_KEYBOARD`，focused input 位置取 ArkUI window 坐标，未发现叠加 statusbar/marginTop 的同类路径；不改代码，状态栏场景列入回归。 |
| 5 | `8eb6a7be14` | 2025-03-06 | JS/TS | fix: crash when pass `dismiss` as event handler (#845) | 代码仓已推送 | 已同步 `KeyboardController.dismiss` 旧签名容错，`onPress={KeyboardController.dismiss}` 不再因事件对象解构崩溃；与后续 dismiss 初始状态修复合并提交。 |
| 6 | `41d798532d` | 2025-03-07 | Android | fix: selection coordinates in container with static height (#844) | 已分析-不需同步 | Android 修手算 multiline selection 坐标。Harmony selection 走 ArkUI caret offset 读取，不走 Android baseline/gravity/scrollY 手算路径；不改代码，保留多行固定高度输入框回归。 |
| 7 | `efd038cba2` | 2025-03-11 | JS/TS | feat: ability to hide "Done" button in `KeyboardToolbar` (#849) | 代码仓已推送 | 已同步 `doneText=""` 隐藏 Done 按钮能力；测试工程已验证 Toolbar 正常。 |
| 8 | `5b65344e6b` | 2025-03-14 | JS/TS | fix: pending `dismiss` if keyboard wasn't shown in app yet (#857) | 代码仓已推送 | 已同步 `isClosed` 初始值为 true；同时补 Harmony ETS `removeListeners` 不清空事件白名单，避免 dismiss Promise 等不到 `keyboardDidHide`。 |
| 9 | `9080f313be` | 2025-03-17 | JS/TS | fix: allow keyboard toolbar take all available space (#865) | 代码仓已推送 | 已同步 Toolbar 内容区布局：隐藏箭头/Done 后中间内容可占满剩余空间；测试工程已验证。 |
| 10 | `cb8161cf00` | 2025-03-18 | JS/TS | feat: `insets` prop for `KeyboardToolbar` (#866) | 代码仓已推送 | 已同步 `KeyboardToolbar.insets` 左右安全区边距能力；测试工程已验证。 |
| 11 | `03937a761c` | 2025-03-25 | JS/TS+Android+iOS | feat: `OverKeyboardView` with custom `ShadowNode` (#863) | 已分析-不需同步 | 上游新增 Android/iOS/Fabric overlay 原生组件。Harmony 当前无 `RNKCOverKeyboardView` 注册，公开 window API 未发现可把 RN 子树挂到系统键盘窗口层级的能力；不新增空实现，后续若做需专项设计原生组件/子窗口方案。 |
| 12 | `87e47e556f` | 2025-03-28 | iOS | Update correct path of RNKCOverKeyboardViewComponentDescriptor.h in  ... (#886) | 已分析-不需同步 | iOS Fabric header import / podspec `header_dir` 修正；Harmony C++ 使用本地 codegen-harmony 头文件，无 iOS header_dir 问题。 |
| 13 | `d379ff5f21` | 2025-03-29 | JS/TS | fix: `KeyboardAvoidingView` + `autoFocus` + `native-stack` (#880) | 代码仓已推送 | 已同步 KAV 非 `height` 行为在 autoFocus/首帧布局变化后刷新 `initialFrame`；`height` 行为保持键盘打开时不重写初始高度。测试工程 `autoFocus position` 已验证。 |
| 14 | `b27ed8a284` | 2025-04-01 | JS/TS | feat: useKeyboardState (#894) | 代码仓已推送 | 已同步 `useKeyboardState` hook、`KeyboardController.state()` 默认状态与类型；依赖 Harmony 现有 didShow/didHide 事件回流。测试工程已验证。 |
| 15 | `13dfb32c54` | 2025-04-04 | iOS | fix: set delegate to all `UITextView` (#902) | 已分析-不需同步 | iOS UITextView delegate 覆盖范围修复。Harmony 无 UIKit delegate 链路，focused input 事件来自 ArkUI/RNOH 节点事件；不补原生空实现。 |
| 16 | `820fcb4b9d` | 2025-04-07 | JS/TS | feat: `KeyboardStickyView` using plain Animated (#898) | 已分析-暂不同步 | 曾同步测试工程，但导致 Harmony `KeyboardStickyView` 收起时帧延迟/表现异常；已回退，保留 Reanimated 版本和已验证的 Harmony start 阶段修复。 |
| 17 | `0c6ae77362` | 2025-04-09 | JS/TS | fix: `useKeyboardState` tweaks (#907) | 代码仓已推送 | 已与 `b27ed8a284` 合并同步：`KeyboardController.state()` 仅返回事件数据，`useKeyboardState()` 聚合 `isVisible`；测试工程随 hook 验证。 |
| 18 | `c44e7e7e31` | 2025-04-11 | Android | fix: `OverKeyboardView` crash (#913) | 已分析-不需同步 | Android overlay touch dispatch try/catch 防崩。Harmony 当前没有 `OverKeyboardView` 原生 overlay/touch dispatch 链路；不改代码，未来实现 overlay 时纳入验收。 |
| 19 | `033e043774` | 2025-04-15 | iOS | fix: Square payment SDK integration (#900) | 已分析-不需同步 | iOS Square SDK 私有 TextField delegate 递归规避。Harmony 无 Objective-C/Swift delegate 注入链路；不改代码。 |
| 20 | `9011a90a78` | 2025-04-17 | JS/TS | fix: disabling KeyboardAwareScrollView didn't prevent useAnimatedStyle flickering (#921) | 代码仓已推送 | 已同步 KASV `enabled=false` 时不再渲染底部 Reanimated padding view，避免禁用态仍出现 padding/flicker；已验证。 |
| 21 | `0071eb058d` | 2025-04-17 | JS/TS | fix: findNodeHandle on the web platform (#912) | 代码仓已推送 | 已同步 `utils/findNodeHandle` 平台封装；Harmony/native 仍走 RN `findNodeHandle`，行为不变，web 默认入口避免抛错。 |
| 22 | `c8a08ce2b8` | 2025-04-18 | Android | fix: `OverKeyboardView` rotation (fabric) (#918) | 已分析-不需同步 | Android overlay 旋转后 ShadowNode 尺寸同步修复。Harmony 当前无 `OverKeyboardView` 原生 overlay，不新增空实现；未来实现 overlay 时需覆盖旋转/窗口尺寸变化。 |
| 23 | `59dddfee31` | 2025-04-21 | iOS | fix: App Store compatible icons (#926) | 已分析-不需同步 | 只影响 iOS example/App Store 图标资产和拼写配置，不影响 Harmony 库源码/API。 |
| 24 | `661b39e24c` | 2025-05-16 | JS/TS | docs: better JSDoc (#942) | 已分析-不需同步 | 触达 `src` 但主要是 JSDoc/类型文件组织/lint 规则；不改运行时功能，本轮不拆类型结构，后续若重构类型需单独批次。 |
| 25 | `be7ef24f76` | 2025-05-20 | JS/TS | fix: rely on screen dimensions instead of window dimensions (#948) | 已分析-暂不同步 | 这是尺寸 hook 中间态，后续被 `427374fcb7` 修正为原生 `windowDidResize` 事件链路；Harmony 当前缺该原生事件，不能只同步中间实现。 |
| 26 | `ba2e187a6c` | 2025-05-22 | Android | fix: keep shadow nodes in sync (#950) | 已分析-不需同步 | Android native-driver/Fabric shadow tree 同步修复；Harmony 未同步 KSV plain Animated，且无 `onUserDrivenAnimationEnded` 同类事件链路。未来若改 plain Animated 再复核。 |
| 27 | `c5c00778f2` | 2025-05-23 | JS/TS | fix: support dynamic `bottomOffset` for `KeyboardAwareScrollView` (#952) | 代码仓已推送 | 已同步 KASV 动态 `bottomOffset` 重算能力；测试中发现二次滚动叠加，并与提前同步的 `852fa4a223` 一起修正。已验证动态 offset 正常。 |
| 28 | `427374fcb7` | 2025-05-25 | JS/TS+iOS | fix: wrong `useWindowDimensions` on iPad (#957) | 已分析-暂不同步 | 上游统一订阅 `WindowDimensionsEvents.windowDidResize`，iOS 原生 layout 变化发 resize。Harmony 当前未发射 `KeyboardController::windowDidResize`，直接同步 JS 会订阅空事件；需先设计 Harmony resize 事件。 |
| 29 | `e431917472` | 2025-05-26 | JS/TS | fix: `KeyboardAvoidingView` incorrect layout (#954) | 代码仓已推送 | 已同步 KAV `behavior="height"` 防 0/负高度 guard；关闭后不应永久塌陷。测试工程验证无明显问题。 |
| 30 | `76226df4d9` | 2025-06-02 | Android | fix: `OverKeyboardView` crash with `a11y` (#962) | 已分析-不需同步 | 上游问题链路：Android `OverKeyboardHostView` 把业务 children 转挂到 `WindowManager.addView(hostView)` 创建的独立 window，但仍通过 `getChildAt/getChildCount` 假装这些 view 是 host children；TalkBack/accessibility 遍历时会校验 descendant 关系并崩溃。上游处理是覆写 `addChildrenForAccessibility()` 为空、`dispatchPopulateAccessibilityEvent()` 返回 false。Harmony 证据：`harmony/keyboard_controller/src/main/cpp/keyboardControllerPackage.cpp` / `.h` 仅注册 `RNKeyboardControllerView` 和 `RNKeyboardGestureArea`，当前没有 `RNKCOverKeyboardView`、没有独立 overlay host、也没有对应 accessibility children 转发链路。因此当前不改代码；未来若实现 Harmony `OverKeyboardView`，必须把无障碍遍历/hover/touch 与子窗口层级一起验收。 |
| 31 | `49898cd5eb` | 2025-06-03 | iOS | ci: use newer XCode (#964) | 已分析-不需同步 | 上游 diff 只触达 `.github/workflows/*ios*` 与 iOS Xcode baseline plist，未修改 `src/android/ios/cpp` 运行时代码，也没有库 API/原生组件语义变化。Harmony 侧没有 Xcode/baseline 构建链路；不改代码。处理结论不是平台跳过，而是确认该 commit 的实际改动属于 iOS CI 基线维护。 |
| 32 | `98df8d88e2` | 2025-06-03 | iOS | fix: switch between inputs with `KeyboardGestureArea` on iOS (#938) | 已分析-不需同步 | 上游问题链路：iOS `KeyboardGestureArea` 通过 `inputAccessoryView` 扩展键盘区域，`resignFirstResponder` 延迟执行；快速从 A 输入框切到 B 输入框时，先 `becomeFirstResponder` 后延迟 `resignFirstResponder`，最终误关闭新键盘。上游 swizzle `becomeFirstResponder/resignFirstResponder`，保存一帧内的新 focus 请求，切换输入框时立即清理 accessory 而不延迟。Harmony 证据：`KeyboardGestureAreaComponentInstance.cpp` 只处理 ArkUI touch，`swipeToDismiss()` 调 ETS `dismiss`，`swipeToUp()` 调 `show`；props 只有 `showOnSwipeUp`、`enableSwipeToDismiss`、`interpolator` 等，未使用 `textInputNativeID`，不存在 UIKit responder/inputAccessoryView 延迟移除链路。当前不改代码；保留“两个输入框 + KGA offset 快速切换不误 dismiss”的回归项。 |
| 33 | `331293a9cc` | 2025-06-09 | Android | fix: 2 memory leaks (#966) | 已分析-不需同步 | Android 修 `ModalAttachedWatcher` / `WindowDimensionListener` 泄漏；Harmony 无这两个 Android 类，也没有对应 attach watcher / window dimension listener 持有链路。此前按“同类 window keyboard listener / TurboModule cleanup 风险”尝试同步修复，测试发现会导致多类键盘事件异常，说明该改动超出原 commit 问题范围并破坏 Harmony 当前事件注册/分发时序。`0b03f32` 已回退，最终判断 Harmony 当前无同类问题，不修改代码；后续若单独发现 Harmony listener 泄漏，需要基于实际复现另开专项。 |
| 34 | `371cf7baef` | 2025-06-10 | iOS | fix: iOS 26 non-firing `onMove` handler (#971) | 已分析-不需同步 | 上游问题链路：iOS 26 键盘内部 view class prefix 从旧 `UIInputSetContainerView/UIInputSetHostView` 扩展出 `UITrackingWindowView/UIKeyboardItemContainerView`，导致 `KeyboardView.find()` 找不到键盘 host，`CADisplayLink` 读取不到 frame，`onMove` 不触发。Harmony 证据：`RNKeyboardControllerTurboModule.ts` 使用 `window.on('keyboardHeightChange')` / API20+ `keyboardWillShow/Hide` 和 `getWindowAvoidArea(TYPE_KEYBOARD)` 获取高度；`KeyboardControllerViewComponentInstance.cpp` 接收 `keyboardHeightChange` 后合成 `onKeyboardMoveStart/onKeyboardMove/onKeyboardMoveEnd`，不遍历系统键盘 view hierarchy，也没有 UIKit class prefix。当前不改代码；后续只需继续回归 `onMove` 是否随键盘高度变化发射。 |
| 35 | `fb95fc1364` | 2025-06-12 | JS/TS | fix: `KeyboardToolbar` on iOS 26 (#978) | 已分析-不需同步 | 上游 JS 变化是给 Toolbar 增加 `KEYBOARD_HAS_ROUNDED_CORNERS = Platform.OS === "ios" && parseInt(Platform.Version, 10) >= 26`，并仅在该条件下应用 floating 样式、margin、`OPENED_OFFSET=-11`。Harmony 运行时 `Platform.OS` 不会进入 iOS 26 分支；同步后对 Harmony 视觉无收益，还会引入与已验证 Toolbar 无关的 iOS 临时 UI 常量。当前不改代码；若后续为了 JS 源码完全对齐再同步，也应注明 Harmony 行为不变。 |
| 36 | `9de91159df` | 2025-06-13 | iOS | perf: pause `CADisplayLink` instead of re-creation (#980) | 已分析-不需同步 | 上游问题链路：iOS `KeyboardMovementObserver` 每次键盘运动前创建并 add `CADisplayLink`，结束后 invalidate；改为初始化时常驻一个 displayLink，通过 `isPaused` 控制，减少创建开销并在 deinit invalidate。Harmony 证据：当前没有 Swift/CADisplayLink；`KeyboardControllerViewComponentInstance.cpp` 只在 `keyboardHeightChangeHandle()` 后通过 `taskExecutor->runDelayedTask` 合成有限帧，`startKeyboardAnimation()` 先 `cancelKeyboardAnimation()`，它不是常驻帧观察器，也不是每次注册系统 display link 的链路。当前无需按本 commit 改代码；后续只关注合成帧任务是否重复/残留。 |
| 37 | `c8398fc0e3` | 2025-06-16 | Android | refactor: don't inject package context (#983) | 已分析-不需同步 | 上游问题链路：Android `createViewManagers(reactContext)` 把 `ReactApplicationContext` 传入各 ViewManager/Impl，但 Impl 实际只在 `createViewInstance(ThemedReactContext)` 时使用实例 context，构造注入已无用，因此移除。Harmony 证据：`keyboardControllerPackage.cpp/.h` 通过 RNOH `Package::Context` 创建 TurboModule，通过 `ComponentInstance::Context` 创建 C++ component；没有 Java/Kotlin ViewManager 层，也没有把 application context 存进 manager 再长期持有的路径。当前不改代码；生命周期持有风险已在 window listener 审计中单独处理。 |
| 38 | `a57fa4b427` | 2025-06-20 | JS/TS+Android+iOS | feat: `KeyboardBackgroundView` (#981) | 代码仓已推送 | 上游能力：新增视觉组件，用于模拟/匹配系统键盘背景。iOS 用私有 `UIKBBackdropView` 匹配键盘材质；Android 无真实背景 API，采用当前 IME package + 深浅色 + hardcoded skin 表推断颜色。Harmony API 证据：普通应用可用 `inputMethod.getCurrentInputMethod()` 获取当前输入法属性，可用 `TextInput.keyboardAppearance()` 请求键盘样式但“需要输入法适配后生效”；`window.getWindowAvoidArea(TYPE_KEYBOARD)` 只返回固定态软键盘区域；未发现读取系统键盘背景色/材质或复用键盘 backdrop 的公开 API。最终处理：不新增 Harmony native spec，JS 导出 `KeyboardBackgroundView`，底层用 `View` fallback 并按 `colorScheme` 给默认浅/深色背景，业务 `style` 可覆盖。该策略与 Android “模拟键盘背景”的能力级别一致，和 iOS 私有材质/原生背景能力不一致。 |
| 39 | `347fef35c0` | 2025-06-26 | JS/TS+Android+iOS | feat: `KeyboardExtender` (#982) | 代码仓已推送 | 上游能力：iOS 用 `UIInputView(inputViewStyle: keyboard)` 挂到 `UITextField/UITextView.inputAccessoryView`，是真正扩展键盘高度；Android 明确无法原生实现，使用 `KeyboardBackgroundView + KeyboardStickyView + useKeyboardAnimation` polyfill。Harmony API 证据：IME Kit 的 `inputMethodEngine.getInputMethodAbility()` / `createPanel()` / `Panel.show()` 明确面向“输入法应用”，不是普通业务 App；ArkUI `TextInput.customKeyboard()` 会替换系统输入法而不是扩展系统键盘；`WindowStage.createSubWindow*` 只能创建应用子窗口，不会成为输入法窗口的一部分。最终处理：不新增空的 Harmony native `KeyboardExtender`，JS 侧采用与 Android 一致的 polyfill：`KeyboardStickyView + KeyboardBackgroundView + useKeyboardAnimation().progress`，效果是跟随键盘上方显示并模拟背景；和 iOS 系统级 keyboard accessory / 扩展键盘高度语义不一致。测试工程 `KeyboardExtenderExample` 已验证 `enabled`、children 交互和背景容器正常。 |
| 40 | `60ec0ceab8` | 2025-06-30 | JS/TS | feat: keyboard state selector (#998) | 代码仓已推送 | 上游给 `useKeyboardState` 增加 selector 参数，业务可写 `useKeyboardState((state) => state.isVisible)` 只取需要的状态片段，减少无关 re-render。已同步代码仓 `src/hooks/useKeyboardState/index.ts`；测试工程 demo 以黄色 selector 区块和 JSON `selectorIsVisible` 字段验证通过。 |
| 41 | `ecb3595085` | 2025-07-01 | JS/TS+Android | refactor: do not rely on deep imports (#1000) | 代码仓已推送 | 上游移除 JS `monkey-patch`，避免依赖 `react-native/Libraries/Components/StatusBar/NativeStatusBarManagerAndroid` 深导入；Android 侧改由原生 `StatusBarManager` override 处理 edge-to-edge 兼容。Harmony 已有本地 `StatusBarManagerCompat` TurboModule 注册，但没有 Android 原生 module override 链路，本轮只同步 JS 去依赖：删除 `KeyboardProvider` 中 apply/revert monkey patch effect，并删除不再引用的 monkey-patch 文件；保留 Harmony 现有键盘事件和动画分支。 |
| 42 | `46bb921a5d` | 2025-07-02 | iOS | fix: reset `shouldIgnoreKeyboardEvents` to `false` on `resignFirstResponder` (#996) | 已分析-不需同步 | 上游修 iOS `KeyboardGestureArea` / `InvisibleInputAccessoryView` 场景：延迟 `resignFirstResponder` 时如果 native-stack 手势关闭页面，可能没有后续 `keyboardDidAppear` 来清掉 `shouldIgnoreKeyboardEvents`，导致后续键盘事件被过滤。Harmony 搜索未发现 `shouldIgnoreKeyboardEvents`、`KeyboardEventsIgnorer`、`InvisibleInputAccessoryView`、`inputAccessoryView`、`resignFirstResponder`、`KeyboardAreaExtender` 链路；当前 KGA 通过 ArkUI touch 调 ETS `dismiss/show`，不存在 UIKit responder 延迟 detach 状态机。因此不改代码，只保留快速返回/页面切换场景的键盘事件回归。 |
| 43 | `5a2e2a8184` | 2025-07-06 | Android | fix: use reflection for original `StatusBar` implementation discovery (#1008) | 已分析-不需同步 | 上游真实问题是 RN 0.80+ Kotlin `StatusBarModule` 变为 `internal` 后 Android 不能直接发现原始 StatusBar 实现，因此新增 `StatusBarModuleProxy` 反射代理。Harmony 当前 `StatusBarManagerCompat` 是本地 ETS/C++ TurboModule 注册路径，未使用 Android `StatusBarModule`、Kotlin internal class 或反射发现链路；无同类问题，不改代码。 |
| 44 | `27fce1cc93` | 2025-07-08 | JS/TS+Android+iOS | feat: use `keyboardAppearance` instead of global appearance (#1004) | 测试工程已同步 | 上游把 KeyboardToolbar 外观来源从全局 color scheme 改为当前 focused TextInput 的 `keyboardAppearance`，并保证 `KeyboardEventData.appearance` 只返回 `dark/light`。Harmony 证据：RNOH `TextInputTraits` 已有 `keyboardAppearance` prop 和 `Default/Light/Dark` 枚举；测试工程已同步 JS，C++ 在 `postFocusedInputChanged` 推送 `appearance`，ETS 对 `default` 回退系统深浅色，事件最终给 JS `dark/light`。待验证 Toolbar 在不同输入框 `keyboardAppearance` 下切换颜色。 |
| 45 | `5ab201112c` | 2025-07-09 | JS/TS | fix: rewrite `onTextChanged` to `onSelectionChanged` event handler in `KeyboardAwareScrollView` (#546) | 测试工程已同步 | 上游把 KASV 文本变化后的滚动驱动从 `onChangeText` 改为 selection caret y，解决多行输入框只按输入框整体高度判断时无法知道光标坐标的问题。Harmony 证据：当前 C++ 已通过 ArkUI `NODE_TEXT_INPUT/AREA_CARET_OFFSET` 派发 `FocusedInputSelectionChanged` 的 `end.y`；测试工程已同步 JS，加入 `clamp`、`lastSelection` 和 selection debounce 逻辑，同时移除之前 bottomOffset 排查日志。待验证多行输入/选区变化时 KASV 滚动是否跟随光标且不回归动态 bottomOffset。 |
| 46 | `f963befc1a` | 2025-07-12 | iOS | fix: `KeyboardExtender` on iOS 26 (#1015) | 未分析 | |
| 47 | `49979932c7` | 2025-07-13 | JS/TS+Android+iOS | feat: preload (#1016) | 未分析 | |
| 48 | `1c03e7b9cf` | 2025-07-15 | JS/TS | feat: preload by default (#1018) | 未分析 | |
| 49 | `8cca8df8a3` | 2025-07-19 | iOS | feat: update `swiftformat` (#1028) | 未分析 | |
| 50 | `a28dbec565` | 2025-07-20 | JS/TS | feat: `KeyboardAwareScrollView` full screen input support (#1026) | 未分析 | |
| 51 | `e6679bde41` | 2025-07-21 | iOS | fix: delay delegate substitution (#1027) | 未分析 | |
| 52 | `65586f9726` | 2025-07-22 | iOS | fix: `KeyboardBackgroundView` build on fabric (#1034) | 未分析 | |
| 53 | `d6b93dc2d8` | 2025-07-23 | Android | fix: re-setup `WindowDimensionsListener`after JS bundle reload (#1036) | 未分析 | |
| 54 | `221b7210ec` | 2025-07-24 | Android | fix: do not use `BackgroundStyleApplicator` directly (#1039) | 未分析 | |
| 55 | `6971c27ab6` | 2025-07-24 | iOS | fix: `keyboardAppearanceValue` extension iOS < 12 compatibility (#1041) | 未分析 | |
| 56 | `8a2bc0814b` | 2025-07-25 | iOS | fix: conditional rendering of `OverKeyboardView` (#1040) | 未分析 | |
| 57 | `e736db9971` | 2025-07-28 | iOS | refactor: isolate keyboard view locator (#1052) | 未分析 | |
| 58 | `6c641a556e` | 2025-07-28 | JS/TS | fix: remove maybescroll console debug (#1053) | 未分析 | |
| 59 | `6d39874aaf` | 2025-07-30 | JS/TS | fix: mark `KeyboardExtender` `enabled` props as optional in TS (#1058) | 未分析 | |
| 60 | `63d77f8a00` | 2025-08-01 | iOS | fix: iOS crash caused by the `KeyboardControllerView` class implementation (#1059) | 未分析 | |
| 61 | `3ae5e72d8c` | 2025-08-01 | iOS | fix: clang format (#1060) | 未分析 | |
| 62 | `e4e6e6e637` | 2025-08-01 | iOS | feat: keyboard tracking view (#1047) | 未分析 | |
| 63 | `93a55c1e96` | 2025-08-05 | iOS | fix: `KeyboardExtender` auto-resizeable height (#1064) | 未分析 | |
| 64 | `5b0b4c35b1` | 2025-08-05 | iOS | fix: attaching `KeyboardExtender` (#1065) | 未分析 | |
| 65 | `003b3c2688` | 2025-08-07 | iOS | fix: iOS 26 interactive dismissal (#1068) | 未分析 | |
| 66 | `b6e3594860` | 2025-08-07 | iOS | fix: `keyboardLayoutGuide` availability (#1070) | 未分析 | |
| 67 | `af6d19d51d` | 2025-08-07 | iOS | refactor: split `KeyboardMovementObserver` implementation (#1071) | 未分析 | |
| 68 | `360230ba10` | 2025-08-09 | iOS | fix: `KeyboardExtender` on iOS 26 with round corners (#1067) | 未分析 | |
| 69 | `c1d18cc677` | 2025-08-09 | Android | fix: `StatusBar` race conditions (#1074) | 未分析 | |
| 70 | `aff3cbe7d1` | 2025-08-12 | JS/TS | fix: fix disabling KeyboardAvoidingView (#1078) | 未分析 | |
| 71 | `1d7721b9c7` | 2025-08-13 | iOS | fix: avoid direct `FBReactNativeSpec` usage (#1084) | 未分析 | |
| 72 | `2efe655939` | 2025-08-13 | Android | fix: add `target_compile_reactnative_options` (#1085) | 未分析 | |
| 73 | `38217e6671` | 2025-08-27 | Android | fix: clear legacy full screen flag (#1098) | 未分析 | |
| 74 | `1edf41f1cb` | 2025-08-29 | Android | fix: don't use `@RequiresApi` with old SDK versions (#1107) | 未分析 | |
| 75 | `2a3bcee5ec` | 2025-09-08 | JS/TS+iOS | fix: `KeyboardExtender` width on iPad in split mode (#1113) | 未分析 | |
| 76 | `ad76c6ab56` | 2025-09-23 | JS/TS | feat: compound `KeyboardToolbar` (#1125) | 未分析 | |
| 77 | `0a97fd68ba` | 2025-09-25 | Android | fix: double keyboard height (#1131) | 未分析 | |
| 78 | `f2d74b3bc7` | 2025-09-26 | iOS | fix: `KeyboardExtender` initial mount on Fabric (#1135) | 未分析 | |
| 79 | `692494f11d` | 2025-09-30 | JS/TS+Android+iOS | feat: dismiss without animation (#1127) | 未分析 | |
| 80 | `8d3726d387` | 2025-10-01 | Android | chore: react-native 0.80 (#979) | 未分析 | |
| 81 | `89fa05d9a8` | 2025-10-12 | iOS | fix: wrong selection coordinates dispatch on iOS (#1156) | 未分析 | |
| 82 | `4ab7be0c15` | 2025-10-13 | iOS | fix: crash when unrecognized selector sent (#1153) | 未分析 | |
| 83 | `bcfe4eea4d` | 2025-10-14 | iOS | fix: ignore interactive keyboard values that are bigger than keyboard height (#1159) | 未分析 | |
| 84 | `6503e23efd` | 2025-10-14 | iOS | fix: iOS 26 + Modal (#1158) | 未分析 | |
| 85 | `3ab39a0edd` | 2025-10-20 | iOS | fix: attach keyboard tracking view in advance and only when window has been changed (#1170) | 未分析 | |
| 86 | `99be9d3011` | 2025-10-27 | iOS | fix: avoid `ensureLayout` calls (#1174) | 未分析 | |
| 87 | `6e4b14d1e9` | 2025-11-03 | Android | fix: match `StatusBar` and edge-to-edge interop according to latest spec (#1185) | 未分析 | |
| 88 | `0dcf4b4d41` | 2025-11-03 | iOS | fix: non-working release version on iOS 26 (#1184) | 未分析 | |
| 89 | `b0e60f6b32` | 2025-11-04 | iOS | fix: attaching `KeyboardTrackingView` (#1193) | 未分析 | |
| 90 | `1441ae3b59` | 2025-11-06 | Android | fix: add `KeyboardBackgroundView` to turbo package (#1197) | 未分析 | |
| 91 | `852fa4a223` | 2025-11-14 | JS/TS | fix: dynamic `bottomOffset` over-scrolling (#1204) | 代码仓已推送 | 为修复 `c5c00778f2` 测试中复现的二次滚动叠加问题，已提前同步；后续到第 91 笔时只需复核。 |
| 92 | `29ad9e8947` | 2025-11-21 | JS/TS | refactor: don't use private reanimated API (#1209) | 未分析 | |
| 93 | `41858c78f9` | 2025-11-24 | JS/TS | feat: `useAnimatedKeyboard` compat layer (#1220) | 未分析 | |
| 94 | `14ededa4c6` | 2025-11-25 | JS/TS+Android+iOS | feat: sync layout from JS (#1213) | 未分析 | |
| 95 | `4bdf4e5b3e` | 2025-12-01 | JS/TS | fix: types mismatch for `dismiss` method (#1225) | 未分析 | |
| 96 | `70784fd70f` | 2025-12-03 | JS/TS | fix: dependencies array changes size between renders (#1229) | 未分析 | |
| 97 | `59951035af` | 2025-12-09 | iOS | fix: avoid undesired `reloadInputViews` during keyboard dismissal (#1235) | 未分析 | |
| 98 | `acbc7ae418` | 2025-12-12 | Android | fix: wrong `onEnd` event on Samsung devices when keyboard gets shown after interactive dismissal (#1240) | 未分析 | |
| 99 | `afeccd120e` | 2025-12-17 | Android+iOS | fix: shorter codegen name (#1251) | 未分析 | |
| 100 | `e5438be3df` | 2025-12-22 | JS/TS | refactor: move `KeyboardProvider` props in separate file (#1255) | 未分析 | |
| 101 | `b5fb83596c` | 2025-12-23 | JS/TS | refactor: change conditional code in `KeyboardAvoidingView` (#1257) | 未分析 | |
| 102 | `838f546c48` | 2025-12-30 | Android | Call correct onInterceptHoverEvent super in onInterceptHoverEvent() (#1260) | 未分析 | |
| 103 | `91df02ecd4` | 2026-01-07 | iOS | fix: navigation in iOS hybrid app (#1266) | 未分析 | |
| 104 | `68395bf201` | 2026-01-12 | Android+iOS | fix: windows builds (#1248) | 未分析 | |
| 105 | `8041105fea` | 2026-01-15 | iOS | fix: manual did events (#1161) | 未分析 | |
| 106 | `3589930ca5` | 2026-01-23 | Android | fix: do not use `emitDeviceEvent` directly (#1282) | 未分析 | |
| 107 | `a0d49aeb7a` | 2026-01-27 | JS/TS | refactor: add `useCombinedRef` to separate the code logically (#1287) | 未分析 | |
| 108 | `36abe0b8b3` | 2026-01-28 | JS/TS+Android | feat: add `ClippingScrollView` component (#1289) | 未分析 | |
| 109 | `167a000a9c` | 2026-01-29 | JS/TS | feat: add `ScrollViewWithBottomPadding` component (#1294) | 未分析 | |
| 110 | `ebc5205903` | 2026-01-30 | iOS | fix: wrong `onInteractive` event when keyboard closed on iOS 26+ with attached `KeyboardGestureArea` (#1299) | 未分析 | |
| 111 | `423dbef67c` | 2026-02-01 | JS/TS+Android | feat: use `contentInset` for `KeyboardAwareScrollView` (#797) | 未分析 | |
| 112 | `5ef74d9365` | 2026-02-06 | iOS | refactor: update swiftformat (#1305) | 未分析 | |
| 113 | `ac7dee1c27` | 2026-02-09 | JS/TS+iOS | fix: wrong selection coordinates on focus (#1234) | 未分析 | |
| 114 | `6c25251470` | 2026-02-10 | iOS | fix: non-working interactive keyboard dismissal (#1304) | 未分析 | |
| 115 | `76056c322d` | 2026-02-20 | JS/TS | fix: `KeyboardAwareScrollView` loosing ref identity (#1319) | 未分析 | |
| 116 | `d1fb606b0b` | 2026-02-21 | JS/TS | fix: `enabled` prop behavior for `KeyboardStickyView` (fabric arch) (#1312) | 未分析 | |
| 117 | `11c0dbfba8` | 2026-02-24 | JS/TS+Android | feat: introduce `KeyboardChatScrollView` (#1314) | 未分析 | |
| 118 | `a096e393fd` | 2026-03-04 | JS/TS+Android+iOS | fix: noop `scrollRectToVisible` on iOS (#1336) | 未分析 | |
| 119 | `83e34b7069` | 2026-03-05 | JS/TS | feat: `extraContentPadding` prop (#1332) | 未分析 | |
| 120 | `8393ee294f` | 2026-03-05 | Android | fix: memory leak on Android (#1343) | 未分析 | |
| 121 | `d4f1a4cf91` | 2026-03-07 | JS/TS | fix: access before initialization error (#1342) | 未分析 | |
| 122 | `7f32767709` | 2026-03-10 | JS/TS+Android+iOS | feat: `minimumContentPadding` prop on `KeyboardChatScrollView` (#1344) | 未分析 | |
| 123 | `66e62784c3` | 2026-03-11 | JS/TS | fix: add `automaticOffset` prop for correct KAV positioning in modals (#1346) | 未分析 | |
| 124 | `f5dca3cdf9` | 2026-03-11 | JS/TS | fix: `KeyboardAvoidingView` types union (#1354) | 未分析 | |
| 125 | `eb2dfbbe35` | 2026-03-12 | JS/TS+Android+iOS | fix: handle broken measureInWindow (#1355) | 未分析 | |
| 126 | `6872736017` | 2026-03-12 | JS/TS | fix: automatically detect top border of `KeyboardAwareScrollView` (#1352) | 未分析 | |
| 127 | `4a32305f7e` | 2026-03-13 | JS/TS+Android+iOS | feat: `KeyboardToolbar.Group` (#881) | 未分析 | |
| 128 | `0bb348ace6` | 2026-03-14 | JS/TS | refactor: rename `minimumContentPadding` to `blankSpace` (#1359) | 未分析 | |
| 129 | `7a7b6cab6d` | 2026-03-16 | iOS | fix: typo in arch definition (#1367) | 未分析 | |
| 130 | `ecd3bbd8e3` | 2026-03-18 | JS/TS | fix: could not fing view for tag warning (#1379) | 未分析 | |
| 131 | `bba6afc9eb` | 2026-03-19 | JS/TS | perf: don't change `currentKeyboardFrame` each frame (#1381) | 未分析 | |
| 132 | `2f82c434c9` | 2026-03-20 | JS/TS | fix: KeyboardAwareScrollView collapses to zero height inside auto-sizing parents (#1384) | 未分析 | |
| 133 | `90dd77289e` | 2026-03-21 | JS/TS | fix: `KeyboardAwareScrollView` regression after optimization (#1387) | 未分析 | |
| 134 | `9acd790895` | 2026-03-23 | JS/TS | fix: remove unnecessary code (#1389) | 未分析 | |
| 135 | `b28ef3b04d` | 2026-03-23 | JS/TS | Fix extraContentPadding not adjusting scroll when it changes by large amount (#1371) | 未分析 | |
| 136 | `33a5d7b585` | 2026-03-24 | Android | fix: `KeyboardChatScrollView` + `RefreshControl` conflict (#1398) | 未分析 | |
| 137 | `af5da35fb0` | 2026-03-26 | iOS | fix: respect UIDesignRequiresCompatibility in KeyboardExtenderCont... (#1402) | 未分析 | |
| 138 | `ba41d5d1ea` | 2026-03-27 | JS/TS | fix: `KeyboardAwareScrollView` re-focus after hardware keyboard dismissal (#1403) | 未分析 | |
| 139 | `f83e78c04d` | 2026-03-31 | JS/TS+Android+iOS | fix: native rounded keyboard detection (#1415) | 未分析 | |
| 140 | `7b9813396c` | 2026-03-31 | iOS | fix: call `syncUpLayout` on iOS when keyboard changes its mode (emoji vs text) (#1417) | 未分析 | |
| 141 | `75981eeb36` | 2026-04-01 | JS/TS | test: cover keyboard resize with unit test (#1418) | 未分析 | |
| 142 | `fbedbc2308` | 2026-04-03 | JS/TS | feat: add `mode` prop for `KeyboardAwareScrollView` (#1420) | 未分析 | |
| 143 | `51fbe47b09` | 2026-04-06 | Android | fix: `WindowDimensionListener` behavior when Activity restarts (#1422) | 未分析 | |
| 144 | `e979dbf426` | 2026-04-10 | Android | fix: don't modify edge-to-edge mode by default when module toggled on/off (#1412) | 未分析 | |
| 145 | `1d9a9ac21f` | 2026-04-15 | JS/TS | feat: `freeze` in `KeyboardChatScrollView` as `SharedValue` (#1434) | 未分析 | |
| 146 | `b3dc83edd5` | 2026-04-16 | JS/TS | fix: close keyboard even if it's partially visible (#1436) | 未分析 | |
| 147 | `15111a4213` | 2026-04-17 | JS/TS | fix: pass through touches in `KeyboardToolbar` (#1440) | 未分析 | |
| 148 | `c436fbb9e0` | 2026-04-20 | JS/TS+Android | fix: bug with `KeyboardChatScrollView` + `inverted` + `maintainVisibleScrollPosition` + `FlashList` (#1437) | 未分析 | |
| 149 | `1048f5238a` | 2026-04-27 | Android | fix: resolve view crash (#1444) | 未分析 | |
| 150 | `ce8ccaa5f7` | 2026-04-28 | JS/TS | feat: add `onContentInsetChange` callback (#1445) | 未分析 | |
| 151 | `21a939f30d` | 2026-04-29 | JS/TS | feat: migrate `KeyboardStickyView` to reanimated (#1307) | 未分析 | |
| 152 | `5dc56430d7` | 2026-05-04 | JS/TS | feat: expose `onEndVisible` callback from `KeyboardChatScrollView` (#1450) | 未分析 | |
| 153 | `1a6450ef35` | 2026-05-15 | iOS | fix: use weak reference to keyboard view on iOS (#1465) | 未分析 | |
| 154 | `cb011a80ff` | 2026-05-16 | iOS | fix: crash on catalyst (#1464) | 未分析 | |
| 155 | `38076a9df8` | 2026-05-21 | Android | fix: delay `onStart` event (#1461) | 未分析 | |
