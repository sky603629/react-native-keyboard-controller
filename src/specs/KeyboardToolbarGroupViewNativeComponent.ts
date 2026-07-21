import codegenNativeComponent from "react-native/Libraries/Utilities/codegenNativeComponent";

import type { HostComponent } from "react-native";
import type { ViewProps } from "react-native/Libraries/Components/View/ViewPropTypes";

export interface NativeProps extends ViewProps {}

/**
 * Marker view that scopes KeyboardToolbar Prev/Next focus traversal
 * to inputs inside the group (upstream KeyboardToolbar.Group).
 */
export default codegenNativeComponent<NativeProps>(
  "KeyboardToolbarGroupView",
) as HostComponent<NativeProps>;
