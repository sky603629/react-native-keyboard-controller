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
 
#include "KeyboardGestureAreaComponentInstance.h"
#include <folly/dynamic.h>


namespace rnoh {
KeyboardGestureAreaComponentInstance::KeyboardGestureAreaComponentInstance(Context context)
    : CppComponentInstance(std::move(context)) {
    DLOG(INFO) << "KeyboardGestureAreaComponentInstance";
    ArkUINodeRegistry::getInstance().registerTouchHandler(&m_stackNode, this);
}
void KeyboardGestureAreaComponentInstance::onTouchEvent(ArkUI_UIInputEvent *e) {
    auto action = OH_ArkUI_UIInputEvent_GetAction(e);
    DLOG(INFO) << "KeyboardGestureAreaComponentInstance onTouchEvent" << action;
    if (action == UI_TOUCH_EVENT_ACTION_DOWN) {
        this->swipeToDismiss();
    }
    if (action == UI_TOUCH_EVENT_ACTION_UP) {
        this->swipeToUp();
    }
}
void KeyboardGestureAreaComponentInstance::onChildInserted(ComponentInstance::Shared const &childComponentInstance,
                                                           std::size_t index) {
    CppComponentInstance::onChildInserted(childComponentInstance, index);
    m_stackNode.insertChild(childComponentInstance->getLocalRootArkUINode(), index + 1);
}

void KeyboardGestureAreaComponentInstance::onChildRemoved(ComponentInstance::Shared const &childComponentInstance) {
    CppComponentInstance::onChildRemoved(childComponentInstance);
    m_stackNode.removeChild(childComponentInstance->getLocalRootArkUINode());
}
ArkUINode &KeyboardGestureAreaComponentInstance::getLocalRootArkUINode() { return m_stackNode; }
void KeyboardGestureAreaComponentInstance::onPropsChanged(SharedConcreteProps const &props) {
    DLOG(INFO) << "###onPropsChanged";
    CppComponentInstance::onPropsChanged(props);
    this->showOnSwipeUp = props->showOnSwipeUp;
    this->enableSwipeToDismiss = props->enableSwipeToDismiss;
    this->interpolator = props->interpolator;
}

void KeyboardGestureAreaComponentInstance::onCommandReceived(std::string const &commandName,
                                                             folly::dynamic const &args) {
    CppComponentInstance::onCommandReceived(commandName, args);
}

void KeyboardGestureAreaComponentInstance::handleTouchEvent(ArkUI_UIInputEvent *e) {
    auto action = OH_ArkUI_UIInputEvent_GetAction(e);
    if (action == UI_TOUCH_EVENT_ACTION_DOWN) {
        this->swipeToDismiss();
    }
    if (action == UI_TOUCH_EVENT_ACTION_UP) {
        this->swipeToUp();
    }
}
void KeyboardGestureAreaComponentInstance::swipeToDismiss() {
    if (!this->enableSwipeToDismiss) {
        return;
    }
    auto rnInstancePtr = this->m_deps->rnInstance.lock();
    if (rnInstancePtr != nullptr) {
        auto turboModule = rnInstancePtr->getTurboModule("KeyboardController");
        auto arkTsTurboModule = std::dynamic_pointer_cast<rnoh::ArkTSTurboModule>(turboModule);
        arkTsTurboModule->callSync("dismiss", {false, true});
    }
}
void KeyboardGestureAreaComponentInstance::swipeToUp() {
    if (!this->showOnSwipeUp) {
        return;
    }
    auto rnInstancePtr = this->m_deps->rnInstance.lock();
    if (rnInstancePtr != nullptr) {
        auto turboModule = rnInstancePtr->getTurboModule("KeyboardController");
        auto arkTsTurboModule = std::dynamic_pointer_cast<rnoh::ArkTSTurboModule>(turboModule);
        arkTsTurboModule->callSync("show", {});
    }
}
void KeyboardGestureAreaComponentInstance::executeInterpolator() {
    // to do
}

} // namespace rnoh
// namespace rnoh
