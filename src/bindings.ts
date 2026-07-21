import { View } from "react-native";

import type {
  FocusedInputEventsModule,
  KeyboardControllerNativeModule,
  KeyboardControllerProps,
  KeyboardEventsModule,
  KeyboardGestureAreaProps,
  OverKeyboardViewProps,
  KeyboardToolbarGroupViewProps,
  WindowDimensionsEventsModule,
} from "./types";
import type { EmitterSubscription } from "react-native";

const NOOP = () => {};

export const KeyboardControllerNative: KeyboardControllerNativeModule = {
  setDefaultMode: NOOP,
  setInputMode: NOOP,
  preload: NOOP,
  dismiss: NOOP,
  setFocusTo: NOOP,
  addListener: NOOP,
  removeListeners: NOOP,
};
export const KeyboardEvents: KeyboardEventsModule = {
  addListener: () => ({ remove: NOOP } as EmitterSubscription),
};
/**
 * This API is not documented, it's for internal usage only (for now), and is a subject to potential breaking changes in future.
 * Use it with cautious.
 */
export const FocusedInputEvents: FocusedInputEventsModule = {
  addListener: () => ({ remove: NOOP } as EmitterSubscription),
};
export const WindowDimensionsEvents: WindowDimensionsEventsModule = {
  addListener: () => ({ remove: NOOP } as EmitterSubscription),
};
export const KeyboardControllerView =
  View as unknown as React.FC<KeyboardControllerProps>;
export const KeyboardControllerViewCommands = {
  synchronizeFocusedInputLayout: (
    _ref: React.Component<KeyboardControllerProps> | null,
  ) => {},
};
export const KeyboardGestureArea =
  View as unknown as React.FC<KeyboardGestureAreaProps>;
export const RCTOverKeyboardView =
  View as unknown as React.FC<OverKeyboardViewProps>;

/**
 * Marker view for KeyboardToolbar.Group — scopes Prev/Next focus traversal.
 */
export const RCTKeyboardToolbarGroupView =
  View as unknown as React.FC<KeyboardToolbarGroupViewProps>;
