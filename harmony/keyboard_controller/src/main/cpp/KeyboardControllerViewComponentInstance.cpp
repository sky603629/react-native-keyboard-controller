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
#include "RNOH/arkui/NativeNodeApi.h"
#include <algorithm>
#include <cmath>
#include <folly/dynamic.h>
#include <iostream>
#include <arkui/native_interface_focus.h>
#include "react/renderer/components/textinput/TextInputProps.h"

namespace rnoh {
using KeyboardControllerStatus = rnoh::KeyboardControllerStatus;

namespace {
constexpr int KBC_FOCUSED_INPUT_AREA_CHANGE_TARGET_ID = 91001;
}

// Map RN KeyboardType enum -> KeyboardTypeOptions string for KeyboardEventData.type
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

    this->enabled = false;
    this->statusBarTranslucent = false;
    this->navigationBarTranslucent = false;
    this->preserveEdgeToEdge = false;
    this->keyboardHeight = 0;
    this->keyboardStatus = KeyboardControllerStatus::HIDE;
    m_customNode.setCustomNodeDelegate(this);
}

void KeyboardControllerViewComponentInstance::onChildInserted(ComponentInstance::Shared const &childComponentInstance,
                                                              std::size_t index) {
    CppComponentInstance::onChildInserted(childComponentInstance, index);
    m_customNode.insertChild(childComponentInstance->getLocalRootArkUINode(), index);
    DLOG(INFO) << "###111" << childComponentInstance->getComponentName();
    findTextInputComponents(childComponentInstance);
}

void KeyboardControllerViewComponentInstance::onChildRemoved(ComponentInstance::Shared const &childComponentInstance) {
    if (childComponentInstance->getLocalRootArkUINode().getArkUINodeHandle() == m_observedFocusedInputHandle) {
        clearFocusedInputLayoutObserver();
    }
    CppComponentInstance::onChildRemoved(childComponentInstance);
    m_customNode.removeChild(childComponentInstance->getLocalRootArkUINode());
}

void KeyboardControllerViewComponentInstance::onPropsChanged(SharedConcreteProps const &props) {
    DLOG(INFO) << "###onPropsChanged" << props->enabled << props->statusBarTranslucent
               << props->navigationBarTranslucent << props->preserveEdgeToEdge;
    CppComponentInstance::onPropsChanged(props);
    if (this->enabled != props->enabled) {
        this->enabled = props->enabled;
        this->startKeyboardObserver();
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
        double previousKeyboardHeight = this->keyboardHeight;
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
            this->keyboardHeight = previousKeyboardHeight > 0 ? previousKeyboardHeight : 0;
            m_lastKeyboardEventTarget = -1;
        }
        if (this->enabled) {
            syncUpLayout();
        }
        this->keyboardHeightChangeHandle(previousKeyboardHeight);
        if (height == 0) {
            this->keyboardHeight = 0;
        }
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
        arkTsTurboModule->callSync("startKeyboardObserver", {this->enabled, true});
    }
}


void KeyboardControllerViewComponentInstance::keyboardHeightChangeHandle(double previousKeyboardHeight) {
    auto rnInstancePtr = this->m_deps->rnInstance.lock();
    if (rnInstancePtr != nullptr && this->enabled) {
        int target = m_tag;
        auto focusedInput = findFocusedTextInput();
        if (focusedInput) {
            target = static_cast<int>(focusedInput->getTag());
        }
        m_lastKeyboardEventTarget = target;
        const bool willHide = this->keyboardStatus == KeyboardControllerStatus::HIDE;
        const double fromHeight = previousKeyboardHeight;
        const double toHeight = willHide ? 0 : this->keyboardHeight;
        startKeyboardAnimation(fromHeight, toHeight, target);
    }
}

void KeyboardControllerViewComponentInstance::cancelKeyboardAnimation() {
    ++m_keyboardAnimationGeneration;
    auto taskExecutor = m_deps->taskExecutor;
    if (taskExecutor && m_keyboardAnimationTask.has_value()) {
        taskExecutor->cancelDelayedTask(m_keyboardAnimationTask.value());
    }
    m_keyboardAnimationTask.reset();
}

void KeyboardControllerViewComponentInstance::startKeyboardAnimation(double fromHeight, double toHeight, int target) {
    cancelKeyboardAnimation();
    const uint64_t generation = m_keyboardAnimationGeneration;
    constexpr int durationMs = 250;
    constexpr int frameMs = 8;
    const int totalFrames = std::max(1, durationMs / frameMs);
    const double startHeight = toHeight > 0 ? toHeight : 0;
    const double startProgress = toHeight > 0 ? 1.0 : 0.0;

    facebook::react::KeyboardControllerViewEventEmitter::MoveEvent start = {
        startHeight,
        startProgress,
        durationMs,
        target};
    m_eventEmitter->onKeyboardMoveStart(start);
    scheduleKeyboardAnimationFrame(generation, fromHeight, toHeight, target, 1, totalFrames);
}

void KeyboardControllerViewComponentInstance::scheduleKeyboardAnimationFrame(
    uint64_t generation,
    double fromHeight,
    double toHeight,
    int target,
    int frame,
    int totalFrames) {
    auto taskExecutor = m_deps->taskExecutor;
    if (!taskExecutor || !m_eventEmitter || !this->enabled) {
        return;
    }

    auto weakSelf = std::weak_ptr<KeyboardControllerViewComponentInstance>(
        std::static_pointer_cast<KeyboardControllerViewComponentInstance>(shared_from_this()));
    m_keyboardAnimationTask = taskExecutor->runDelayedTask(
        TaskThread::MAIN,
        [weakSelf, generation, fromHeight, toHeight, target, frame, totalFrames] {
            auto self = weakSelf.lock();
            if (!self || generation != self->m_keyboardAnimationGeneration || !self->m_eventEmitter || !self->enabled) {
                return;
            }

            constexpr int durationMs = 250;
            const double progress = static_cast<double>(frame) / static_cast<double>(totalFrames);
            const double height = fromHeight + (toHeight - fromHeight) * progress;
            const double denominator = std::max(fromHeight, toHeight);
            const double eventProgress = denominator > 0 ? std::abs(height / denominator) : 0.0;
            facebook::react::KeyboardControllerViewEventEmitter::MoveEvent move = {
                height,
                eventProgress,
                durationMs,
                target};
            self->m_eventEmitter->onKeyboardMove(move);

            if (frame >= totalFrames) {
                facebook::react::KeyboardControllerViewEventEmitter::MoveEvent end = {
                    toHeight,
                    toHeight > 0 ? 1.0 : 0.0,
                    durationMs,
                    target};
                self->m_eventEmitter->onKeyboardMoveEnd(end);
                self->m_keyboardAnimationTask.reset();
                return;
            }

            self->scheduleKeyboardAnimationFrame(generation, fromHeight, toHeight, target, frame + 1, totalFrames);
        },
        16);
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
        syncUpLayout();
        m_eventEmitter->onFocusedInputTextChanged(event);
     }

}

void KeyboardControllerViewComponentInstance::onChange(std::string text, std::string extendStr) {
    onChange(std::move(text));
}

void KeyboardControllerViewComponentInstance::onTextSelectionChange(int32_t location, int32_t length) {
    DLOG(INFO) << "onKeyboardControllerView onTextSelectionChange loc=" << location << " len=" << length;
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
    // Convert caret geometry px -> vp to match layout metrics used by JS maybeScroll/bottomOffset.
    double outX = 0.0;
    double outY = 0.0;
    if (ok) {
        outX = pxToVp(static_cast<double>(caretX));
        outY = pxToVp(static_cast<double>(caretY));
    }
    // Keep layout in sync so absoluteY/height used by bottomOffset stay fresh while typing/selecting.
    syncUpLayout();
    dispatchSelectionToJS(target, location, endPos, outX, outY);
}

void KeyboardControllerViewComponentInstance::dispatchSelectionToJS(
    int target, int32_t startPos, int32_t endPos, double caretX, double caretY) {
    if (!m_eventEmitter || !this->enabled) {
        return;
    }
    // position: character indices; x/y: caret relative to input (vp).
    // Range selection: no official selection-end geometry API; both ends use caret (upstream-like anchor on end.y).
    facebook::react::KeyboardControllerViewEventEmitter::InputSectionEvent event = {};
    event.target = target;
    event.selection.start.x = caretX;
    event.selection.start.y = caretY;
    event.selection.start.position = startPos;
    event.selection.end.x = caretX;
    event.selection.end.y = caretY;
    event.selection.end.position = endPos;
    DLOG(INFO) << "###cpp dispatchSelectionToJS target=" << target
               << " start=" << startPos << " end=" << endPos
               << " x=" << caretX << " y=" << caretY;
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
    // Early push focused input {target, type} for will/did payload (before keyboard events)
    this->postFocusedInputChanged();
    this->focusDidSet();
    syncUpLayout();
}

void KeyboardControllerViewComponentInstance::onBlur() {
    DLOG(INFO) << " onKeyboardControllerView onBlur";
    clearFocusedInputLayoutObserver();
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

void KeyboardControllerViewComponentInstance::focusedInputLayoutEventReceiver(ArkUI_NodeEvent *event) {
    if (event == nullptr) {
        return;
    }
    auto eventType = OH_ArkUI_NodeEvent_GetEventType(event);
    auto targetId = OH_ArkUI_NodeEvent_GetTargetId(event);
    if (eventType != NODE_EVENT_ON_AREA_CHANGE || targetId != KBC_FOCUSED_INPUT_AREA_CHANGE_TARGET_ID) {
        return;
    }
    auto instance = static_cast<KeyboardControllerViewComponentInstance *>(OH_ArkUI_NodeEvent_GetUserData(event));
    if (instance == nullptr) {
        return;
    }
    instance->handleFocusedInputLayoutEvent(event);
}

void KeyboardControllerViewComponentInstance::handleFocusedInputLayoutEvent(ArkUI_NodeEvent *event) {
    if (!this->enabled || event == nullptr) {
        return;
    }
    auto eventType = OH_ArkUI_NodeEvent_GetEventType(event);
    if (eventType != NODE_EVENT_ON_AREA_CHANGE) {
        return;
    }
    auto handle = OH_ArkUI_NodeEvent_GetNodeHandle(event);
    if (handle == nullptr || handle != m_observedFocusedInputHandle) {
        return;
    }
    syncUpLayout();
}

void KeyboardControllerViewComponentInstance::clearFocusedInputLayoutObserver() {
    if (m_observedFocusedInputHandle == nullptr) {
        return;
    }
    auto nodeApi = NativeNodeApi::getInstance();
    nodeApi->unregisterNodeEvent(m_observedFocusedInputHandle, NODE_EVENT_ON_AREA_CHANGE);
    nodeApi->removeNodeEventReceiver(
        m_observedFocusedInputHandle,
        KeyboardControllerViewComponentInstance::focusedInputLayoutEventReceiver);
    m_observedFocusedInputHandle = nullptr;
}

void KeyboardControllerViewComponentInstance::updateFocusedInputLayoutObserver(
    TextInputComponentInstance::Shared const &focusedInput) {
    ArkUI_NodeHandle handle = nullptr;
    if (focusedInput != nullptr) {
        handle = focusedInput->getLocalRootArkUINode().getArkUINodeHandle();
    }

    if (handle == m_observedFocusedInputHandle) {
        return;
    }

    clearFocusedInputLayoutObserver();
    if (handle == nullptr) {
        return;
    }

    auto nodeApi = NativeNodeApi::getInstance();
    auto addRet = nodeApi->addNodeEventReceiver(
        handle,
        KeyboardControllerViewComponentInstance::focusedInputLayoutEventReceiver);
    auto registerRet = nodeApi->registerNodeEvent(
        handle,
        NODE_EVENT_ON_AREA_CHANGE,
        KBC_FOCUSED_INPUT_AREA_CHANGE_TARGET_ID,
        this);
    if (addRet != ARKUI_ERROR_CODE_NO_ERROR || registerRet != ARKUI_ERROR_CODE_NO_ERROR) {
        nodeApi->unregisterNodeEvent(handle, NODE_EVENT_ON_AREA_CHANGE);
        nodeApi->removeNodeEventReceiver(
            handle,
            KeyboardControllerViewComponentInstance::focusedInputLayoutEventReceiver);
        return;
    }

    m_observedFocusedInputHandle = handle;
}

void KeyboardControllerViewComponentInstance::syncUpLayout() {
    auto focusedInput = findFocusedTextInput();
    if (!focusedInput) {
        clearFocusedInputLayoutObserver();
        return;
    }

    updateFocusedInputLayoutObserver(focusedInput);

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
    dispatchKeyboardFocusChangedIfNeeded();
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

void KeyboardControllerViewComponentInstance::dispatchKeyboardFocusChangedIfNeeded() {
    if (!this->enabled || !m_eventEmitter || this->keyboardHeight <= 0) {
        return;
    }

    auto focusedInput = findFocusedTextInput();
    if (!focusedInput) {
        return;
    }

    int target = static_cast<int>(focusedInput->getTag());
    if (m_lastKeyboardEventTarget == -1 || target == m_lastKeyboardEventTarget) {
        return;
    }

    cancelKeyboardAnimation();
    facebook::react::KeyboardControllerViewEventEmitter::MoveEvent event = {
        this->keyboardHeight,
        1.0,
        0,
        target};
    m_lastKeyboardEventTarget = target;
    m_eventEmitter->onKeyboardMoveStart(event);
    m_eventEmitter->onKeyboardMoveEnd(event);
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
    // Cast may fail for non-TextInputProps; keep target real and type default
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
} // namespace rnoh
