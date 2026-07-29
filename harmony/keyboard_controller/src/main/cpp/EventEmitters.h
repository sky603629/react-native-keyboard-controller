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

#ifndef TESTER_HARMONY_KEYBOARD_CONTROLLER_SRC_MAIN_CPP_EVENTEMITTERS_H
#define TESTER_HARMONY_KEYBOARD_CONTROLLER_SRC_MAIN_CPP_EVENTEMITTERS_H

#ifndef EVENTEMITTERS_H
#define EVENTEMITTERS_H

#pragma once
#include <react/renderer/components/view/ViewEventEmitter.h>
#include <react/renderer/core/EventEmitter.h>
#include <jsi/jsi.h>

namespace facebook {
namespace react {
class JSI_EXPORT KeyboardController : public EventEmitter {
public:
    using EventEmitter::EventEmitter;
    struct KeyboardEvent {
        double height;
        float duration;
        double timestamp;
        int target;
    };
    void keyboardWillShow(KeyboardEvent value) const;
    void keyboardDidShow(KeyboardEvent value) const;
    void keyboardWillHide(KeyboardEvent value) const;
    void keyboardDidHide(KeyboardEvent value) const;
    void focusDidSet(KeyboardEvent value) const;
};
class JSI_EXPORT KeyboardControllerViewEventEmitter : public ViewEventEmitter {
public:
    using ViewEventEmitter::ViewEventEmitter;

    struct MoveEvent {
        double height;
        double progress;
        float duration;
        int target;
    };

    struct InputLayoutEvent {
        int parentScrollViewTarget;
        int target;
        struct Layout {
            double absoluteX;
            double absoluteY;
            double x;
            double y;
            double width;
            double height;
        } layout;
    };
    struct TextChangeEvent {
        std::string text;
    };
    struct InputSectionEvent {
        int target;
        struct Selection {
            struct Start {
                double x;
                double y;
                int position;
            } start;
            struct End {
                double x;
                double y;
                int position;
            } end;
        } selection;
    };

    void onKeyboardMoveStart(MoveEvent value) const;
    void onKeyboardMove(MoveEvent value) const;
    void onKeyboardMoveEnd(MoveEvent value) const;
    void onKeyboardMoveInteractive(MoveEvent value) const;
    void onFocusedInputLayoutChanged(InputLayoutEvent value) const;
    void onFocusedInputTextChanged(TextChangeEvent value) const;
    void onFocusedInputSelectionChanged(InputSectionEvent value) const;
};
} // namespace react
} // namespace facebook

#endif

#endif
