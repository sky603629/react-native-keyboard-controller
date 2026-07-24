import React from "react";
import { useColorScheme, View } from "react-native";

import type {
  FocusedInputEventsModule,
  KeyboardBackgroundViewProps,
  KeyboardControllerNativeModule,
  KeyboardControllerProps,
  KeyboardEventsModule,
  KeyboardExtenderProps,
  KeyboardGestureAreaProps,
  OverKeyboardViewProps,
  WindowDimensionsEventsModule,
} from "./types";
import type { EmitterSubscription } from "react-native";
import type { View as RNView } from "react-native";

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
export const KeyboardGestureArea =
  View as unknown as React.FC<KeyboardGestureAreaProps>;
export const RCTOverKeyboardView =
  View as unknown as React.FC<OverKeyboardViewProps>;
export const KeyboardBackgroundView = React.forwardRef<
  RNView,
  KeyboardBackgroundViewProps
>(({ style, ...props }, ref) => {
  const colorScheme = useColorScheme();
  const backgroundColor = colorScheme === "dark" ? "#2c2c2e" : "#eceff3";

  return React.createElement(View, {
    ref,
    style: [{ backgroundColor }, style],
    ...props,
  });
});
export const RCTKeyboardExtender =
  View as unknown as React.FC<KeyboardExtenderProps>;
