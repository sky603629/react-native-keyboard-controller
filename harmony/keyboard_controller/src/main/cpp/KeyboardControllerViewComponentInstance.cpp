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

#include "KeyboardControllerViewComponentInstance.h"
#include <folly/dynamic.h>
#include <iostream>
#include <arkui/native_interface_focus.h>

namespace rnoh {
using KeyboardControllerStatus = rnoh::KeyboardControllerStatus;
KeyboardControllerViewComponentInstance::KeyboardControllerViewComponentInstance(Context context)
    : CppComponentInstance(std::move(context)), ArkTSMessageHub::Observer(m_deps->arkTSMessageHub) {
    DLOG(INFO) << "KeyboardControllerViewComponentInstance";

    m_customNode.setCustomNodeDelegate(this);
}

KeyboardControllerViewComponentInstance::~KeyboardControllerViewComponentInstance() {
    if (this->disableSystemKeyboardAvoidance) {
        this->disableSystemKeyboardAvoidance = false;
        this->updateKeyboardAvoidMode();
    }
}

void KeyboardControllerViewComponentInstance::onChildInserted(ComponentInstance::Shared const &childComponentInstance,
                                                              std::size_t index) {
    CppComponentInstance::onChildInserted(childComponentInstance, index);
    m_customNode.insertChild(childComponentInstance->getLocalRootArkUINode(), index);
    DLOG(INFO) << "###111" << childComponentInstance->getComponentName();
    findTextInputComponents(childComponentInstance);
}

void KeyboardControllerViewComponentInstance::onChildRemoved(ComponentInstance::Shared const &childComponentInstance) {
    CppComponentInstance::onChildRemoved(childComponentInstance);
    m_customNode.removeChild(childComponentInstance->getLocalRootArkUINode());
}

void KeyboardControllerViewComponentInstance::onPropsChanged(SharedConcreteProps const &props) {
    DLOG(INFO) << "###onPropsChanged" << props->enabled << props->statusBarTranslucent
               << props->navigationBarTranslucent << props->preserveEdgeToEdge
               << props->disableSystemKeyboardAvoidance;
    CppComponentInstance::onPropsChanged(props);
    bool enabledChanged = this->enabled != props->enabled;
    bool keyboardAvoidanceChanged =
        this->disableSystemKeyboardAvoidance != props->disableSystemKeyboardAvoidance;
    if (enabledChanged) {
        this->enabled = props->enabled;
    }
    if (keyboardAvoidanceChanged) {
        this->disableSystemKeyboardAvoidance = props->disableSystemKeyboardAvoidance;
    }
    if (enabledChanged) {
        this->startKeyboardObserver();
    } else if (keyboardAvoidanceChanged) {
        this->updateKeyboardAvoidMode();
    }
    if (this->navigationBarTranslucent != props->navigationBarTranslucent) {
        this->navigationBarTranslucent = props->navigationBarTranslucent;
        this->setWindowSystemBarEnable();
    }
    if (this->statusBarTranslucent != props->statusBarTranslucent) {
        this->statusBarTranslucent = props->statusBarTranslucent;
        this->setWindowSystemBarEnable();
    }
    if (this->preserveEdgeToEdge != props->preserveEdgeToEdge) {
        this->preserveEdgeToEdge = props->preserveEdgeToEdge;
        this->setWindowLayoutFullScreen();
    }
}

void KeyboardControllerViewComponentInstance::onCommandReceived(std::string const &commandName,
                                                                folly::dynamic const &args) {
    CppComponentInstance::onCommandReceived(commandName, args);
    if (commandName == "synchronizeFocusedInputLayout") {
        syncUpLayout();
    }
}

ArkUINode &KeyboardControllerViewComponentInstance::getLocalRootArkUINode() { return m_customNode; }
void KeyboardControllerViewComponentInstance::onClick() {}
void KeyboardControllerViewComponentInstance::onMessageReceived(ArkTSMessage const &message) {
    DLOG(INFO) << "onMessageReceived";
    if (message.name == "keyboardHeightChange") {
        double height = message.payload.getDouble();
        DLOG(INFO) << "keyboardHeightChange: " << height;
        if (height > 0) {
            this->keyboardStatus = KeyboardControllerStatus::SHOW;
            this->keyboardHeight = height;
            this->textInputVector.clear();
            this->focusDidSet();
            auto textInputVectorTemp = ViewHierarchyNavigator::getAllInputFields(this->shared_from_this());
            for (size_t i = 0; i < textInputVectorTemp.size(); ++i) {
                auto &input = textInputVectorTemp[i];
                findTextInputComponents(input);
            }
        }
        if (height == 0) {
            this->keyboardStatus = KeyboardControllerStatus::HIDE;
            this->keyboardHeight = 0;
            this->m_lastFocusedInputTarget = -1;
        }
        if (this->enabled) {
            syncUpLayout();
        }
        this->keyboardHeightChangeHandle();
    }
    if (message.name == "setFocusTo") {
        std::string direction = message.payload.getString();
        this->setFocusTo(direction);
    }
}
void KeyboardControllerViewComponentInstance::setWindowSystemBarEnable() {
    auto rnInstancePtr = this->m_deps->rnInstance.lock();
    if (rnInstancePtr != nullptr) {
        auto turboModule = rnInstancePtr->getTurboModule("KeyboardController");
        auto arkTsTurboModule = std::dynamic_pointer_cast<rnoh::ArkTSTurboModule>(turboModule);
        arkTsTurboModule->callSync("setWindowSystemBarEnable", {this->statusBarTranslucent, this->navigationBarTranslucent});
    }
}

void KeyboardControllerViewComponentInstance::startKeyboardObserver() {
    auto rnInstancePtr = this->m_deps->rnInstance.lock();
    if (rnInstancePtr != nullptr) {
        auto turboModule = rnInstancePtr->getTurboModule("KeyboardController");
        auto arkTsTurboModule = std::dynamic_pointer_cast<rnoh::ArkTSTurboModule>(turboModule);
        arkTsTurboModule->callSync("startKeyboardObserver", {this->enabled, this->disableSystemKeyboardAvoidance});
    }
}

void KeyboardControllerViewComponentInstance::updateKeyboardAvoidMode() {
    auto rnInstancePtr = this->m_deps->rnInstance.lock();
    if (rnInstancePtr != nullptr) {
        auto turboModule = rnInstancePtr->getTurboModule("KeyboardController");
        auto arkTsTurboModule = std::dynamic_pointer_cast<rnoh::ArkTSTurboModule>(turboModule);
        arkTsTurboModule->callSync("setKeyboardAvoidModeEnabled",
                                   {this->enabled && this->disableSystemKeyboardAvoidance});
    }
}


void KeyboardControllerViewComponentInstance::keyboardHeightChangeHandle() {
    auto rnInstancePtr = this->m_deps->rnInstance.lock();
    if (rnInstancePtr != nullptr && this->enabled) {
        auto focusedInput = findFocusedTextInput();
        int eventTarget = focusedInput ? static_cast<int>(focusedInput->getTag()) : -1;
        if (this->keyboardStatus == KeyboardControllerStatus::HIDE) {
            facebook::react::KeyboardControllerViewEventEmitter::MoveEvent start = {0, 0, 0, eventTarget};
            facebook::react::KeyboardControllerViewEventEmitter::MoveEvent end = {0, 0, 0, eventTarget};
            m_eventEmitter->onKeyboardMoveStart(start);
              m_eventEmitter->onKeyboardMove(end);
            m_eventEmitter->onKeyboardMoveEnd(end);
        } else {
            facebook::react::KeyboardControllerViewEventEmitter::MoveEvent start = {this->keyboardHeight, 0, 0, eventTarget};
            facebook::react::KeyboardControllerViewEventEmitter::MoveEvent end = {this->keyboardHeight, 1, 0, eventTarget};
            m_eventEmitter->onKeyboardMoveStart(start);
             m_eventEmitter->onKeyboardMove(end);
            m_eventEmitter->onKeyboardMoveEnd(end);
        }
        this->m_lastFocusedInputTarget =
            this->keyboardStatus == KeyboardControllerStatus::HIDE ? -1 : eventTarget;
    }
}

void KeyboardControllerViewComponentInstance::setWindowLayoutFullScreen() {
    auto rnInstancePtr = this->m_deps->rnInstance.lock();
    if (rnInstancePtr != nullptr) {
        auto turboModule = rnInstancePtr->getTurboModule("KeyboardController");
        auto arkTsTurboModule = std::dynamic_pointer_cast<rnoh::ArkTSTurboModule>(turboModule);
        arkTsTurboModule->callSync("setWindowLayoutFullScreen", {this->preserveEdgeToEdge});
    }
}

/**
 * 遍历查找TextInput
 * */
void KeyboardControllerViewComponentInstance::findTextInputComponents(
    ComponentInstance::Shared const &childComponentInstance) {
    std::string name = childComponentInstance->getComponentName();
    DLOG(INFO) << "###" << name;
    if (name.find("TextInput") != std::string::npos || name.find("TextArea") != std::string::npos) {
        auto input = std::dynamic_pointer_cast<TextInputComponentInstance>(childComponentInstance);
        ArkUINode &node = input->getLocalRootArkUINode();
        TextInputNode *inputNodePtr = dynamic_cast<TextInputNode *>(&node);
        if (!inputNodePtr) {
            TextAreaNode *textAreaNodePtr = dynamic_cast<TextAreaNode *>(&node);
            textAreaNodePtr->setTextAreaNodeDelegate(this);
        } else {
            inputNodePtr->setTextInputNodeDelegate(this);
        }
        this->textInputVector.push_back(input);
    }
    auto childrenComponentInstances = childComponentInstance->getChildren();
    if (childrenComponentInstances.empty()) {
        return;
    }

    std::vector<ComponentInstance::Shared>::iterator it1;
    for (it1 = childrenComponentInstances.begin(); it1 != childrenComponentInstances.end(); ++it1) {
        ComponentInstance::Shared const &t = *it1;
        findTextInputComponents(t);
    }
}

void KeyboardControllerViewComponentInstance::onChange(std::string text) {
    facebook::react::KeyboardControllerViewEventEmitter::TextChangeEvent event = {text};
    if(this->enabled){
        m_eventEmitter->onFocusedInputTextChanged(event);
        syncUpLayout();
     }

}

void KeyboardControllerViewComponentInstance::onChange(std::string text, std::string extendStr) {
    onChange(std::move(text));
}

void KeyboardControllerViewComponentInstance::onTextSelectionChange(int32_t location, int32_t length) {
    DLOG(INFO) << " onKeyboardControllerView onTextSelectionChange";
    // to do
};

void KeyboardControllerViewComponentInstance::focusDidSet() {
    int currentIndex = -1;
    this->textInputVector = ViewHierarchyNavigator::getAllInputFields(this->shared_from_this());
    int count = static_cast<int>(this->textInputVector.size());
    for (size_t i = 0; i < this->textInputVector.size(); ++i) {
        auto& input = this->textInputVector[i];
        ArkUINode& node = input->getLocalRootArkUINode();
        if (node.isFocused()) {
           currentIndex = static_cast<int>(i);
           break;
        }
    }
    this->textInputVector.clear();
   // 发送 focusDidSet 事件到 JS 层
    if (currentIndex >= 0 && this->enabled) {
       auto rnInstancePtr = this->m_deps->rnInstance.lock();
       if (rnInstancePtr != nullptr) {
           folly::dynamic payload = folly::dynamic::object
               ("current", currentIndex)
               ("count", count);
           rnInstancePtr->postMessageToArkTS("focusDidSet", payload);
       }
   }
}

void KeyboardControllerViewComponentInstance::onFocus() {
    DLOG(INFO) << "onKeyboardControllerView onFocus";
    auto focusedInput = findFocusedTextInput();
    int focusedTarget = focusedInput ? static_cast<int>(focusedInput->getTag()) : -1;
    bool shouldDispatchFocusKeyboardEvents =
        this->enabled &&
        this->keyboardStatus == KeyboardControllerStatus::SHOW &&
        this->keyboardHeight > 0 &&
        this->m_lastFocusedInputTarget != -1 &&
        focusedTarget != -1 &&
        this->m_lastFocusedInputTarget != focusedTarget;
    this->focusDidSet();
    syncUpLayout();
    if (shouldDispatchFocusKeyboardEvents && m_eventEmitter) {
        facebook::react::KeyboardControllerViewEventEmitter::MoveEvent start = {
            this->keyboardHeight, 1, 0, focusedTarget};
        facebook::react::KeyboardControllerViewEventEmitter::MoveEvent end = {
            this->keyboardHeight, 1, 0, focusedTarget};
        m_eventEmitter->onKeyboardMoveStart(start);
        m_eventEmitter->onKeyboardMoveEnd(end);
    }
    if (focusedTarget != -1) {
        this->m_lastFocusedInputTarget = focusedTarget;
    }
}

void KeyboardControllerViewComponentInstance::onBlur() {
    DLOG(INFO) << " onKeyboardControllerView onBlur";
    m_lastLayoutEvent = FocusedInputLayoutData{};
    if (m_eventEmitter && this->enabled) {
        facebook::react::KeyboardControllerViewEventEmitter::InputLayoutEvent payload = {};
        payload.target = -1;
        payload.parentScrollViewTarget = -1;
        m_eventEmitter->onFocusedInputLayoutChanged(payload);
    }
}

/**
 * 设置焦点到指定方向的输入框
 * 实现逻辑仿照 iOS ViewHierarchyNavigator.swift
 * @param direction "next" | "prev"
 */
void KeyboardControllerViewComponentInstance::setFocusTo(const std::string& direction) {
    // 确定当前焦点组件
    this->textInputVector = ViewHierarchyNavigator::getAllInputFields(this->shared_from_this());
    ComponentInstance::Shared currentFocus = nullptr;
    for (size_t i = 0; i < this->textInputVector.size(); ++i) {
        auto& input = this->textInputVector[i];
        ArkUINode& node = input->getLocalRootArkUINode();
        if (node.isFocused()) {
           currentFocus = input;
           break;
        }
    }
    if (!currentFocus) {
        DLOG(INFO) << "no current focus available";
        return;
    }
    // 使用 ViewHierarchyNavigator 查找目标输入框
    auto targetInput = ViewHierarchyNavigator::setFocusTo(direction, currentFocus);
    if (targetInput) {
        DLOG(INFO) << "setFocusTo: found target, requesting focus, tag=" << targetInput->getTag();
        // 获取 ArkUI_NodeHandle 并请求焦点
        ArkUINode& node = targetInput->getLocalRootArkUINode();
        ArkUI_NodeHandle nodeHandle = node.getArkUINodeHandle();
        ArkUI_ErrorCode result = ARKUI_ERROR_CODE_NO_ERROR;
        high_lib_handle = dlopen("libace_ndk.z.so", RTLD_LAZY);
        if (high_lib_handle == NULL) {
            DLOG(INFO) << "libace_ndk.z.so not existed";
            dlclose(high_lib_handle);
        } else {
            focusRequestMethod = (ArkUI_ErrorCode(*)(ArkUI_NodeHandle node))dlsym(high_lib_handle, "OH_ArkUI_FocusRequest");
            bool isFocusRequestMethodExisted = focusRequestMethod != NULL;
            if (isFocusRequestMethodExisted) {
                DLOG(INFO) << "OH_ArkUI_FocusRequest method existed";
                result = focusRequestMethod(nodeHandle);
            } else {
                DLOG(INFO) << "OH_ArkUI_FocusRequest method not existed";
            }
            dlclose(high_lib_handle);
        }
        if (!(result == ARKUI_ERROR_CODE_NO_ERROR)) {
            DLOG(WARNING) << "setFocusTo: focus request failed with error code: " << result;
        }
    } else {
        DLOG(INFO) << "setFocusTo: no target input found in direction " << direction;
    }
    this->textInputVector.clear();
}

TextInputComponentInstance::Shared KeyboardControllerViewComponentInstance::findFocusedTextInput() {
    auto allInputs = ViewHierarchyNavigator::getAllInputFields(this->shared_from_this());
    for (auto &input : allInputs) {
        ArkUINode &node = input->getLocalRootArkUINode();
        if (node.isFocused()) {
            return input;
        }
    }
    return nullptr;
}

int KeyboardControllerViewComponentInstance::findParentScrollViewTarget(ComponentInstance::Shared const &input) {
    auto parent = input->getParent().lock();
    while (parent != nullptr) {
        std::string name = parent->getComponentName();
        if (name.find("ScrollView") != std::string::npos) {
            return static_cast<int>(parent->getTag());
        }
        parent = parent->getParent().lock();
    }
    return -1;
}

double KeyboardControllerViewComponentInstance::pxToVp(double px) const {
    auto pointScaleFactor = this->getLayoutMetrics().pointScaleFactor;
    if (pointScaleFactor == 0.0) {
        return px;
    }
    return px / pointScaleFactor;
}

void KeyboardControllerViewComponentInstance::syncUpLayout() {
    auto focusedInput = findFocusedTextInput();
    if (!focusedInput) {
        return;
    }

    ArkUI_NodeHandle handle = focusedInput->getLocalRootArkUINode().getArkUINodeHandle();
    if (handle == nullptr) {
        return;
    }

    ArkUI_IntOffset windowOffset{};
    ArkUI_IntOffset localOffset{};
    ArkUI_IntSize size{};
    int32_t windowRet = OH_ArkUI_NodeUtils_GetLayoutPositionInWindow(handle, &windowOffset);
    int32_t localRet = OH_ArkUI_NodeUtils_GetLayoutPosition(handle, &localOffset);
    int32_t sizeRet = OH_ArkUI_NodeUtils_GetLayoutSize(handle, &size);
    if (windowRet != ARKUI_ERROR_CODE_NO_ERROR ||
        localRet != ARKUI_ERROR_CODE_NO_ERROR ||
        sizeRet != ARKUI_ERROR_CODE_NO_ERROR) {
        DLOG(WARNING) << "syncUpLayout: measure failed windowRet=" << windowRet
                      << " localRet=" << localRet << " sizeRet=" << sizeRet;
        return;
    }

    FocusedInputLayoutData event;
    event.absoluteX = pxToVp(static_cast<double>(windowOffset.x));
    event.absoluteY = pxToVp(static_cast<double>(windowOffset.y));
    event.x = pxToVp(static_cast<double>(localOffset.x));
    event.y = pxToVp(static_cast<double>(localOffset.y));
    event.width = pxToVp(static_cast<double>(size.width));
    event.height = pxToVp(static_cast<double>(size.height));
    event.target = static_cast<int>(focusedInput->getTag());
    event.parentScrollViewTarget = findParentScrollViewTarget(focusedInput);

    dispatchLayoutToJS(event);
}

void KeyboardControllerViewComponentInstance::dispatchLayoutToJS(FocusedInputLayoutData const &event) {
    if (event == m_lastLayoutEvent) {
        return;
    }

    m_lastLayoutEvent = event;
    if (m_eventEmitter && this->enabled) {
        facebook::react::KeyboardControllerViewEventEmitter::InputLayoutEvent payload = {};
        payload.target = event.target;
        payload.parentScrollViewTarget = event.parentScrollViewTarget;
        payload.layout.absoluteX = event.absoluteX;
        payload.layout.absoluteY = event.absoluteY;
        payload.layout.x = event.x;
        payload.layout.y = event.y;
        payload.layout.width = event.width;
        payload.layout.height = event.height;
        m_eventEmitter->onFocusedInputLayoutChanged(payload);
    }
}
} // namespace rnoh
