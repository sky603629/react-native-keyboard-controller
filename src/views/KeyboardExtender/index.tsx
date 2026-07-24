import React from "react";
import { Animated } from "react-native";

import { KeyboardBackgroundView } from "../../bindings";
import { KeyboardStickyView } from "../../components";
import { useKeyboardAnimation } from "../../hooks";

import type { KeyboardExtenderProps } from "../../types";
import type { PropsWithChildren } from "react";

const AnimatedKeyboardBackgroundView = Animated.createAnimatedComponent(
  KeyboardBackgroundView,
);

const KeyboardExtender = ({
  children,
  enabled = true,
}: PropsWithChildren<KeyboardExtenderProps>) => {
  const { progress } = useKeyboardAnimation();

  return (
    <KeyboardStickyView enabled={enabled}>
      <AnimatedKeyboardBackgroundView style={{ opacity: progress }}>
        {children}
      </AnimatedKeyboardBackgroundView>
    </KeyboardStickyView>
  );
};

export default KeyboardExtender;
