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
#include <optional>
#include "RNOH/CppComponentInstance.h"
#include "RNOH/TaskExecutor/TaskExecutor.h"
#include "KeyboardControllerViewComponentDescriptor.h"
#include "RNOHCorePackage/ComponentInstances/TextInputComponentInstance.h"
#include "RNOH/arkui/CustomNode.h"
#include "RNOH/ArkTSMessageHub.h"
#include "RNOH/arkui/TextInputNode.h"
#include "RNOH/arkui/TextAreaNode.h"
#include "ViewHierarchyNavigator.h"
#include "RNOH/arkui/NativeNodeApi.h"
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
    ~KeyboardControllerViewComponentInstance() override;
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
    bool enabled = false;
    bool statusBarTranslucent = false;
    bool navigationBarTranslucent = false;
    bool preserveEdgeToEdge = false;
    double keyboardHeight = 0;
    std::map<std::string, std::any> dictionary;
    int keyboardStatus = KeyboardControllerStatus::HIDE;
    std::vector<TextInputComponentInstance::Shared> textInputVector{};
    CustomNode m_customNode{};
    ComponentInstance::Shared currentResponder;
    void findTextInputComponents(ComponentInstance::Shared const &parentComponentInstance);
    void clearTextInputDelegates(ComponentInstance::Shared const &parentComponentInstance);
    void setWindowSystemBarEnable();
    void startKeyboardObserver();
    void closeKeyboardObserver();
    void keyboardHeightChangeHandle(double previousKeyboardHeight);
    void setWindowLayoutFullScreen();
    void setFocusTo(const std::string& direction);
    void focusDidSet();
    void syncUpLayout();
    void dispatchLayoutToJS(FocusedInputLayoutData const &event);
    void dispatchKeyboardFocusChangedIfNeeded();
    TextInputComponentInstance::Shared findFocusedTextInput();
    // Harmony does not expose selection start/end geometry; x/y are reported as 0.
    void dispatchSelectionToJS(int target, int32_t startPos, int32_t endPos);
    void postFocusedInputChanged();
    int findParentScrollViewTarget(ComponentInstance::Shared const &input);
    double pxToVp(double px) const;
    void updateFocusedInputLayoutObserver(TextInputComponentInstance::Shared const &focusedInput);
    void clearFocusedInputLayoutObserver();
    void handleFocusedInputLayoutEvent(ArkUI_NodeEvent *event);
    static void focusedInputLayoutEventReceiver(ArkUI_NodeEvent *event);
    void cancelKeyboardAnimation();
    void startKeyboardAnimation(double fromHeight, double toHeight, int target);
    void scheduleKeyboardAnimationFrame(
        uint64_t generation,
        double fromHeight,
        double toHeight,
        int target,
        int frame,
        int totalFrames);
    FocusedInputLayoutData m_lastLayoutEvent;
    int m_lastKeyboardEventTarget = -1;
    bool m_isHandlingKeyboardHeightChange = false;
    ArkUI_NodeHandle m_observedFocusedInputHandle = nullptr;
    std::optional<TaskExecutor::DelayedTask> m_keyboardAnimationTask;
    uint64_t m_keyboardAnimationGeneration = 0;
    void *high_lib_handle = NULL;
    ArkUI_ErrorCode (*focusRequestMethod)(ArkUI_NodeHandle node);
};
} // namespace rnoh

#endif

#endif
