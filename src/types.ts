import type { PropsWithChildren } from "react";
import type {
  EmitterSubscription,
  NativeSyntheticEvent,
  TextInputProps,
  ViewProps,
} from "react-native";

// DirectEventHandler events declaration
export type NativeEvent = {
  progress: number;
  height: number;
  duration: number;
  target: number;
};
export type FocusedInputLayoutChangedEvent = {
  target: number;
  parentScrollViewTarget: number;
  layout: {
    x: number;
    y: number;
    width: number;
    height: number;
    absoluteX: number;
    absoluteY: number;
  };
};
export type FocusedInputTextChangedEvent = {
  text: string;
};
export type FocusedInputSelectionChangedEvent = {
  target: number;
  selection: {
    start: {
      x: number;
      y: number;
      position: number;
    };
    end: {
      x: number;
      y: number;
      position: number;
    };
  };
};
export type EventWithName<T> = {
  eventName: string;
} & T;

// native View/Module declarations
export type KeyboardControllerProps = {
  // callback props
  onKeyboardMoveStart?: (
    e: NativeSyntheticEvent<EventWithName<NativeEvent>>,
  ) => void;
  onKeyboardMove?: (
    e: NativeSyntheticEvent<EventWithName<NativeEvent>>,
  ) => void;
  onKeyboardMoveEnd?: (
    e: NativeSyntheticEvent<EventWithName<NativeEvent>>,
  ) => void;
  onKeyboardMoveInteractive?: (
    e: NativeSyntheticEvent<EventWithName<NativeEvent>>,
  ) => void;
  onFocusedInputLayoutChanged?: (
    e: NativeSyntheticEvent<EventWithName<FocusedInputLayoutChangedEvent>>,
  ) => void;
  onFocusedInputTextChanged?: (
    e: NativeSyntheticEvent<EventWithName<FocusedInputTextChangedEvent>>,
  ) => void;
  onFocusedInputSelectionChanged?: (
    e: NativeSyntheticEvent<EventWithName<FocusedInputSelectionChangedEvent>>,
  ) => void;
  // fake props used to activate reanimated bindings
  onKeyboardMoveReanimated?: (
    e: NativeSyntheticEvent<EventWithName<NativeEvent>>,
  ) => void;
  onFocusedInputLayoutChangedReanimated?: (
    e: NativeSyntheticEvent<EventWithName<FocusedInputLayoutChangedEvent>>,
  ) => void;
  onFocusedInputTextChangedReanimated?: (
    e: NativeSyntheticEvent<EventWithName<FocusedInputTextChangedEvent>>,
  ) => void;
  onFocusedInputSelectionChangedReanimated?: (
    e: NativeSyntheticEvent<EventWithName<FocusedInputSelectionChangedEvent>>,
  ) => void;
  // props
  statusBarTranslucent?: boolean;
  navigationBarTranslucent?: boolean;
  preserveEdgeToEdge?: boolean;
  disableSystemKeyboardAvoidance?: boolean;
  enabled?: boolean;
} & ViewProps;

export type KeyboardGestureAreaProps = {
  interpolator?: "ios" | "linear";
  /**
   * Whether to allow to show a keyboard from dismissed state by swipe up.
   * Default to `false`.
   */
  showOnSwipeUp?: boolean;
  /**
   * Whether to allow to control a keyboard by gestures. The strategy how
   * it should be controlled is determined by `interpolator` property.
   * Defaults to `true`.
   */
  enableSwipeToDismiss?: boolean;
  /**
   * Extra distance to the keyboard.
   */
  offset?: number;
  /**
   * A corresponding `nativeID` value from the corresponding `TextInput`.
   */
  textInputNativeID?: string;
} & ViewProps;

export type OverKeyboardViewProps = PropsWithChildren<{
  visible: boolean;
}>;
/** Marker view props for KeyboardToolbar.Group (no custom props). */
export type KeyboardToolbarGroupViewProps = PropsWithChildren<ViewProps>;
export type KeyboardHandlers = Handlers<KeyboardHandler>;
export type FocusedInputHandler = Partial<{
  onChangeText: (e: FocusedInputTextChangedEvent) => void;
  onSelectionChange: (e: FocusedInputSelectionChangedEvent) => void;
}>;
export type FocusedInputHandlers = Handlers<FocusedInputHandler>;
