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
#include <cmath>
#include <folly/dynamic.h>
#include <iostream>
#include <arkui/native_interface_focus.h>
#include "react/renderer/components/textinput/TextInputProps.h"

namespace rnoh {
using KeyboardControllerStatus = rnoh::KeyboardControllerStatus;

// KeyboardType 鏋氫妇(RN 鏍囧噯 14 鍊奸泦鍚? -> RN KeyboardTypeOptions 瀛楃涓?
// 瀵归綈 JS 灞?TextInputProps["keyboardType"] 鏈熸湜鍊?
static std::string keyboardTypeToString(facebook::react::KeyboardType type) {
    switch (type) {
        case facebook::react::KeyboardType::Default:
            return "default";
        case facebook::react::KeyboardType::EmailAddress:
            return "email-address";
        case facebook::react::KeyboardType::Numeric:
            return "numeric";
        case facebook::react::KeyboardType::PhonePad:
            return "phone-pad";
        case facebook::react::KeyboardType::NumberPad:
            return "number-pad";
        case facebook::react::KeyboardType::DecimalPad:
            return "decimal-pad";
        case facebook::react::KeyboardType::ASCIICapable:
            return "ascii-capable";
        case facebook::react::KeyboardType::NumbersAndPunctuation:
            return "numbers-and-punctuation";
        case facebook::react::KeyboardType::URL:
            return "url";
        case facebook::react::KeyboardType::NamePhonePad:
            return "name-phone-pad";
        case facebook::react::KeyboardType::Twitter:
            return "twitter";
        case facebook::react::KeyboardType::WebSearch:
            return "web-search";
        case facebook::react::KeyboardType::ASCIICapableNumberPad:
            return "ascii-capable-number-pad";
        case facebook::react::KeyboardType::VisiblePassword:
            return "visible-password";
    }
    return "default";
}
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
        // 鍏滃簳: 鍒锋柊 ArkTS 鐨勭劍鐐硅緭鍏ユ缂撳瓨(鏈浜嬩欢宸插お鏅? 鏈嶅姟涓嬩竴娆￠敭鐩樹簨浠?
        this->postFocusedInputChanged();
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
 * 閬嶅巻鏌ユ壘TextInput
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
    if (this->enabled) {
        m_eventEmitter->onFocusedInputTextChanged(event);
        syncUpLayout();
        // Text change moves caret; refresh geometry so KASV can anchor to caret y.
        auto focusedInput = findFocusedTextInput();
        if (focusedInput) {
            int target = static_cast<int>(focusedInput->getTag());
            ArkUINode &node = focusedInput->getLocalRootArkUINode();
            ArkUI_NodeHandle handle = node.getArkUINodeHandle();
            bool isTextArea = dynamic_cast<TextAreaNode *>(&node) != nullptr;
            int32_t caretIndex = 0;
            float caretX = 0.f;
            float caretY = 0.f;
            if (handle != nullptr && readCaretOffset(handle, isTextArea, caretIndex, caretX, caretY)) {
                dispatchSelectionToJS(
                    target, caretIndex, caretIndex,
                    static_cast<double>(caretX), static_cast<double>(caretY));
            }
        }
    }
}

void KeyboardControllerViewComponentInstance::onChange(std::string text, std::string extendStr) {
    onChange(std::move(text));
}

void KeyboardControllerViewComponentInstance::onTextSelectionChange(int32_t location, int32_t length) {
    if (!this->enabled) {
        return;
    }
    auto focusedInput = findFocusedTextInput();
    if (!focusedInput) {
        return;
    }
    int target = static_cast<int>(focusedInput->getTag());
    ArkUINode &node = focusedInput->getLocalRootArkUINode();
    ArkUI_NodeHandle handle = node.getArkUINodeHandle();
    bool isTextArea = dynamic_cast<TextAreaNode *>(&node) != nullptr;
    int32_t caretIndex = location;
    float caretX = 0.f;
    float caretY = 0.f;
    bool ok = handle != nullptr && readCaretOffset(handle, isTextArea, caretIndex, caretX, caretY);
    int32_t endPos = location + length;
    // y=-1 signals JS to use full input height (do not treat missing caret as y=0).
    double outX = ok ? static_cast<double>(caretX) : 0.0;
    double outY = ok ? static_cast<double>(caretY) : -1.0;
    dispatchSelectionToJS(target, location, endPos, outX, outY);
}

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
   // 鍙戦€?focusDidSet 浜嬩欢鍒?JS 灞?
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
    // 鏃╂湡鎺ㄩ€佺劍鐐硅緭鍏ユ {target, type} 缁?ArkTS(鏃╀簬閿洏寮硅捣浜嬩欢), 渚?will/did payload 浣跨敤
    this->postFocusedInputChanged();
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
 * 璁剧疆鐒︾偣鍒版寚瀹氭柟鍚戠殑杈撳叆妗?
 * 瀹炵幇閫昏緫浠跨収 iOS ViewHierarchyNavigator.swift
 * @param direction "next" | "prev"
 */
void KeyboardControllerViewComponentInstance::setFocusTo(const std::string& direction) {
    // 纭畾褰撳墠鐒︾偣缁勪欢
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
    // 浣跨敤 ViewHierarchyNavigator 鏌ユ壘鐩爣杈撳叆妗?
    auto targetInput = ViewHierarchyNavigator::setFocusTo(direction, currentFocus);
    if (targetInput) {
        DLOG(INFO) << "setFocusTo: found target, requesting focus, tag=" << targetInput->getTag();
        // 鑾峰彇 ArkUI_NodeHandle 骞惰姹傜劍鐐?
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

void KeyboardControllerViewComponentInstance::postFocusedInputChanged() {
    if (!this->enabled) {
        return;
    }
    auto focusedInput = findFocusedTextInput();
    if (!focusedInput) {
        return;
    }
    int target = static_cast<int>(focusedInput->getTag());
    std::string typeStr = "default";
    // 鍔ㄦ€佽浆鎹㈠彇 keyboardType; cast 澶辫触(闈?TextInputProps)鏃?type 鍏滃簳 "default", target 浠嶇湡瀹?
    auto textInputProps = std::dynamic_pointer_cast<const facebook::react::TextInputProps>(
        focusedInput->getProps());
    if (textInputProps) {
        typeStr = keyboardTypeToString(textInputProps->traits.keyboardType);
    }
    auto rnInstancePtr = this->m_deps->rnInstance.lock();
    if (rnInstancePtr != nullptr) {
        folly::dynamic payload = folly::dynamic::object
            ("target", target)
            ("type", typeStr);
        DLOG(INFO) << "###cpp postFocusedInputChanged target=" << target << " type=" << typeStr;
        rnInstancePtr->postMessageToArkTS("focusedInputChanged", std::move(payload));
    }
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
        DLOG(INFO) << "###cpp dispatchLayoutToJS target=" << event.target
                   << " parent=" << event.parentScrollViewTarget
                   << " absY=" << event.absoluteY << " height=" << event.height;
        m_eventEmitter->onFocusedInputLayoutChanged(payload);
    }
}

void KeyboardControllerViewComponentInstance::dispatchSelectionToJS(
    int target, int32_t startPos, int32_t endPos, double caretX, double caretY) {
    if (!m_eventEmitter || !this->enabled) {
        return;
    }
    // position: character indices; x/y: caret relative to input (vp), aligned with iOS caretRect / Android Layout.
    // For collapsed caret start==end geometrically; for ranges we still anchor scroll to caret end.y.
    facebook::react::KeyboardControllerViewEventEmitter::InputSectionEvent event = {};
    event.target = target;
    event.selection.start.x = caretX;
    event.selection.start.y = caretY;
    event.selection.start.position = startPos;
    event.selection.end.x = caretX;
    event.selection.end.y = caretY;
    event.selection.end.position = endPos;
    m_eventEmitter->onFocusedInputSelectionChanged(event);
}

bool KeyboardControllerViewComponentInstance::readCaretOffset(
    ArkUI_NodeHandle handle, bool isTextArea, int32_t &index, float &x, float &y) const {
    if (handle == nullptr) {
        return false;
    }
    auto *nodeApi = NativeNodeApi::getInstance();
    if (nodeApi == nullptr || nodeApi->getAttribute == nullptr) {
        return false;
    }

    auto tryRead = [&](ArkUI_NodeAttributeType attr) -> bool {
        const ArkUI_AttributeItem *item = nodeApi->getAttribute(handle, attr);
        if (item == nullptr || item->value == nullptr || item->size < 1) {
            return false;
        }
        // Documented: [i32 index, f32 x, f32 y]
        if (item->size >= 3) {
            index = item->value[0].i32;
            x = item->value[1].f32;
            y = item->value[2].f32;
            return std::isfinite(y) && y >= 0.f;
        }
        // Observed on Harmony: size=2 => [i32 index, f32 y]
        if (item->size == 2) {
            index = item->value[0].i32;
            x = 0.f;
            y = item->value[1].f32;
            return std::isfinite(y) && y >= 0.f;
        }
        return false;
    };

    ArkUI_NodeAttributeType primary =
        isTextArea ? NODE_TEXT_AREA_CARET_OFFSET : NODE_TEXT_INPUT_CARET_OFFSET;
    ArkUI_NodeAttributeType secondary =
        isTextArea ? NODE_TEXT_INPUT_CARET_OFFSET : NODE_TEXT_AREA_CARET_OFFSET;
    return tryRead(primary) || tryRead(secondary);
}
} // namespace rnoh



