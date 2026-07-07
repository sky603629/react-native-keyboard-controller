/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 */
/**
 * MIT License
 *
 * Copyright (C) 2024 Huawei Device Co., Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <glog/logging.h>
#include "EventEmitters.h"

namespace facebook {
namespace react {

void KeyboardControllerViewEventEmitter::onKeyboardMoveStart(MoveEvent event) const {
    dispatchEvent("KeyboardMoveStart", [event = std::move(event)](jsi::Runtime &runtime) {
        auto payload = jsi::Object(runtime);
        payload.setProperty(runtime, "height", event.height);
        payload.setProperty(runtime, "target", event.target);
        payload.setProperty(runtime, "duration", event.duration);
        payload.setProperty(runtime, "progress", event.progress);
        return payload;
    });
}

void KeyboardControllerViewEventEmitter::onKeyboardMove(MoveEvent event) const {
    dispatchEvent("KeyboardMove", [event = std::move(event)](jsi::Runtime &runtime) {
        auto payload = jsi::Object(runtime);
        payload.setProperty(runtime, "height", event.height);
        payload.setProperty(runtime, "target", event.target);
        payload.setProperty(runtime, "duration", event.duration);
        payload.setProperty(runtime, "progress", event.progress);
        return payload;
    });
}
void KeyboardControllerViewEventEmitter::onKeyboardMoveEnd(MoveEvent event) const {
    dispatchEvent("KeyboardMoveEnd", [event = std::move(event)](jsi::Runtime &runtime) {
        auto payload = jsi::Object(runtime);
        payload.setProperty(runtime, "height", event.height);
        payload.setProperty(runtime, "target", event.target);
        payload.setProperty(runtime, "duration", event.duration);
        payload.setProperty(runtime, "progress", event.progress);
        return payload;
    });
}
void KeyboardControllerViewEventEmitter::onKeyboardMoveInteractive(MoveEvent event) const {
    dispatchEvent("KeyboardMoveInteractive", [event = std::move(event)](jsi::Runtime &runtime) {
        auto payload = jsi::Object(runtime);
        payload.setProperty(runtime, "height", event.height);
        payload.setProperty(runtime, "target", event.target);
        payload.setProperty(runtime, "duration", event.duration);
        payload.setProperty(runtime, "progress", event.progress);
        return payload;
    });
}
void KeyboardControllerViewEventEmitter::onFocusedInputLayoutChanged(InputLayoutEvent event) const {
    dispatchEvent("FocusedInputLayoutChanged", [event = std::move(event)](jsi::Runtime &runtime) {
        auto payload = jsi::Object(runtime);
        auto layout = jsi::Object(runtime);
        payload.setProperty(runtime, "target", event.target);
        payload.setProperty(runtime, "parentScrollViewTarget", event.parentScrollViewTarget);
        layout.setProperty(runtime, "x", event.layout.x);
        layout.setProperty(runtime, "y", event.layout.y);
        layout.setProperty(runtime, "width", event.layout.width);
        layout.setProperty(runtime, "height", event.layout.height);
        layout.setProperty(runtime, "absoluteX", event.layout.absoluteX);
        layout.setProperty(runtime, "absoluteY", event.layout.absoluteY);
        payload.setProperty(runtime, "layout", layout);
        return payload;
    });
}
void KeyboardControllerViewEventEmitter::onFocusedInputTextChanged(TextChangeEvent event) const {
    dispatchEvent("FocusedInputTextChanged", [event = std::move(event)](jsi::Runtime &runtime) {
        auto payload = jsi::Object(runtime);
        payload.setProperty(runtime, "text", event.text);
        return payload;
    });
}
void KeyboardControllerViewEventEmitter::onFocusedInputSelectionChanged(InputSectionEvent event) const {
    dispatchEvent("FocusedInputSelectionChanged", [event = std::move(event)](jsi::Runtime &runtime) {
        auto payload = jsi::Object(runtime);
        auto selection = jsi::Object(runtime);
        auto start = jsi::Object(runtime);
        auto end = jsi::Object(runtime);
        payload.setProperty(runtime, "target", event.target);
        start.setProperty(runtime, "x", event.selection.start.x);
        start.setProperty(runtime, "y", event.selection.start.y);
        start.setProperty(runtime, "position", event.selection.start.position);
        end.setProperty(runtime, "x", event.selection.end.x);
        end.setProperty(runtime, "y", event.selection.end.y);
        end.setProperty(runtime, "position", event.selection.end.position);
        selection.setProperty(runtime, "start", start);
        selection.setProperty(runtime, "end", end);
        payload.setProperty(runtime, "selection", selection);
        return payload;
    });
}

} // namespace react
} // namespace facebook
