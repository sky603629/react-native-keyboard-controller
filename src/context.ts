import { createContext, useContext } from "react";
import { Animated } from "react-native";

import type {
  FocusedInputHandlers,
  FocusedInputLayoutChangedEvent,
  KeyboardHandlers,
} from "./types";
import type React from "react";
import type { SharedValue } from "react-native-reanimated";

export type AnimatedContext = {
  progress: Animated.Value;
  height: Animated.AnimatedMultiplication<number>;
};
export type ReanimatedContext = {
  progress: SharedValue<number>;
  height: SharedValue<number>;
};
export type KeyboardAnimationContext = {
  enabled: boolean;
  animated: AnimatedContext;
  reanimated: ReanimatedContext;
  layout: SharedValue<FocusedInputLayoutChangedEvent | null>;
  /** Method for updating info about focused input layout. */
  update: () => Promise<void>;
  setKeyboardHandlers: (handlers: KeyboardHandlers) => void;
  setInputHandlers: (handlers: FocusedInputHandlers) => void;
  setEnabled: React.Dispatch<React.SetStateAction<boolean>>;
  requestSystemKeyboardAvoidanceDisabled: () => () => void;
};
const NOOP = () => {};
const NESTED_NOOP = () => NOOP;
const withSharedValue = <T>(value: T): SharedValue<T> => ({
  value,
  addListener: NOOP,
  removeListener: NOOP,
  modify: NOOP,
  get: () => value,
  set: NOOP,
});
const DEFAULT_SHARED_VALUE = withSharedValue(0);
const DEFAULT_LAYOUT = withSharedValue<FocusedInputLayoutChangedEvent | null>(
  null,
);
const defaultContext: KeyboardAnimationContext = {
  enabled: true,
  animated: {
    progress: new Animated.Value(0),
    height: new Animated.Value(0),
  },
  reanimated: {
    progress: DEFAULT_SHARED_VALUE,
    height: DEFAULT_SHARED_VALUE,
  },
  layout: DEFAULT_LAYOUT,
  update: () => Promise.resolve(),
  setKeyboardHandlers: NESTED_NOOP,
  setInputHandlers: NESTED_NOOP,
  setEnabled: NOOP,
  requestSystemKeyboardAvoidanceDisabled: NESTED_NOOP,
};
export const KeyboardContext = createContext(defaultContext);
export const useKeyboardContext = () => {
  const context = useContext(KeyboardContext);
  if (__DEV__ && context === defaultContext) {
    console.warn(
      "Couldn't find real values for `KeyboardContext`. Please make sure you're inside of `KeyboardProvider` - otherwise functionality of `react-native-keyboard-controller` will not work.",
    );
  }

  return context;
};
