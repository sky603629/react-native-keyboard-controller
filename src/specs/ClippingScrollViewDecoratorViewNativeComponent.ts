import codegenNativeComponent from "react-native/Libraries/Utilities/codegenNativeComponent";

import type { HostComponent } from "react-native";
import type { ViewProps } from "react-native/Libraries/Components/View/ViewPropTypes";
import type { Double } from "react-native/Libraries/Types/CodegenTypes";

export interface NativeProps extends ViewProps {
  contentInsetBottom?: Double;
  contentInsetTop?: Double;
}

export default codegenNativeComponent<NativeProps>(
  "ClippingScrollViewDecoratorView",
  {
    excludedPlatforms: ["iOS"],
  },
) as HostComponent<NativeProps>;
