import { useCallback } from "react";
import { useScrollViewOffset, useSharedValue } from "react-native-reanimated";

import type { LayoutChangeEvent } from "react-native";
import type { AnimatedRef } from "react-native-reanimated";
import type Reanimated from "react-native-reanimated";

const useScrollState = (ref: AnimatedRef<Reanimated.ScrollView>) => {
  const offset = useScrollViewOffset(ref);
  const layout = useSharedValue({ width: 0, height: 0 });
  const size = useSharedValue({ width: 0, height: 0 });

  // RNOH 0.82 does not expose the newer event registration helper used by
  // upstream. Track offset through Reanimated and dimensions through props.
  const onLayout = useCallback(
    (e: LayoutChangeEvent) => {
      layout.value = {
        width: e.nativeEvent.layout.width,
        height: e.nativeEvent.layout.height,
      };
    },
    [layout],
  );

  const onContentSizeChange = useCallback(
    (w: number, h: number) => {
      size.value = { width: w, height: h };
    },
    [size],
  );

  return { offset, layout, size, onLayout, onContentSizeChange };
};

export default useScrollState;
