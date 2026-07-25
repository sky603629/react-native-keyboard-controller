# 附件：ClippingScrollView 鸿蒙试验实现备份

## 文档性质

本文只备份测试工程中验证过的 Harmony 试验实现，便于后续 RNOH 暴露稳定 ScrollView bottom inset / content container padding 能力后复原参考。

这不是主仓已同步实现。当前结论仍是：上游 `108 / 155` `36abe0b8b3` `feat: add ClippingScrollView component (#1289)` 在 Harmony 上不能完整等价实现，原因见 `docs/鸿蒙ClippingScrollView滚动底部空间差异分析.md`。

## 试验工程位置

- 测试工程：`E:\Devsoftware\25p5`
- 测试包源码：`E:\Devsoftware\25p5\node_modules\@react-native-ohos\react-native-keyboard-controller`
- Harmony 构建包副本：`E:\Devsoftware\25p5\harmony\entry\oh_modules\@react-native-ohos\react-native-keyboard-controller`
- RNTester demo：`E:\Devsoftware\25p5\rn-tester\examples\react-native-keyboard-controller\examples\ClippingScrollView\index.tsx`
- demo 备份仓：`E:\Devsoftware\kbc\demo\react-native-keyboard-controller\examples\ClippingScrollView\index.tsx`

## 新增文件清单

测试包中新增过以下文件：

- `harmony/keyboard_controller/src/main/cpp/ClippingScrollViewComponentDescriptor.h`
- `harmony/keyboard_controller/src/main/cpp/ClippingScrollViewComponentInstance.h`
- `harmony/keyboard_controller/src/main/cpp/ClippingScrollViewComponentInstance.cpp`
- `harmony/keyboard_controller/src/main/cpp/ClippingScrollViewJSIBinder.h`
- `src/specs/ClippingScrollViewDecoratorViewNativeComponent.ts`

同时修改过：

- `harmony/keyboard_controller/src/main/cpp/keyboardControllerPackage.cpp`
- `harmony/keyboard_controller/src/main/cpp/keyboardControllerPackage.h`
- `src/bindings.native.ts`
- `src/types.ts`
- RNTester ClippingScrollView demo 入口和示例文件

## Native descriptor 备份

```cpp
inline const char ClippingScrollViewComponentName[] =
    "ClippingScrollViewDecoratorView";

class ClippingScrollViewProps : public ViewProps {
public:
    double contentInsetBottom = 0.0;
    ClippingScrollViewProps() = default;

    ClippingScrollViewProps(
        const PropsParserContext &context,
        const ClippingScrollViewProps &sourceProps,
        const RawProps &rawProps)
        : ViewProps(context, sourceProps, rawProps),
          contentInsetBottom(convertRawProp(
              context,
              rawProps,
              "contentInsetBottom",
              sourceProps.contentInsetBottom,
              {0.0})) {}
};

using ClippingScrollViewShadowNode =
    ConcreteViewShadowNode<
        ClippingScrollViewComponentName,
        ClippingScrollViewProps,
        ViewEventEmitter>;

class ClippingScrollViewComponentDescriptor final
    : public ConcreteComponentDescriptor<ClippingScrollViewShadowNode> {
public:
    ClippingScrollViewComponentDescriptor(
        ComponentDescriptorParameters const &parameters)
        : ConcreteComponentDescriptor(parameters) {}
};
```

## Native instance 备份

头文件结构：

```cpp
class ClippingScrollViewComponentInstance
    : public CppComponentInstance<facebook::react::ClippingScrollViewShadowNode> {
public:
    ClippingScrollViewComponentInstance(Context context);

    void onChildInserted(ComponentInstance::Shared const &childComponentInstance, std::size_t index) override;
    void onChildRemoved(ComponentInstance::Shared const &childComponentInstance) override;
    void onPropsChanged(SharedConcreteProps const &props) override;
    void onLayoutChanged(facebook::react::LayoutMetrics const &layoutMetrics) override;
    ArkUINode &getLocalRootArkUINode() override;

private:
    StackNode m_stackNode;
    double m_contentInsetBottom = 0.0;

    void decorateScrollView();
};
```

核心试验逻辑：

```cpp
static constexpr char DEBUG_TAG[] = "KBC_CLIPPING_SCROLL_DEBUG";

void ClippingScrollViewComponentInstance::decorateScrollView() {
    DLOG(INFO) << DEBUG_TAG << " native.decorate.begin tag=" << getTag()
               << " bottom=" << m_contentInsetBottom
               << " childrenCount=" << getChildren().size();

    for (auto const &child : getChildren()) {
        if (child != nullptr) {
            DLOG(INFO) << DEBUG_TAG << " native.decorate.child wrapperTag=" << getTag()
                       << " childTag=" << child->getTag()
                       << " childName=" << child->getComponentName();
        }

        if (child != nullptr && child->getComponentName() == "ScrollView") {
            auto &scrollNode = child->getLocalRootArkUINode();
            scrollNode.setClip(true);
            scrollNode.setPadding(0, 0, 0, static_cast<float>(m_contentInsetBottom));

            DLOG(INFO) << DEBUG_TAG << " native.decorate.applied wrapperTag=" << getTag()
                       << " scrollTag=" << child->getTag()
                       << " bottom=" << m_contentInsetBottom
                       << " clip=true paddingBottom=" << m_contentInsetBottom;
            return;
        }
    }

    DLOG(INFO) << DEBUG_TAG << " native.decorate.noScrollView tag=" << getTag();
}
```

调用时机：

- `onChildInserted`：插入 child 到 `StackNode` 后调用 `decorateScrollView()`。
- `onChildRemoved`：移除 child 后重新调用。
- `onPropsChanged`：`contentInsetBottom` 变化后重新调用。
- `onLayoutChanged`：布局变化后重新调用。

## JSI binder 备份

```cpp
class ClippingScrollViewJSIBinder : public ViewComponentJSIBinder {
protected:
    facebook::jsi::Object createNativeProps(facebook::jsi::Runtime &rt) override
    {
        auto object = ViewComponentJSIBinder::createNativeProps(rt);
        object.setProperty(rt, "contentInsetBottom", "number");
        return object;
    }
};
```

## Package 注册点备份

`keyboardControllerPackage.cpp`：

```cpp
#include "ClippingScrollViewJSIBinder.h"

std::vector<facebook::react::ComponentDescriptorProvider>
KeyboardControllerPackage::createComponentDescriptorProviders() {
    return {
        facebook::react::concreteComponentDescriptorProvider<
            facebook::react::KeyboardControllerViewComponentDescriptor>(),
        facebook::react::concreteComponentDescriptorProvider<
            facebook::react::KeyboardGestureAreaComponentDescriptor>(),
        facebook::react::concreteComponentDescriptorProvider<
            facebook::react::ClippingScrollViewComponentDescriptor>()};
}

ComponentJSIBinderByString KeyboardControllerPackage::createComponentJSIBinderByName() {
    return {
        {"RNKeyboardControllerView", std::make_shared<KeyboardControllerViewJSIBinder>()},
        {"RNKeyboardGestureArea", std::make_shared<KeyboardGestureAreaJSIBinder>()},
        {"ClippingScrollViewDecoratorView", std::make_shared<ClippingScrollViewJSIBinder>()}};
}

ComponentInstance::Shared KeyboardControllerPackage::createComponentInstance(
    const ComponentInstance::Context &ctx) {
    if (ctx.componentName == "ClippingScrollViewDecoratorView") {
        return std::make_shared<ClippingScrollViewComponentInstance>(ctx);
    }
    return nullptr;
};
```

`keyboardControllerPackage.h`：

```cpp
#include "ClippingScrollViewComponentInstance.h"
```

并在 factory delegate 中追加：

```cpp
if (ctx.componentName == "ClippingScrollViewDecoratorView") {
    return std::make_shared<ClippingScrollViewComponentInstance>(ctx);
}
```

## JS spec / binding 备份

`src/specs/ClippingScrollViewDecoratorViewNativeComponent.ts`：

```ts
import codegenNativeComponent from "react-native/Libraries/Utilities/codegenNativeComponent";

import type { HostComponent } from "react-native";
import type { ViewProps } from "react-native/Libraries/Components/View/ViewPropTypes";
import type { Double } from "react-native/Libraries/Types/CodegenTypes";

export interface NativeProps extends ViewProps {
  contentInsetBottom?: Double;
}

export default codegenNativeComponent<NativeProps>(
  "ClippingScrollViewDecoratorView",
  {
    excludedPlatforms: ["iOS"],
  },
) as HostComponent<NativeProps>;
```

`src/types.ts`：

```ts
export type ClippingScrollViewProps = PropsWithChildren<
  ViewProps & {
    contentInsetBottom?: number;
  }
>;
```

`src/bindings.native.ts`：

```ts
export const ClippingScrollView: React.FC<ClippingScrollViewProps> =
  Platform.OS === "android" || (Platform.OS as string) === "harmony"
    ? require("./specs/ClippingScrollViewDecoratorViewNativeComponent").default
    : ({ children }: ClippingScrollViewProps) => children;
```

## Demo 备份

测试 demo 目标：

- 同屏展示 `Plain ScrollView` 和 `ClippingScrollView`。
- 通过按钮切换 `contentInsetBottom`：`0 / 80 / 160 / 240`。
- 使用同一 tag `KBC_CLIPPING_SCROLL_DEBUG` 打印 native 和 JS 滚动数据。
- 观察 `contentHeight`、`layoutHeight`、`y`、`distanceToEnd`。

关键日志函数：

```ts
const DEBUG_TAG = "KBC_CLIPPING_SCROLL_DEBUG";

function logScroll(
  panel: string,
  phase: string,
  inset: number,
  event: NativeSyntheticEvent<NativeScrollEvent>,
) {
  const { contentOffset, contentSize, layoutMeasurement } = event.nativeEvent;
  const distanceToEnd =
    contentSize.height - layoutMeasurement.height - contentOffset.y;

  console.log(
    DEBUG_TAG,
    "demo.scroll",
    JSON.stringify({
      panel,
      phase,
      inset,
      y: Number(contentOffset.y.toFixed(2)),
      contentHeight: Number(contentSize.height.toFixed(2)),
      layoutHeight: Number(layoutMeasurement.height.toFixed(2)),
      distanceToEnd: Number(distanceToEnd.toFixed(2)),
    }),
  );
}
```

demo 中 `ClippingScrollView` 用法：

```tsx
<ClippingScrollView
  style={styles.decorator}
  contentInsetBottom={inset}
  onLayout={(event) => logLayout(panelKey, "decorator", inset, event)}
>
  {scrollView}
</ClippingScrollView>
```

## 实测结论备份

`clip=false`：

- 能让内容绘制到 ScrollView bounds 外。
- 会覆盖上方 plain panel 区域。
- 不等价于 Android `clipToPadding=false`。
- 不可作为库能力合入。

`clip=true + setPadding(bottom)`：

- native 日志确认 `paddingBottom` 被写入。
- 拖拽阶段底部可多拉出约 `contentInsetBottom` 的视觉空间。
- 但 `contentSize.height` 不增加。
- 松手后 offset 被恢复到 `contentSize.height - layoutHeight`。
- 只能做到拖拽视觉变化，不能做到稳定底部可滚动范围。

最后一个 child 设置 margin：

- 试验后没有形成稳定底部空间。
- 也会侵入业务 child 自身样式。
- 不作为后续方向。

## 后续复原条件

未来满足以下任一条件后，可以重新评估同步：

1. RNOH `ScrollView` 原生支持 `contentInset.bottom`，并让其进入 `contentSize` / scroll range。
2. RNOH 暴露安全 API，允许外部库设置 `ScrollViewComponentInstance` 内部 content container bottom padding。
3. RNOH core 实现等价 Android `clipToPadding=false + paddingBottom` 的可停留 scroll range 语义。

在这些条件满足前，主仓不应导出 Harmony `ClippingScrollViewDecoratorView`，否则上层 `KeyboardAwareScrollView` / `ScrollViewWithBottomPadding` 会误以为底部 inset 已经稳定可用。
