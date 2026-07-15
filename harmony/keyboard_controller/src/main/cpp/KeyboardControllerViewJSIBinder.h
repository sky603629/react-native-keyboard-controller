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

#ifndef TESTER_HARMONY_KEYBOARD_CONTROLLER_SRC_MAIN_CPP_KEYBOARDCONTROLLERVIEWJSIBINDER_H
#define TESTER_HARMONY_KEYBOARD_CONTROLLER_SRC_MAIN_CPP_KEYBOARDCONTROLLERVIEWJSIBINDER_H

#pragma once
#include "RNOHCorePackage/ComponentBinders/ViewComponentJSIBinder.h"
namespace rnoh {
class KeyboardControllerViewJSIBinder : public ViewComponentJSIBinder {
protected:
    facebook::jsi::Object createNativeProps(facebook::jsi::Runtime &rt) override {
        auto object = ViewComponentJSIBinder::createNativeProps(rt);
        object.setProperty(rt, "enabled", "bool");
        object.setProperty(rt, "statusBarTranslucent", "bool");
        object.setProperty(rt, "navigationBarTranslucent", "bool");
        object.setProperty(rt, "preserveEdgeToEdge", "bool");
        object.setProperty(rt, "disableSystemKeyboardAvoidance", "bool");
        return object;
    }

    facebook::jsi::Object createBubblingEventTypes(facebook::jsi::Runtime &rt) override {
        facebook::jsi::Object events(rt);
        return events;
    }

    facebook::jsi::Object createDirectEventTypes(facebook::jsi::Runtime &rt) override {
        facebook::jsi::Object events(rt);
        events.setProperty(rt, "topKeyboardMoveStart", createDirectEvent(rt, "onKeyboardMoveStart"));
        events.setProperty(rt, "topKeyboardMove", createDirectEvent(rt, "onKeyboardMove"));
        events.setProperty(rt, "topKeyboardMoveEnd", createDirectEvent(rt, "onKeyboardMoveEnd"));
        events.setProperty(rt, "topKeyboardMoveInteractive", createDirectEvent(rt, "onKeyboardMoveInteractive"));
        events.setProperty(rt, "topFocusedInputLayoutChanged", createDirectEvent(rt, "onFocusedInputLayoutChanged"));
        events.setProperty(rt, "topFocusedInputTextChanged", createDirectEvent(rt, "onFocusedInputTextChanged"));
        events.setProperty(rt, "topFocusedInputSelectionChanged",
                           createDirectEvent(rt, "onFocusedInputSelectionChanged"));
        return events;
    }
};
} // namespace rnoh

#endif
