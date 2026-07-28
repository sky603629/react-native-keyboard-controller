import { useSharedValue } from "react-native-reanimated";

import {useKeyboardHandler} from "../../hooks";
import {useKeyboardContext} from "../../context";
import { useLayoutEffect } from "react";
import { Platform } from "react-native";

const OS = Platform.OS;

export const useKeyboardAnimation = () => {
  const { reanimated } = useKeyboardContext();

  const heightWhenOpened = useSharedValue(0);
  const height = useSharedValue(0);
  const progress = useSharedValue(0);
  const isClosed = useSharedValue(true);

  useLayoutEffect(() => {
    const initialHeight = -reanimated.height.value;
    const initialProgress = reanimated.progress.value;

    // eslint-disable-next-line react-compiler/react-compiler
    heightWhenOpened.value = initialHeight;
    height.value = initialHeight;
    progress.value = initialProgress;
    isClosed.value = initialProgress === 0;
  }, []);

  useKeyboardHandler(
    {
      onStart: (e) => {
        "worklet";

        if (e.height > 0) {
          isClosed.value = false;
          heightWhenOpened.value = e.height;
        }
      },
      onMove: (e) => {
        "worklet";
  
        progress.value = e.progress;
        height.value = e.height;
      },
      onInteractive: (e) => {
        "worklet";

        progress.value = e.progress;
        height.value = e.height;
      },
      onEnd: (e) => {
        "worklet";
       if(Platform.OS as string === 'harmony'){
        heightWhenOpened.value = e.height;
       }
  
        isClosed.value = e.height === 0;

        // `height` update happens in `onMove` handler
        // in `onEnd` we need to update only if `onMove`
        // wasn't called (i. e. duration === 0)
        //
        // we can not call this code without condition below
        // because in some corner cases (iOS with `secureTextEntry`)
        // `onEnd` can be emitted before `onMove` and in this case
        // it may lead to choppy/glitchy/jumpy UI
        // see https://github.com/kirillzyusko/react-native-keyboard-controller/issues/327
        if (e.duration === 0) {
          progress.value = e.progress;
          height.value = e.height;
        }
      },
    },
    [],
  );

  return { height, progress, heightWhenOpened, isClosed };
};

export const useTranslateAnimation = () => {
  const { reanimated } = useKeyboardContext();

  const padding = useSharedValue(0);
  const translate = useSharedValue(0);

  useLayoutEffect(() => {
    // eslint-disable-next-line react-compiler/react-compiler
    padding.value = reanimated.progress.value;
  }, []);

  useKeyboardHandler(
    {
      onStart: (e) => {
        "worklet";

        if (e.height === 0) {
          padding.value = 0;
        }
        if (OS === "ios") {
          translate.value = e.progress;
        }
      },
      onMove: (e) => {
        "worklet";

        if (OS !== "ios") {
          translate.value = e.progress;
        }
      },
      onInteractive: (e) => {
        "worklet";

        padding.value = 0;

        translate.value = e.progress;
      },
      onEnd: (e) => {
        "worklet";

        padding.value = e.progress;

        if (OS !== "ios") {
          translate.value = e.progress;
        }
      },
    },
    [],
  );

  return { translate, padding };
};
