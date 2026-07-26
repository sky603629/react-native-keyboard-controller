/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 */
/**
 * MIT License
 *
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#include "ViewHierarchyNavigator.h"

namespace rnoh {

namespace {
const char *kToolbarGroupName = "KeyboardToolbarGroupView";
}

bool ViewHierarchyNavigator::isToolbarGroupComponent(
    ComponentInstance::Shared component) {
    return component && component->getComponentName() == kToolbarGroupName;
}

ComponentInstance::Shared ViewHierarchyNavigator::findGroupAncestor(
    ComponentInstance::Shared component) {
    if (!component) {
        return nullptr;
    }
    auto parent = component->getParent().lock();
    while (parent) {
        if (isToolbarGroupComponent(parent)) {
            return parent;
        }
        parent = parent->getParent().lock();
    }
    return nullptr;
}

TextInputComponentInstance::Shared ViewHierarchyNavigator::setFocusTo(
    const std::string& direction, ComponentInstance::Shared currentFocus) {
    if (!currentFocus) {
        return nullptr;
    }
    return findTextInputInDirection(
        currentFocus,
        direction == "next" ? 1 : -1);
}

void ViewHierarchyNavigator::collectInputFields(
    ComponentInstance::Shared component,
    std::vector<TextInputComponentInstance::Shared>& out,
    bool skipGroups) {
    if (!component) {
        return;
    }
    if (auto textInput = isValidTextInput(component)) {
        out.push_back(textInput);
        return;
    }
    if (skipGroups && isToolbarGroupComponent(component)) {
        return;
    }
    for (const auto& child : component->getChildren()) {
        collectInputFields(child, out, skipGroups);
    }
}

std::vector<TextInputComponentInstance::Shared> ViewHierarchyNavigator::getAllInputFields(
    ComponentInstance::Shared rootComponent) {
    std::vector<TextInputComponentInstance::Shared> textInputs;
    if (!rootComponent) {
        return textInputs;
    }
    if (isToolbarGroupComponent(rootComponent)) {
        for (const auto& child : rootComponent->getChildren()) {
            collectInputFields(child, textInputs, true);
        }
    } else {
        collectInputFields(rootComponent, textInputs, true);
    }
    return textInputs;
}

TextInputComponentInstance::Shared ViewHierarchyNavigator::findTextInputInDirection(
    ComponentInstance::Shared currentFocus, int direction) {
    if (!currentFocus) {
        return nullptr;
    }
    // 获取父组件
    auto parentComponent = currentFocus->getParent().lock();
    if (!parentComponent) {
        return nullptr;
    }
    // 获取父组件的所有子组件
    const auto& siblings = parentComponent->getChildren();
    // 找到当前组件在父组件中的索引
    int currentIndex = -1;
    for (size_t i = 0; i < siblings.size(); ++i) {
        if (siblings[i].get() == currentFocus.get()) {
            currentIndex = static_cast<int>(i);
            break;
        }
    }
    if (currentIndex < 0) {
        return nullptr;
    }
    // 根据方向确定遍历范围
    if (direction > 0) {
        // 向后遍历：从 currentIndex+1 到末尾
        for (size_t i = currentIndex + 1; i < siblings.size(); ++i) {
            auto result = findTextInputOrGoDeeper(siblings[i], direction);
            if (result) {
                return result;
            }
        }
    } else {
        // 向前遍历：从 currentIndex-1 到 0（倒序）
        for (int i = currentIndex - 1; i >= 0; --i) {
            auto result = findTextInputOrGoDeeper(siblings[i], direction);
            if (result) {
                return result;
            }
        }
    }
    // Group 是导航边界，组内焦点不能跳转到组外输入框
    if (isToolbarGroupComponent(parentComponent)) {
        return nullptr;
    }
    // 如果同级没找到，递归到父级继续查找
    return findTextInputInDirection(parentComponent, direction);
}

TextInputComponentInstance::Shared ViewHierarchyNavigator::findTextInputInHierarchy(
    ComponentInstance::Shared component, int direction) {
    if (!component || isToolbarGroupComponent(component)) {
        return nullptr;
    }
    const auto& children = component->getChildren();
    if (direction > 0) {
        // 正序遍历
        for (const auto& child : children) {
            auto result = findTextInputOrGoDeeper(child, direction);
            if (result) {
                return result;
            }
        }
    } else {
        // 倒序遍历
        for (auto it = children.rbegin(); it != children.rend(); ++it) {
            auto result = findTextInputOrGoDeeper(*it, direction);
            if (result) {
                return result;
            }
        }
    }
    return nullptr;
}

TextInputComponentInstance::Shared ViewHierarchyNavigator::findTextInputOrGoDeeper(
    ComponentInstance::Shared child, int direction) {
    if (!child || isToolbarGroupComponent(child)) {
        return nullptr;
    }
    // 首先检查当前组件是否是有效输入框
    auto textInput = isValidTextInput(child);
    if (textInput) {
        return textInput;
    }
    // 如果不是输入框，递归查找其子组件
    return findTextInputInHierarchy(child, direction);
}

TextInputComponentInstance::Shared ViewHierarchyNavigator::isValidTextInput(
    ComponentInstance::Shared component) {
    if (!component) {
        return nullptr;
    }
    // 检查组件名称是否包含 TextInput 或 TextArea
    const std::string& name = component->getComponentName();
    if (name.find("TextInput") != std::string::npos || 
        name.find("TextArea") != std::string::npos) {
        // 尝试转换为 TextInputComponentInstance
        auto textInput = std::dynamic_pointer_cast<TextInputComponentInstance>(component);
        if (textInput) {
            return textInput;
        }
    }
    return nullptr;
}
} // namespace rnoh
