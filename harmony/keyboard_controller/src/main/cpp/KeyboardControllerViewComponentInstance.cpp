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
#include <algorithm>
#include <cmath>
#include <functional>
#include <folly/dynamic.h>
#include <iostream>
#include <arkui/native_interface_focus.h>
#include "react/renderer/components/textinput/TextInputProps.h"

namespace rnoh {
using KeyboardControllerStatus = rnoh::KeyboardControllerStatus;

namespace {
constexpr int KBC_FOCUSED_INPUT_AREA_CHANGE_TARGET_ID = 0x4B4243;

static std::string keyboardTypeToString(facebook::react::KeyboardType type) {
    switch (type) {
        case facebook::react::KeyboardType::Default: return "default";
        case facebook::react::KeyboardType::EmailAddress: return "email-address";
        case facebook::react::KeyboardType::Numeric: return "numeric";
        case facebook::react::KeyboardType::PhonePad: return "phone-pad";
        case facebook::react::KeyboardType::NumberPad: return "number-pad";
        case facebook::react::KeyboardType::DecimalPad: return "decimal-pad";
        case facebook::react::KeyboardType::ASCIICapable: return "ascii-capable";
        case facebook::react::KeyboardType::NumbersAndPunctuation: return "numbers-and-punctuation";
        case facebook::react::KeyboardType::URL: return "url";
        case facebook::react::KeyboardType::NamePhonePad: return "name-phone-pad";
        case facebook::react::KeyboardType::Twitter: return "twitter";
        case facebook::react::KeyboardType::WebSearch: return "web-search";
        case facebook::react::KeyboardType::ASCIICapableNumberPad: return "ascii-capable-number-pad";
        case facebook::react::KeyboardType::VisiblePassword: return "visible-password";
    }
    return "default";
}
} // namespace

KeyboardControllerViewComponentInstance::KeyboardControllerViewComponentInstance(Context context)
    : CppComponentInstance(std::move(context)), ArkTSMessageHub::Observer(m_deps->arkTSMessageHub) {
    DLOG(INFO) << "KeyboardControllerViewComponentInstance";

    m_customNode.setCustomNodeDelegate(this);
}

KeyboardControllerViewComponentInstance::~KeyboardControllerViewComponentInstance() {
    cancelKeyboardAnimation();
    clearFocusedInputLayoutObserver();
    for (const auto &child : getChildren()) {
        clearTextInputDelegates(child);
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
    clearTextInputDelegates(childComponentInstance);
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
        m_isHandlingKeyboardHeightChange = true;
        if (this->enabled) {
            syncUpLayout();
        }
        this->keyboardHeightChangeHandle(previousKeyboardHeight);
        m_isHandlingKeyboardHeightChange = false;
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
        startKeyboardAnimation(previousKeyboardHeight, willHide ? 0 : this->keyboardHeight, target);
    }
}

void KeyboardControllerViewComponentInstance::cancelKeyboardAnimation() {
    ++m_keyboardAnimationGeneration;
    if (m_keyboardAnimationTask.has_value() && m_deps->taskExecutor) {
        m_deps->taskExecutor->cancelDelayedTask(m_keyboardAnimationTask.value());
    }
    m_keyboardAnimationTask.reset();
}

void KeyboardControllerViewComponentInstance::startKeyboardAnimation(double fromHeight, double toHeight, int target) {
    cancelKeyboardAnimation();
    const uint64_t generation = m_keyboardAnimationGeneration;
    constexpr int durationMs = 250;
    constexpr int frameMs = 8;
    const int totalFrames = std::max(1, durationMs / frameMs);
    facebook::react::KeyboardControllerViewEventEmitter::MoveEvent start = {
        toHeight > 0 ? toHeight : 0,
        toHeight > 0 ? 1.0 : 0.0,
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
    constexpr int frameMs = 8;
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
            facebook::react::KeyboardControllerViewEventEmitter::MoveEvent move = {
                height,
                denominator > 0 ? std::abs(height / denominator) : 0.0,
                durationMs,
                target};
            self->m_eventEmitter->onKeyboardMove(move);
            if (frame >= totalFrames) {
                facebook::react::KeyboardControllerViewEventEmitter::MoveEvent end = {
                    toHeight, toHeight > 0 ? 1.0 : 0.0, durationMs, target};
                self->m_eventEmitter->onKeyboardMoveEnd(end);
                self->m_keyboardAnimationTask.reset();
                return;
            }
            self->scheduleKeyboardAnimationFrame(generation, fromHeight, toHeight, target, frame + 1, totalFrames);
        },
        frameMs);
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
    if (name.find("TextInput") != std::string::npos || name.find("TextArea") != std::string::npos) {
        auto input = std::dynamic_pointer_cast<TextInputComponentInstance>(childComponentInstance);
        if (!input) {
            return;
        }
        ArkUINode &node = input->getLocalRootArkUINode();
        TextInputNode *inputNodePtr = dynamic_cast<TextInputNode *>(&node);
        if (!inputNodePtr) {
            TextAreaNode *textAreaNodePtr = dynamic_cast<TextAreaNode *>(&node);
            if (textAreaNodePtr) {
                textAreaNodePtr->setTextAreaNodeDelegate(this);
            }
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

void KeyboardControllerViewComponentInstance::clearTextInputDelegates(
    ComponentInstance::Shared const &component) {
    if (!component) {
        return;
    }
    ArkUINode &node = component->getLocalRootArkUINode();
    if (node.getArkUINodeHandle() == m_observedFocusedInputHandle) {
        clearFocusedInputLayoutObserver();
    }
    if (auto *inputNode = dynamic_cast<TextInputNode *>(&node)) {
        inputNode->setTextInputNodeDelegate(nullptr);
    } else if (auto *textAreaNode = dynamic_cast<TextAreaNode *>(&node)) {
        textAreaNode->setTextAreaNodeDelegate(nullptr);
    }
    for (const auto &child : component->getChildren()) {
        clearTextInputDelegates(child);
    }
}

void KeyboardControllerViewComponentInstance::onChange(std::string text) {
    facebook::react::KeyboardControllerViewEventEmitter::TextChangeEvent event = {text};
    if (this->enabled && m_eventEmitter) {
        syncUpLayout();
        m_eventEmitter->onFocusedInputTextChanged(event);
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
    syncUpLayout();
    dispatchSelectionToJS(
        static_cast<int>(focusedInput->getTag()),
        location,
        location + length);
}

void KeyboardControllerViewComponentInstance::dispatchSelectionToJS(
    int target, int32_t startPos, int32_t endPos) {
    if (!m_eventEmitter || !this->enabled) {
        return;
    }
    facebook::react::KeyboardControllerViewEventEmitter::InputSectionEvent event = {};
    event.target = target;
    event.selection.start.x = 0;
    event.selection.start.y = 0;
    event.selection.start.position = startPos;
    event.selection.end.x = 0;
    event.selection.end.y = 0;
    event.selection.end.position = endPos;
    m_eventEmitter->onFocusedInputSelectionChanged(event);
}

void KeyboardControllerViewComponentInstance::focusDidSet() {
    auto focused = findFocusedTextInput();
    if (!focused || !this->enabled) {
        this->textInputVector.clear();
        return;
    }
    this->textInputVector = ViewHierarchyNavigator::getAllInputFields(this->shared_from_this());
    int count = static_cast<int>(this->textInputVector.size());
    int currentIndex = -1;
    for (size_t i = 0; i < this->textInputVector.size(); ++i) {
        auto& input = this->textInputVector[i];
        if (input && input->getTag() == focused->getTag()) {
            currentIndex = static_cast<int>(i);
            break;
        }
    }
    this->textInputVector.clear();
   // 发送 focusDidSet 事件到 JS 层
    if (currentIndex >= 0) {
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
    postFocusedInputChanged();
    this->focusDidSet();
    syncUpLayout();
}

void KeyboardControllerViewComponentInstance::onBlur() {
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
    auto currentFocus = this->findFocusedTextInput();
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
    std::function<TextInputComponentInstance::Shared(ComponentInstance::Shared)> findFocused;
    findFocused = [&](ComponentInstance::Shared component) -> TextInputComponentInstance::Shared {
        if (!component) {
            return nullptr;
        }
        if (auto input = std::dynamic_pointer_cast<TextInputComponentInstance>(component)) {
            if (input->getLocalRootArkUINode().isFocused()) {
                return input;
            }
        }
        for (const auto &child : component->getChildren()) {
            if (auto input = findFocused(child)) {
                return input;
            }
        }
        return nullptr;
    };
    return findFocused(this->shared_from_this());
}

int KeyboardControllerViewComponentInstance::findParentScrollViewTarget(ComponentInstance::Shared const &input) {
    auto parent = input->getParent().lock();
    while (parent != nullptr) {
        if (parent->getComponentName().find("ScrollView") != std::string::npos) {
            return static_cast<int>(parent->getTag());
        }
        parent = parent->getParent().lock();
    }
    return -1;
}

double KeyboardControllerViewComponentInstance::pxToVp(double px) const {
    auto pointScaleFactor = this->getLayoutMetrics().pointScaleFactor;
    return pointScaleFactor == 0.0 ? px : px / pointScaleFactor;
}

void KeyboardControllerViewComponentInstance::focusedInputLayoutEventReceiver(ArkUI_NodeEvent *event) {
    if (event == nullptr || OH_ArkUI_NodeEvent_GetEventType(event) != NODE_EVENT_ON_AREA_CHANGE ||
        OH_ArkUI_NodeEvent_GetTargetId(event) != KBC_FOCUSED_INPUT_AREA_CHANGE_TARGET_ID) {
        return;
    }
    auto *instance = static_cast<KeyboardControllerViewComponentInstance *>(OH_ArkUI_NodeEvent_GetUserData(event));
    if (instance != nullptr) {
        instance->handleFocusedInputLayoutEvent(event);
    }
}

void KeyboardControllerViewComponentInstance::handleFocusedInputLayoutEvent(ArkUI_NodeEvent *event) {
    if (!this->enabled || event == nullptr || OH_ArkUI_NodeEvent_GetNodeHandle(event) != m_observedFocusedInputHandle) {
        return;
    }
    syncUpLayout();
}

void KeyboardControllerViewComponentInstance::clearFocusedInputLayoutObserver() {
    if (m_observedFocusedInputHandle == nullptr) {
        return;
    }
    auto *nodeApi = NativeNodeApi::getInstance();
    nodeApi->unregisterNodeEvent(m_observedFocusedInputHandle, NODE_EVENT_ON_AREA_CHANGE);
    nodeApi->removeNodeEventReceiver(
        m_observedFocusedInputHandle,
        KeyboardControllerViewComponentInstance::focusedInputLayoutEventReceiver);
    m_observedFocusedInputHandle = nullptr;
}

void KeyboardControllerViewComponentInstance::updateFocusedInputLayoutObserver(
    TextInputComponentInstance::Shared const &focusedInput) {
    ArkUI_NodeHandle handle = focusedInput
        ? focusedInput->getLocalRootArkUINode().getArkUINodeHandle()
        : nullptr;
    if (handle == m_observedFocusedInputHandle) {
        return;
    }
    clearFocusedInputLayoutObserver();
    if (handle == nullptr) {
        return;
    }
    auto *nodeApi = NativeNodeApi::getInstance();
    auto addRet = nodeApi->addNodeEventReceiver(
        handle, KeyboardControllerViewComponentInstance::focusedInputLayoutEventReceiver);
    auto registerRet = nodeApi->registerNodeEvent(
        handle, NODE_EVENT_ON_AREA_CHANGE, KBC_FOCUSED_INPUT_AREA_CHANGE_TARGET_ID, this);
    if (addRet != ARKUI_ERROR_CODE_NO_ERROR || registerRet != ARKUI_ERROR_CODE_NO_ERROR) {
        nodeApi->unregisterNodeEvent(handle, NODE_EVENT_ON_AREA_CHANGE);
        nodeApi->removeNodeEventReceiver(
            handle, KeyboardControllerViewComponentInstance::focusedInputLayoutEventReceiver);
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
    ArkUI_IntOffset windowOffset{};
    ArkUI_IntOffset localOffset{};
    ArkUI_IntSize size{};
    if (handle == nullptr ||
        OH_ArkUI_NodeUtils_GetLayoutPositionInWindow(handle, &windowOffset) != ARKUI_ERROR_CODE_NO_ERROR ||
        OH_ArkUI_NodeUtils_GetLayoutPosition(handle, &localOffset) != ARKUI_ERROR_CODE_NO_ERROR ||
        OH_ArkUI_NodeUtils_GetLayoutSize(handle, &size) != ARKUI_ERROR_CODE_NO_ERROR) {
        return;
    }
    FocusedInputLayoutData event;
    event.absoluteX = pxToVp(windowOffset.x);
    event.absoluteY = pxToVp(windowOffset.y);
    event.x = pxToVp(localOffset.x);
    event.y = pxToVp(localOffset.y);
    event.width = pxToVp(size.width);
    event.height = pxToVp(size.height);
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
    if (!m_eventEmitter || !this->enabled) {
        return;
    }
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

void KeyboardControllerViewComponentInstance::dispatchKeyboardFocusChangedIfNeeded() {
    if (m_isHandlingKeyboardHeightChange) {
        return;
    }

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
        this->keyboardHeight, 1.0, 0, target};
    m_lastKeyboardEventTarget = target;
    m_eventEmitter->onKeyboardMoveStart(event);
    m_eventEmitter->onKeyboardMoveEnd(event);
}

void KeyboardControllerViewComponentInstance::postFocusedInputChanged() {
    auto focusedInput = findFocusedTextInput();
    int target = -1;
    std::string type = "default";
    if (focusedInput) {
        target = static_cast<int>(focusedInput->getTag());
        auto props = std::dynamic_pointer_cast<const facebook::react::TextInputProps>(focusedInput->getProps());
        if (props) {
            type = keyboardTypeToString(props->traits.keyboardType);
        }
    }
    auto rnInstancePtr = this->m_deps->rnInstance.lock();
    if (rnInstancePtr != nullptr) {
        folly::dynamic payload = folly::dynamic::object("target", target)("type", type);
        rnInstancePtr->postMessageToArkTS("focusedInputChanged", std::move(payload));
    }
}
} // namespace rnoh
