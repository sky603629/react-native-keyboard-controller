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
#include <arkui/native_node.h>
#include <folly/dynamic.h>
#include <dlfcn.h>
#include <iostream>
#include <arkui/native_interface_focus.h>

namespace rnoh {
using KeyboardControllerStatus = rnoh::KeyboardControllerStatus;

namespace {
using GetLayoutSizeFn = int32_t (*)(ArkUI_NodeHandle node, ArkUI_IntSize* size);
using GetLayoutPositionFn = int32_t (*)(ArkUI_NodeHandle node, ArkUI_IntOffset* offset);

double getPointScaleFactor(facebook::react::LayoutMetrics const &layoutMetrics) {
    return layoutMetrics.pointScaleFactor > 0 ? layoutMetrics.pointScaleFactor : 1;
}

bool updateLayoutFromArkUINode(
    TextInputComponentInstance::Shared const &input,
    facebook::react::KeyboardControllerViewEventEmitter::InputLayoutEvent &event) {
    if (!input) {
        return false;
    }

    ArkUI_NodeHandle nodeHandle = input->getLocalRootArkUINode().getArkUINodeHandle();
    if (nodeHandle == nullptr) {
        return false;
    }

    void *libHandle = dlopen("libace_ndk.z.so", RTLD_LAZY);
    if (libHandle == nullptr) {
        return false;
    }

    auto getLayoutSize = reinterpret_cast<GetLayoutSizeFn>(
        dlsym(libHandle, "OH_ArkUI_NodeUtils_GetLayoutSize"));
    auto getLayoutPosition = reinterpret_cast<GetLayoutPositionFn>(
        dlsym(libHandle, "OH_ArkUI_NodeUtils_GetLayoutPosition"));
    auto getLayoutPositionInWindow = reinterpret_cast<GetLayoutPositionFn>(
        dlsym(libHandle, "OH_ArkUI_NodeUtils_GetLayoutPositionInWindow"));

    const double scale = getPointScaleFactor(input->getLayoutMetrics());
    bool hasRuntimeLayout = false;

    if (getLayoutSize != nullptr) {
        ArkUI_IntSize size = {0, 0};
        if (getLayoutSize(nodeHandle, &size) == ARKUI_ERROR_CODE_NO_ERROR &&
            (size.width > 0 || size.height > 0)) {
            event.layout.width = size.width / scale;
            event.layout.height = size.height / scale;
            hasRuntimeLayout = true;
        }
    }

    if (getLayoutPosition != nullptr) {
        ArkUI_IntOffset localOffset = {0, 0};
        if (getLayoutPosition(nodeHandle, &localOffset) == ARKUI_ERROR_CODE_NO_ERROR) {
            event.layout.x = localOffset.x / scale;
            event.layout.y = localOffset.y / scale;
            hasRuntimeLayout = true;
        }
    }

    if (getLayoutPositionInWindow != nullptr) {
        ArkUI_IntOffset windowOffset = {0, 0};
        if (getLayoutPositionInWindow(nodeHandle, &windowOffset) == ARKUI_ERROR_CODE_NO_ERROR) {
            event.layout.absoluteX = windowOffset.x / scale;
            event.layout.absoluteY = windowOffset.y / scale;
            hasRuntimeLayout = true;
        }
    }

    dlclose(libHandle);
    return hasRuntimeLayout;
}
} // namespace

KeyboardControllerViewComponentInstance::KeyboardControllerViewComponentInstance(Context context)
    : CppComponentInstance(std::move(context)), ArkTSMessageHub::Observer(m_deps->arkTSMessageHub) {
    DLOG(INFO) << "KeyboardControllerViewComponentInstance";

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
        arkTsTurboModule->callSync("startKeyboardObserver", {this->enabled});
    }
}


void KeyboardControllerViewComponentInstance::keyboardHeightChangeHandle() {
    auto rnInstancePtr = this->m_deps->rnInstance.lock();
    if (rnInstancePtr != nullptr && this->enabled) {
        if (this->keyboardStatus == KeyboardControllerStatus::HIDE) {
            facebook::react::KeyboardControllerViewEventEmitter::MoveEvent start = {this->keyboardHeight, 0, 0, m_tag};
            facebook::react::KeyboardControllerViewEventEmitter::MoveEvent end = {0, 0, 0, m_tag};
            m_eventEmitter->onKeyboardMoveStart(start);
              m_eventEmitter->onKeyboardMove(end);
            m_eventEmitter->onKeyboardMoveEnd(end);
        } else {
            facebook::react::KeyboardControllerViewEventEmitter::MoveEvent start = {0, 0, 0, m_tag};
            facebook::react::KeyboardControllerViewEventEmitter::MoveEvent end = {this->keyboardHeight, 1, 0, m_tag};
            m_eventEmitter->onKeyboardMoveStart(start);
             m_eventEmitter->onKeyboardMove(end);
            m_eventEmitter->onKeyboardMoveEnd(end);
        }
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

TextInputComponentInstance::Shared KeyboardControllerViewComponentInstance::findFocusedInput() {
    auto inputs = ViewHierarchyNavigator::getAllInputFields(this->shared_from_this());
    for (auto &input : inputs) {
        ArkUINode &node = input->getLocalRootArkUINode();
        if (node.isFocused()) {
            return input;
        }
    }

    return nullptr;
}

TextInputComponentInstance::Shared KeyboardControllerViewComponentInstance::findInputByNode(ArkUINode* node) {
    if (!node) {
        return nullptr;
    }

    auto inputs = ViewHierarchyNavigator::getAllInputFields(this->shared_from_this());
    for (auto &input : inputs) {
        if (&input->getLocalRootArkUINode() == node) {
            return input;
        }
    }

    return nullptr;
}

facebook::react::Point KeyboardControllerViewComponentInstance::getAbsoluteOrigin(
    ComponentInstance::Shared const &componentInstance) {
    facebook::react::Point origin = {0, 0};
    auto current = componentInstance;

    while (current) {
        auto layoutMetrics = current->getShadowView().layoutMetrics;
        origin.x += layoutMetrics.frame.origin.x;
        origin.y += layoutMetrics.frame.origin.y;
        current = current->getParent().lock();
    }

    return origin;
}

int KeyboardControllerViewComponentInstance::findParentScrollViewTarget(
    ComponentInstance::Shared const &componentInstance) {
    auto current = componentInstance ? componentInstance->getParent().lock() : nullptr;

    while (current) {
        const std::string name = current->getComponentName();
        if (name.find("ScrollView") != std::string::npos) {
            return current->getTag();
        }
        current = current->getParent().lock();
    }

    return -1;
}

void KeyboardControllerViewComponentInstance::emitFocusedInputLayout(
    TextInputComponentInstance::Shared const &input) {
    if (!this->enabled || !input || !m_eventEmitter) {
        return;
    }

    auto layoutMetrics = input->getLayoutMetrics();
    auto absoluteOrigin = this->getAbsoluteOrigin(input);
    facebook::react::KeyboardControllerViewEventEmitter::InputLayoutEvent event;
    event.target = input->getTag();
    event.parentScrollViewTarget = this->findParentScrollViewTarget(input);
    event.layout.x = layoutMetrics.frame.origin.x;
    event.layout.y = layoutMetrics.frame.origin.y;
    event.layout.width = layoutMetrics.frame.size.width;
    event.layout.height = layoutMetrics.frame.size.height;
    event.layout.absoluteX = absoluteOrigin.x;
    event.layout.absoluteY = absoluteOrigin.y;
    updateLayoutFromArkUINode(input, event);
    m_eventEmitter->onFocusedInputLayoutChanged(event);
}

void KeyboardControllerViewComponentInstance::emitNoFocusedInputLayout() {
    if (!this->enabled || !m_eventEmitter) {
        return;
    }

    facebook::react::KeyboardControllerViewEventEmitter::InputLayoutEvent event;
    event.target = -1;
    event.parentScrollViewTarget = -1;
    event.layout.x = 0;
    event.layout.y = 0;
    event.layout.width = 0;
    event.layout.height = 0;
    event.layout.absoluteX = 0;
    event.layout.absoluteY = 0;
    m_eventEmitter->onFocusedInputLayoutChanged(event);
}

void KeyboardControllerViewComponentInstance::emitFocusedInputSelection(
    ArkUINode* node, int32_t location, int32_t length) {
    if (!this->enabled || !node || !m_eventEmitter) {
        return;
    }

    auto input = this->findInputByNode(node);
    if (!input) {
        input = this->findFocusedInput();
    }
    if (!input) {
        return;
    }

    const double scale = getPointScaleFactor(input->getLayoutMetrics());
    int32_t startPosition = location;
    int32_t endPosition = location + length;
    double caretX = 0;
    double caretY = 0;
    const bool isTextArea = dynamic_cast<TextAreaNode*>(node) != nullptr;
    auto nativeNodeApi = NativeNodeApi::getInstance();

    if (nativeNodeApi != nullptr) {
        const ArkUI_AttributeItem* selectionItem = nativeNodeApi->getAttribute(
            node->getArkUINodeHandle(),
            isTextArea ? NODE_TEXT_AREA_TEXT_SELECTION : NODE_TEXT_INPUT_TEXT_SELECTION);
        if (selectionItem != nullptr && selectionItem->value != nullptr && selectionItem->size >= 2) {
            startPosition = selectionItem->value[0].i32;
            endPosition = selectionItem->value[1].i32;
        }

        const ArkUI_AttributeItem* caretItem = nativeNodeApi->getAttribute(
            node->getArkUINodeHandle(),
            isTextArea ? NODE_TEXT_AREA_CARET_OFFSET : NODE_TEXT_INPUT_CARET_OFFSET);
        if (caretItem != nullptr && caretItem->value != nullptr && caretItem->size >= 3) {
            caretX = caretItem->value[1].f32;
            caretY = caretItem->value[2].f32;
        }
    }

    facebook::react::KeyboardControllerViewEventEmitter::InputSectionEvent event;
    event.target = input->getTag();
    event.selection.start.x = length > 0 ? 0 : caretX / scale;
    event.selection.start.y = caretY / scale;
    event.selection.start.position = startPosition;
    event.selection.end.x = caretX / scale;
    event.selection.end.y = caretY / scale;
    event.selection.end.position = endPosition;
    m_eventEmitter->onFocusedInputSelectionChanged(event);
}

void KeyboardControllerViewComponentInstance::onChange(std::string text) {
    facebook::react::KeyboardControllerViewEventEmitter::TextChangeEvent event = {text};
    if(this->enabled){
        this->emitFocusedInputLayout(this->findFocusedInput());
        m_eventEmitter->onFocusedInputTextChanged(event);
     }

}

void KeyboardControllerViewComponentInstance::onChange(std::string text, std::string extendStr) {
    onChange(std::move(text));
}

void KeyboardControllerViewComponentInstance::onContentSizeChange(ArkUINode* node, float width, float height) {
    auto input = this->findInputByNode(node);
    if (!input) {
        input = this->findFocusedInput();
    }
    this->emitFocusedInputLayout(input);
}

void KeyboardControllerViewComponentInstance::onTextSelectionChange(
    ArkUINode* node, int32_t location, int32_t length) {
    DLOG(INFO) << " onKeyboardControllerView onTextSelectionChange with node";
    this->skipNextDeprecatedSelectionEvent = true;
    this->emitFocusedInputLayout(this->findInputByNode(node));
    this->emitFocusedInputSelection(node, location, length);
}

void KeyboardControllerViewComponentInstance::onTextSelectionChange(int32_t location, int32_t length) {
    DLOG(INFO) << " onKeyboardControllerView onTextSelectionChange";
    if (this->skipNextDeprecatedSelectionEvent) {
        this->skipNextDeprecatedSelectionEvent = false;
        return;
    }

    auto input = this->findFocusedInput();
    if (!input) {
        return;
    }

    this->emitFocusedInputLayout(input);
    this->emitFocusedInputSelection(&input->getLocalRootArkUINode(), location, length);
};

void KeyboardControllerViewComponentInstance::focusDidSet() {
    int currentIndex = -1;
    int target = -1;
    this->textInputVector = ViewHierarchyNavigator::getAllInputFields(this->shared_from_this());
    int count = static_cast<int>(this->textInputVector.size());
    for (size_t i = 0; i < this->textInputVector.size(); ++i) {
        auto& input = this->textInputVector[i];
        ArkUINode& node = input->getLocalRootArkUINode();
        if (node.isFocused()) {
           currentIndex = static_cast<int>(i);
           target = input->getTag();
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
               ("count", count)
               ("target", target);
           rnInstancePtr->postMessageToArkTS("focusDidSet", payload);
       }
   }
   this->emitFocusedInputLayout(this->findFocusedInput());
}

void KeyboardControllerViewComponentInstance::onFocus() {
    DLOG(INFO) << "onKeyboardControllerView onFocus";
    this->focusDidSet();
}

void KeyboardControllerViewComponentInstance::onBlur() {
    DLOG(INFO) << " onKeyboardControllerView onBlur";
    this->currentResponder = nullptr;
    this->emitNoFocusedInputLayout();
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
} // namespace rnoh
