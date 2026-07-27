import type { PropsWithChildren, ReactNode } from "react";
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
export type ClippingScrollViewProps = PropsWithChildren<
  ViewProps & {
    /** An additional space that gets applied to the bottom of the `ScrollView` (inside a scrollable content). Default is `0`. */
    contentInsetBottom?: number;
    /** An additional space that gets applied to the top of the `ScrollView` (inside a scrollable content). Default is `0`. */
    contentInsetTop?: number;
  }
>;
export type KeyboardBackgroundViewProps = PropsWithChildren<ViewProps>;
export type KeyboardToolbarGroupViewProps = PropsWithChildren<ViewProps>;
export type KeyboardExtenderProps = PropsWithChildren<{
  /** Controls whether this `KeyboardExtender` instance should take effect. Default is `true`. */
  enabled?: boolean;
}>;
export type KeyboardProviderProps = {
  children: ReactNode;
  /**
   * Set the value to `true`, if you use translucent status bar on Android.
   * If you already control status bar translucency via `react-native-screens`
   * or `StatusBar` component from `react-native`, you can ignore it.
   * Defaults to `false`.
   *
   * @see https://github.com/kirillzyusko/react-native-keyboard-controller/issues/14
   * @platform android
   */
  statusBarTranslucent?: boolean;
  /**
   * Set the value to `true`, if you use translucent navigation bar on Android.
   * Defaults to `false`.
   *
   * @see https://github.com/kirillzyusko/react-native-keyboard-controller/issues/119
   * @platform android
   */
  navigationBarTranslucent?: boolean;
  /**
   * A boolean property indicating whether to keep edge-to-edge mode always enabled (even when you disable the module).
   * Defaults to `false`.
   *
   * @see https://github.com/kirillzyusko/react-native-keyboard-controller/issues/592
   * @platform android
   */
  preserveEdgeToEdge?: boolean;
  /**
   * A boolean prop indicating whether the module is enabled. It indicate only initial state,
   * i. e. if you try to change this prop after component mount it will not have any effect.
   * To change the property in runtime use `useKeyboardController` hook and `setEnabled` method.
   * Defaults to `true`.
   */
  enabled?: boolean;
  /**
   * A boolean prop indicating whether to preload the keyboard to reduce time-to-interaction (TTI) on first input focus.
   * Defaults to `true`.
   *
   * @platform ios
   */
  preload?: boolean;
};

export type Direction = "next" | "prev" | "current";
export type DismissOptions = {
  keepFocus: boolean;
  animated: boolean;
};
export type KeyboardControllerModule = {
  // android only
  setDefaultMode: () => void;
  setInputMode: (mode: number) => void;
  // ios only
  preload: () => void;
  // all platforms
  /**
   * Dismisses the active keyboard. Removes focus by default, but allows passing
   * `{ keepFocus: true }` to keep focus. Pass `{ animated: false }` to request
   * immediate keyboard dismissal when the platform supports it.
   */
  dismiss: (options?: Partial<DismissOptions>) => Promise<void>;
  setFocusTo: (direction: Direction) => void;
  isVisible: () => boolean;
  state: () => KeyboardEventData;
  addListener: (eventName: string) => void;
  removeListeners: (count: number) => void;
};
export type KeyboardControllerNativeModule = {
  // android only
  setDefaultMode: () => void;
  setInputMode: (mode: number) => void;
  // ios only
  preload: () => void;
  // all platforms
  dismiss: (keepFocus: boolean, animated: boolean) => void;
  setFocusTo: (direction: Direction) => void;
  // native event module stuff
  addListener: (eventName: string) => void;
  removeListeners: (count: number) => void;
};

// Event module declarations
export type KeyboardControllerEvents =
  | "keyboardWillShow"
  | "keyboardDidShow"
  | "keyboardWillHide"
  | "keyboardDidHide";
export type KeyboardEventData = {
  height: number;
  duration: number;
  timestamp: number;
  target: number;
  type: NonNullable<TextInputProps["keyboardType"]>;
  appearance: NonNullable<TextInputProps["keyboardAppearance"]>;
};
export type IKeyboardState = {
  isVisible: boolean;
} & KeyboardEventData;
export type KeyboardEventsModule = {
  addListener: (
    name: KeyboardControllerEvents,
    cb: (e: KeyboardEventData) => void,
  ) => EmitterSubscription;
};
export type FocusedInputAvailableEvents = "focusDidSet";
export type FocusedInputEventData = {
  current: number;
  count: number;
};
export type FocusedInputEventsModule = {
  addListener: (
    name: FocusedInputAvailableEvents,
    cb: (e: FocusedInputEventData) => void,
  ) => EmitterSubscription;
};
export type WindowDimensionsAvailableEvents = "windowDidResize";
export type WindowDimensionsEventData = {
  width: number;
  height: number;
};
export type WindowDimensionsEventsModule = {
  addListener: (
    name: WindowDimensionsAvailableEvents,
    cb: (e: WindowDimensionsEventData) => void,
  ) => EmitterSubscription;
};

// reanimated hook declaration
export type KeyboardHandlerHook<TContext, Event> = (
  handlers: {
    onKeyboardMoveStart?: (e: NativeEvent, context: TContext) => void;
    onKeyboardMove?: (e: NativeEvent, context: TContext) => void;
    onKeyboardMoveEnd?: (e: NativeEvent, context: TContext) => void;
    onKeyboardMoveInteractive?: (e: NativeEvent, context: TContext) => void;
  },
  dependencies?: unknown[],
) => (e: NativeSyntheticEvent<Event>) => void;
export type FocusedInputLayoutHandlerHook<TContext, Event> = (
  handlers: {
    onFocusedInputLayoutChanged?: (
      e: FocusedInputLayoutChangedEvent,
      context: TContext,
    ) => void;
  },
  dependencies?: unknown[],
) => (e: NativeSyntheticEvent<Event>) => void;
export type FocusedInputTextHandlerHook<TContext, Event> = (
  handlers: {
    onFocusedInputTextChanged?: (
      e: FocusedInputTextChangedEvent,
      context: TContext,
    ) => void;
  },
  dependencies?: unknown[],
) => (e: NativeSyntheticEvent<Event>) => void;
export type FocusedInputSelectionHandlerHook<TContext, Event> = (
  handlers: {
    onFocusedInputSelectionChanged?: (
      e: FocusedInputSelectionChangedEvent,
      context: TContext,
    ) => void;
  },
  dependencies?: unknown[],
) => (e: NativeSyntheticEvent<Event>) => void;

// package types
export type Handlers<T> = Record<string, T | undefined>;
export type KeyboardHandler = Partial<{
  onStart: (e: NativeEvent) => void;
  onMove: (e: NativeEvent) => void;
  onEnd: (e: NativeEvent) => void;
  onInteractive: (e: NativeEvent) => void;
}>;
export type KeyboardHandlers = Handlers<KeyboardHandler>;
export type FocusedInputHandler = Partial<{
  onChangeText: (e: FocusedInputTextChangedEvent) => void;
  onSelectionChange: (e: FocusedInputSelectionChangedEvent) => void;
}>;
export type FocusedInputHandlers = Handlers<FocusedInputHandler>;
