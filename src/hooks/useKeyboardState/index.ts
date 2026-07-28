import { useEffect, useState } from "react";

import { KeyboardEvents } from "../../bindings";
import { KeyboardController } from "../../module";

import type { IKeyboardState } from "../../types";

const EVENTS = ["keyboardWillShow", "keyboardDidHide"] as const;

const getLatestState = () => ({
  ...KeyboardController.state(),
  isVisible: KeyboardController.isVisible(),
});

type KeyboardStateSelector<T> = (state: IKeyboardState) => T;

const defaultSelector: KeyboardStateSelector<IKeyboardState> = (state) => state;

function useKeyboardState<T = IKeyboardState>(
  selector: KeyboardStateSelector<T> = defaultSelector as KeyboardStateSelector<T>,
): T {
  const [state, setState] = useState<T>(() => selector(getLatestState()));

  useEffect(() => {
    const subscriptions = EVENTS.map((event) =>
      KeyboardEvents.addListener(event, () =>
        // state will be updated by global listener first,
        // so we simply read it and don't derive data from the event
        setState(selector(getLatestState())),
      ),
    );

    // we might have missed an update between reading a value in render and
    // `addListener` in this handler, so we set it here. If there was
    // no change, React will filter out this update as a no-op.
    setState(selector(getLatestState()));

    return () => {
      subscriptions.forEach((subscription) => subscription.remove());
    };
  }, []);

  return state;
}

export { useKeyboardState };
