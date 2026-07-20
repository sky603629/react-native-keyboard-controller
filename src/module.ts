import { KeyboardControllerNative, KeyboardEvents } from "./bindings";

import type {
  DismissOptions,
  KeyboardControllerModule,
  KeyboardEventData,
  IKeyboardState,
} from "./types";

let isClosed = false;
let lastState: IKeyboardState = {
  isVisible: false,
  height: 0,
  duration: 0,
  timestamp: new Date().getTime(),
  target: -1,
  type: "default",
  appearance: "light",
};

const getKeyboardStateFromEvent = (event: KeyboardEventData): IKeyboardState => {
  return {
    isVisible: event.height > 0,
    ...event,
  };
};

KeyboardEvents.addListener("keyboardDidHide", (e) => {
  isClosed = true;
  lastState = getKeyboardStateFromEvent(e);
});

KeyboardEvents.addListener("keyboardDidShow", (e) => {
  isClosed = false;
  lastState = getKeyboardStateFromEvent(e);
});

const dismiss = async (
  { keepFocus }: DismissOptions = { keepFocus: false },
): Promise<void> => {
  return new Promise((resolve) => {
    if (isClosed) {
      resolve();

      return;
    }

    const subscription = KeyboardEvents.addListener("keyboardDidHide", () => {
      resolve(undefined);
      subscription.remove();
    });

    KeyboardControllerNative.dismiss(keepFocus);
  });
};
const isVisible = () => !isClosed;
const state = () => lastState;

export const KeyboardController: KeyboardControllerModule = {
  setDefaultMode: KeyboardControllerNative.setDefaultMode,
  setInputMode: KeyboardControllerNative.setInputMode,
  setFocusTo: KeyboardControllerNative.setFocusTo,
  preload: KeyboardControllerNative.preload,
  dismiss,
  isVisible,
  state,
  addListener: KeyboardControllerNative.addListener,
  removeListeners: KeyboardControllerNative.removeListeners,
};
