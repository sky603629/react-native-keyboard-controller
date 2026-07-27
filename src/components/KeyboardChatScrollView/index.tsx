import React, { forwardRef, useCallback, useEffect, useMemo } from "react";
import { Platform, StyleSheet } from "react-native";
import { useAnimatedRef, useAnimatedStyle } from "react-native-reanimated";
import Reanimated from "react-native-reanimated";

import useCombinedRef from "../hooks/useCombinedRef";
import ScrollViewWithBottomPadding from "../ScrollViewWithBottomPadding";

import { useChatKeyboard } from "./useChatKeyboard";

import type { KeyboardChatScrollViewProps } from "./types";
import type { LayoutChangeEvent } from "react-native";

const IS_HARMONY = (Platform.OS as string) === "harmony";

const KeyboardChatScrollView = forwardRef<
  Reanimated.ScrollView,
  React.PropsWithChildren<KeyboardChatScrollViewProps>
>(
  (
    {
      children,
      ScrollViewComponent = Reanimated.ScrollView,
      inverted = false,
      keyboardLiftBehavior = "always",
      freeze = false,
      offset = 0,
      onLayout: onLayoutProp,
      onContentSizeChange: onContentSizeChangeProp,
      ...rest
    },
    ref,
  ) => {
    const scrollViewRef = useAnimatedRef<Reanimated.ScrollView>();
    const onRef = useCombinedRef(ref, scrollViewRef);
    const effectiveInverted = IS_HARMONY ? false : inverted;

    useEffect(() => {
      if (IS_HARMONY && __DEV__ && inverted) {
        console.warn(
          "[KeyboardChatScrollView] inverted is not supported by the Harmony spacer implementation and has been disabled.",
        );
      }
    }, [inverted]);

    const {
      padding,
      currentHeight,
      contentOffsetY,
      onLayout: onLayoutInternal,
      onContentSizeChange: onContentSizeChangeInternal,
    } = useChatKeyboard(scrollViewRef, {
      inverted: effectiveInverted,
      keyboardLiftBehavior,
      freeze,
      offset,
    });

    const onLayout = useCallback(
      (e: LayoutChangeEvent) => {
        onLayoutInternal(e);
        onLayoutProp?.(e);
      },
      [onLayoutInternal, onLayoutProp],
    );

    const onContentSizeChange = useCallback(
      (w: number, h: number) => {
        const contentHeight = IS_HARMONY ? Math.max(h - padding.value, 0) : h;

        onContentSizeChangeInternal(w, contentHeight);
        onContentSizeChangeProp?.(w, contentHeight);
      },
      [onContentSizeChangeInternal, onContentSizeChangeProp, padding],
    );

    // Invisible view whose animated style changes every frame during keyboard
    // animation. On Fabric, this forces Reanimated to schedule a commit,
    // which flushes the scrollTo call in the same frame (fixing de-synchronization).
    // see https://github.com/software-mansion/react-native-reanimated/issues/9000
    const commitStyle = useAnimatedStyle(
      () => ({
        transform: [{ translateY: -currentHeight.value }],
      }),
      [],
    );
    const commit = useMemo(
      () => [styles.commitView, commitStyle],
      [commitStyle],
    );

    return (
      <>
        <ScrollViewWithBottomPadding
          ref={onRef}
          {...rest}
          bottomPadding={padding}
          contentOffsetY={contentOffsetY}
          inverted={effectiveInverted}
          ScrollViewComponent={ScrollViewComponent}
          onContentSizeChange={onContentSizeChange}
          onLayout={onLayout}
        >
          {children}
        </ScrollViewWithBottomPadding>
        <Reanimated.View style={commit} />
      </>
    );
  },
);

const styles = StyleSheet.create({
  commitView: {
    display: "none",
    position: "absolute",
  },
});

export default KeyboardChatScrollView;
