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

#ifndef KEYBOARDCONTROLLERVIEWCOMPONENTINSTANCE_H
#define KEYBOARDCONTROLLERVIEWCOMPONENTINSTANCE_H

#ifndef TESTER_HARMONY_KEYBOARD_CONTROLLER_SRC_MAIN_CPP_KEYBOARDCONTROLLERVIEWCOMPONENTINSTANCE_H
#define TESTER_HARMONY_KEYBOARD_CONTROLLER_SRC_MAIN_CPP_KEYBOARDCONTROLLERVIEWCOMPONENTINSTANCE_H

#include <any>
#include "RNOH/CppComponentInstance.h"
#include "KeyboardControllerViewComponentDescriptor.h"
#include "RNOHCorePackage/ComponentInstances/TextInputComponentInstance.h"
#include "RNOH/arkui/CustomNode.h"
#include "RNOH/ArkTSMessageHub.h"
#include "RNOH/arkui/TextInputNode.h"
#include "RNOH/arkui/TextAreaNode.h"
#include "ViewHierarchyNavigator.h"
#include <arkui/native_node.h>
#include <arkui/native_type.h>

namespace rnoh {
struct FocusedInputLayoutData {
    int target = -1;
    int parentScrollViewTarget = -1;
    double absoluteX = 0;
    double absoluteY = 0;
    double x = 0;
    double y = 0;
    double width = 0;
    double height = 0;

    bool operator==(FocusedInputLayoutData const &other) const {
        return target == other.target &&
               parentScrollViewTarget == other.parentScrollViewTarget &&
               absoluteX == other.absoluteX &&
               absoluteY == other.absoluteY &&
               x == other.x &&
               y == other.y &&
               width == other.width &&
               height == other.height;
    }
};

enum KeyboardControllerStatus { HIDE = 0, SHOW = 1 };
class KeyboardControllerViewComponentInstance
    : public CppComponentInstance<facebook::react::KeyboardControllerViewShadowNode>,
      public CustomNodeDelegate,
      public TextAreaNodeDelegate,
      public TextInputNodeDelegate,
      public ArkTSMessageHub::Observer {
public:
    KeyboardControllerViewComponentInstance(Context context);
    void onKeyboardMoveStart();
    void onKeyboardMove();
    void onKeyboardMoveEnd();
    void onKeyboardMoveInteractive();
    void onFocusedInputLayoutChanged();
    void onFocusedInputTextChanged();
    void onFocusedInputSelectionChanged();

    void onChildInserted(ComponentInstance::Shared const &childComponentInstance, std::size_t index) override;
    void onChildRemoved(ComponentInstance::Shared const &childComponentInstance) override;
    void onPropsChanged(SharedConcreteProps const &props) override;
    void onCommandReceived(std::string const &commandName, folly::dynamic const &args) override;
    void onMessageReceived(ArkTSMessage const &message) override;
    ArkUINode &getLocalRootArkUINode() override;
    void onClick() override;
    // textArea textInput delegate
    void onChange(std::string text) override;
    void onChange(std::string text, std::string extendStr) override;
    void onBlur() override;
    void onFocus() override;
    void onTextSelectionChange(int32_t location, int32_t length) override;

protected:
private:
    bool enabled;
    bool statusBarTranslucent;
    bool navigationBarTranslucent;
    bool preserveEdgeToEdge;
    double keyboardHeight;
    std::map<std::string, std::any> dictionary;
    int keyboardStatus;
    std::vector<TextInputComponentInstance::Shared> textInputVector{};
    CustomNode m_customNode{};
    ComponentInstance::Shared currentResponder;
    void findTextInputComponents(ComponentInstance::Shared const &parentComponentInstance);
    void setWindowSystemBarEnable();
    void startKeyboardObserver();
    void closeKeyboardObserver();
    void keyboardHeightChangeHandle();
    void setWindowLayoutFullScreen();
    void setFocusTo(const std::string& direction);
    void focusDidSet();
    void syncUpLayout();
    void dispatchLayoutToJS(FocusedInputLayoutData const &event);
    TextInputComponentInstance::Shared findFocusedTextInput();
    // Emit onFocusedInputSelectionChanged; x/y default 0 when caret geometry unavailable
    void dispatchSelectionToJS(int target, int32_t startPos, int32_t endPos);
    // Push focused TextInput {target, type} to ArkTS for KeyboardEvents will/did payload
    void postFocusedInputChanged();
    int findParentScrollViewTarget(ComponentInstance::Shared const &input);
    double pxToVp(double px) const;
    void updateFocusedInputLayoutObserver(TextInputComponentInstance::Shared const &focusedInput);
    void clearFocusedInputLayoutObserver();
    void handleFocusedInputLayoutEvent(ArkUI_NodeEvent *event);
    static void focusedInputLayoutEventReceiver(ArkUI_NodeEvent *event);
    FocusedInputLayoutData m_lastLayoutEvent;
    ArkUI_NodeHandle m_observedFocusedInputHandle = nullptr;
    void *high_lib_handle = NULL;
    ArkUI_ErrorCode (*focusRequestMethod)(ArkUI_NodeHandle node);
};
} // namespace rnoh

#endif

#endif
