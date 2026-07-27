import React, { forwardRef } from "react";
import { Platform, View } from "react-native";
import Reanimated, {
  useAnimatedProps,
  useAnimatedStyle,
} from "react-native-reanimated";

import { ClippingScrollView } from "../../bindings";

import styles from "./styles";

import type { ScrollViewProps } from "react-native";
import type { SharedValue } from "react-native-reanimated";

const IS_HARMONY = (Platform.OS as string) === "harmony";
const ReanimatedClippingScrollView =
  Platform.OS === "android"
    ? Reanimated.createAnimatedComponent(ClippingScrollView)
    : ClippingScrollView;

type AnimatedScrollViewProps = React.ComponentProps<
  typeof Reanimated.ScrollView
>;

export type AnimatedScrollViewComponent = React.ForwardRefExoticComponent<
  AnimatedScrollViewProps & React.RefAttributes<Reanimated.ScrollView>
>;

type ScrollViewWithBottomPaddingProps = {
  ScrollViewComponent: AnimatedScrollViewComponent;
  children?: React.ReactNode;
  inverted?: boolean;
  bottomPadding: SharedValue<number>;
  contentOffsetY?: SharedValue<number>;
} & ScrollViewProps;

const ScrollViewWithBottomPadding = forwardRef<
  Reanimated.ScrollView,
  ScrollViewWithBottomPaddingProps
>(
  (
    {
      ScrollViewComponent,
      bottomPadding,
      contentInset,
      scrollIndicatorInsets,
      inverted = false,
      contentOffsetY,
      children,
      ...rest
    },
    ref,
  ) => {
    const animatedProps = useAnimatedProps(() => {
      const insetTop = inverted ? bottomPadding.value : 0;
      const insetBottom = inverted ? 0 : bottomPadding.value;
      const result: Record<string, unknown> = {
        contentInset: {
          bottom: insetBottom + (contentInset?.bottom || 0),
          top: insetTop + (contentInset?.top || 0),
          right: contentInset?.right,
          left: contentInset?.left,
        },
        contentInsetBottom: insetBottom,
        contentInsetTop: insetTop,
        scrollIndicatorInsets: {
          bottom: insetBottom + (scrollIndicatorInsets?.bottom || 0),
          top: insetTop + (scrollIndicatorInsets?.top || 0),
          right: scrollIndicatorInsets?.right,
          left: scrollIndicatorInsets?.left,
        },
      };

      if (contentOffsetY) {
        result.contentOffset = { x: 0, y: contentOffsetY.value };
      }

      return result;
    }, [
      inverted,
      contentInset?.bottom,
      contentInset?.top,
      contentInset?.right,
      contentInset?.left,
      scrollIndicatorInsets?.bottom,
      scrollIndicatorInsets?.top,
      scrollIndicatorInsets?.right,
      scrollIndicatorInsets?.left,
      contentOffsetY,
    ]);
    const spacerStyle = useAnimatedStyle(
      () => ({
        height: inverted ? 0 : bottomPadding.value,
      }),
      [inverted],
    );

    if (IS_HARMONY) {
      return (
        <ScrollViewComponent ref={ref} contentInset={contentInset} {...rest}>
          {children}
          <Reanimated.View pointerEvents="none" style={spacerStyle} />
        </ScrollViewComponent>
      );
    }

    return (
      <ReanimatedClippingScrollView
        animatedProps={animatedProps}
        style={styles.container}
      >
        <ScrollViewComponent ref={ref} animatedProps={animatedProps} {...rest}>
          {inverted ? (
            <View collapsable={false} nativeID="container">
              {children}
            </View>
          ) : (
            children
          )}
        </ScrollViewComponent>
      </ReanimatedClippingScrollView>
    );
  },
);

export default ScrollViewWithBottomPadding;
