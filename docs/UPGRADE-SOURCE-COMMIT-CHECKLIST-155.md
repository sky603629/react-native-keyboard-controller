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

截至第 `29 / 155` 笔按时间顺序分析完成；另提前同步第 `91 / 155` 笔 `852fa4a223`：

| 指标 | 数量 |
| --- | ---: |
| 总源码相关 commit | 155 |
| 已按顺序处理到 | 29 |
| 当前进度 | 18.7% |
| 已代码同步并推送 | 16 |
| 已分析-待测试 | 0 |
| 已分析-不需同步 | 11 |
| 已分析-暂不同步 | 3 |
| 未分析 | 125 |
| 其中提前同步 | 1 |

当前位置：

- 最后一笔已分析：`29` / `155`，`e431917472`
- 下一笔待处理：`30` / `155`，`76226df4d9`
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

| 顺序 | Commit | 日期 | 影响区域 | 标题 | 当前状态 | 备注 |
| ---: | --- | --- | --- | --- | --- | --- |
| 1 | `2a923a6962` | 2025-02-25 | JS/TS | refactor: use `nativeFabricUIManager` instead of `IS_FABRIC` to detect new architecture in JS (#832) | 代码仓已推送 | |
| 2 | `b8a4de91fd` | 2025-02-26 | JS/TS | chore: react-native 0.78 (#812) | 代码仓已推送 | |
| 3 | `2b21634df9` | 2025-02-27 | JS/TS | fix: set shared values in layout effects (#838) | 代码仓已推送 | |
| 4 | `1714227edd` | 2025-02-27 | Android | fix: exclude `StatusBar` height from `useWindowDimensions` if `StatusBar` is not translucent (#837) | 已分析-不需同步 | |
| 5 | `8eb6a7be14` | 2025-03-06 | JS/TS | fix: crash when pass `dismiss` as event handler (#845) | 代码仓已推送 | |
| 6 | `41d798532d` | 2025-03-07 | Android | fix: selection coordinates in container with static height (#844) | 已分析-不需同步 | |
| 7 | `efd038cba2` | 2025-03-11 | JS/TS | feat: ability to hide "Done" button in `KeyboardToolbar` (#849) | 代码仓已推送 | |
| 8 | `5b65344e6b` | 2025-03-14 | JS/TS | fix: pending `dismiss` if keyboard wasn't shown in app yet (#857) | 代码仓已推送 | |
| 9 | `9080f313be` | 2025-03-17 | JS/TS | fix: allow keyboard toolbar take all available space (#865) | 代码仓已推送 | |
| 10 | `cb8161cf00` | 2025-03-18 | JS/TS | feat: `insets` prop for `KeyboardToolbar` (#866) | 代码仓已推送 | |
| 11 | `03937a761c` | 2025-03-25 | JS/TS+Android+iOS | feat: `OverKeyboardView` with custom `ShadowNode` (#863) | 已分析-不需同步 | |
| 12 | `87e47e556f` | 2025-03-28 | iOS | Update correct path of RNKCOverKeyboardViewComponentDescriptor.h in  ... (#886) | 已分析-不需同步 | |
| 13 | `d379ff5f21` | 2025-03-29 | JS/TS | fix: `KeyboardAvoidingView` + `autoFocus` + `native-stack` (#880) | 代码仓已推送 | |
| 14 | `b27ed8a284` | 2025-04-01 | JS/TS | feat: useKeyboardState (#894) | 代码仓已推送 | |
| 15 | `13dfb32c54` | 2025-04-04 | iOS | fix: set delegate to all `UITextView` (#902) | 已分析-不需同步 | |
| 16 | `820fcb4b9d` | 2025-04-07 | JS/TS | feat: `KeyboardStickyView` using plain Animated (#898) | 已分析-暂不同步 | |
| 17 | `0c6ae77362` | 2025-04-09 | JS/TS | fix: `useKeyboardState` tweaks (#907) | 代码仓已推送 | |
| 18 | `c44e7e7e31` | 2025-04-11 | Android | fix: `OverKeyboardView` crash (#913) | 已分析-不需同步 | |
| 19 | `033e043774` | 2025-04-15 | iOS | fix: Square payment SDK integration (#900) | 已分析-不需同步 | |
| 20 | `9011a90a78` | 2025-04-17 | JS/TS | fix: disabling KeyboardAwareScrollView didn't prevent useAnimatedStyle flickering (#921) | 代码仓已推送 | |
| 21 | `0071eb058d` | 2025-04-17 | JS/TS | fix: findNodeHandle on the web platform (#912) | 代码仓已推送 | |
| 22 | `c8a08ce2b8` | 2025-04-18 | Android | fix: `OverKeyboardView` rotation (fabric) (#918) | 已分析-不需同步 | Harmony 当前不支持 OverKeyboardView 原生 overlay，不新增空实现。 |
| 23 | `59dddfee31` | 2025-04-21 | iOS | fix: App Store compatible icons (#926) | 已分析-不需同步 | iOS example/App Store 图标资产，不影响 Harmony 库功能。 |
| 24 | `661b39e24c` | 2025-05-16 | JS/TS | docs: better JSDoc (#942) | 已分析-不需同步 | 仅 JSDoc/类型组织优化；本轮按要求不进行同步。 |
| 25 | `be7ef24f76` | 2025-05-20 | JS/TS | fix: rely on screen dimensions instead of window dimensions (#948) | 已分析-暂不同步 | 与 `427374fcb7` 同属尺寸 hook 链路，不能只同步中间态；Harmony 还缺 `windowDidResize` 原生事件发射。 |
| 26 | `ba2e187a6c` | 2025-05-22 | Android | fix: keep shadow nodes in sync (#950) | 已分析-不需同步 | Android native-driver shadow tree 同步；Harmony 未同步 KSV plain Animated，且无同类 `onUserDrivenAnimationEnded` 链路。 |
| 27 | `c5c00778f2` | 2025-05-23 | JS/TS | fix: support dynamic `bottomOffset` for `KeyboardAwareScrollView` (#952) | 代码仓已推送 | 已验证动态 `bottomOffset` 正常；与提前同步的 `852fa4a223` 一起避免 over-scrolling。 |
| 28 | `427374fcb7` | 2025-05-25 | JS/TS+iOS | fix: wrong `useWindowDimensions` on iPad (#957) | 已分析-暂不同步 | 上游最终改为依赖 `WindowDimensionsEvents.windowDidResize`；Harmony 原生当前未发射该事件。 |
| 29 | `e431917472` | 2025-05-26 | JS/TS | fix: `KeyboardAvoidingView` incorrect layout (#954) | 代码仓已推送 | 已验证无明显问题；`behavior="height"` 时避免返回 `height <= 0`。 |
| 30 | `76226df4d9` | 2025-06-02 | Android | fix: `OverKeyboardView` crash with `a11y` (#962) | 未分析 | |
| 31 | `49898cd5eb` | 2025-06-03 | iOS | ci: use newer XCode (#964) | 未分析 | |
| 32 | `98df8d88e2` | 2025-06-03 | iOS | fix: switch between inputs with `KeyboardGestureArea` on iOS (#938) | 未分析 | |
| 33 | `331293a9cc` | 2025-06-09 | Android | fix: 2 memory leaks (#966) | 代码仓已推送 | 鸿蒙无 Android 同名类，但已按等价生命周期问题修复 window keyboard listener / TurboModule cleanup。 |
| 34 | `371cf7baef` | 2025-06-10 | iOS | fix: iOS 26 non-firing `onMove` handler (#971) | 未分析 | |
| 35 | `fb95fc1364` | 2025-06-12 | JS/TS | fix: `KeyboardToolbar` on iOS 26 (#978) | 未分析 | |
| 36 | `9de91159df` | 2025-06-13 | iOS | perf: pause `CADisplayLink` instead of re-creation (#980) | 未分析 | |
| 37 | `c8398fc0e3` | 2025-06-16 | Android | refactor: don't inject package context (#983) | 未分析 | |
| 38 | `a57fa4b427` | 2025-06-20 | JS/TS+Android+iOS | feat: `KeyboardBackgroundView` (#981) | 未分析 | |
| 39 | `347fef35c0` | 2025-06-26 | JS/TS+Android+iOS | feat: `KeyboardExtender` (#982) | 未分析 | |
| 40 | `60ec0ceab8` | 2025-06-30 | JS/TS | feat: keyboard state selector (#998) | 未分析 | |
| 41 | `ecb3595085` | 2025-07-01 | JS/TS+Android | refactor: do not rely on deep imports (#1000) | 未分析 | |
| 42 | `46bb921a5d` | 2025-07-02 | iOS | fix: reset `shouldIgnoreKeyboardEvents` to `false` on `resignFirstResponder` (#996) | 未分析 | |
| 43 | `5a2e2a8184` | 2025-07-06 | Android | fix: use reflection for original `StatusBar` implementation discovery (#1008) | 未分析 | |
| 44 | `27fce1cc93` | 2025-07-08 | JS/TS+Android+iOS | feat: use `keyboardAppearance` instead of global appearance (#1004) | 未分析 | |
| 45 | `5ab201112c` | 2025-07-09 | JS/TS | fix: rewrite `onTextChanged` to `onSelectionChanged` event handler in `KeyboardAwareScrollView` (#546) | 未分析 | |
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
