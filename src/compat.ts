import { useSharedValue } from "react-native-reanimated";

import { useKeyboardHandler } from "./hooks";

export const KeyboardState = {
  UNKNOWN: 0,
  OPENING: 1,
  OPEN: 2,
  CLOSING: 3,
  CLOSED: 4,
};

export const useAnimatedKeyboard = () => {
  const height = useSharedValue(0);
  const state = useSharedValue(KeyboardState.UNKNOWN);

  useKeyboardHandler(
    {
      onStart: (e) => {
        "worklet";

        state.value =
          e.height > 0 ? KeyboardState.OPENING : KeyboardState.CLOSING;
      },
      onMove: (e) => {
        "worklet";

        height.value = e.height;
      },
      onInteractive: (e) => {
        "worklet";

        height.value = e.height;
      },
      onEnd: (e) => {
        "worklet";

        state.value = e.height > 0 ? KeyboardState.OPEN : KeyboardState.CLOSED;
        height.value = e.height;
      },
    },
    [],
  );

  return { height, state };
};
