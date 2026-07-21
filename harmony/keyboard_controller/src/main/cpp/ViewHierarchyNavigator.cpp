/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 */

#include "ViewHierarchyNavigator.h"
#include <glog/logging.h>
#include <algorithm>

namespace rnoh {

namespace {
const char *kToolbarGroupName = "KeyboardToolbarGroupView";
}

bool ViewHierarchyNavigator::isToolbarGroupComponent(
    ComponentInstance::Shared component) {
    if (!component) {
        return false;
    }
    const std::string &name = component->getComponentName();
    return name == kToolbarGroupName ||
           name.find("KeyboardToolbarGroup") != std::string::npos;
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
        DLOG(INFO) << "ViewHierarchyNavigator::setFocusTo - currentFocus is null";
        return nullptr;
    }
    auto group = findGroupAncestor(currentFocus);
    DLOG(INFO) << "ViewHierarchyNavigator::setFocusTo direction=" << direction
               << " tag=" << currentFocus->getTag()
               << " inGroup=" << (group != nullptr);
    auto textInput = findTextInputInDirection(currentFocus, direction, group);
    if (textInput) {
        DLOG(INFO) << "ViewHierarchyNavigator::setFocusTo - found target";
    } else {
        DLOG(INFO) << "ViewHierarchyNavigator::setFocusTo - no target";
    }
    return textInput;
}

void ViewHierarchyNavigator::collectInputFields(
    ComponentInstance::Shared component,
    std::vector<TextInputComponentInstance::Shared>& out,
    bool stopAtNestedGroups) {
    if (!component) {
        return;
    }
    if (auto ti = isValidTextInput(component)) {
        out.push_back(ti);
        return;
    }
    // Do not descend into nested groups when scanning from outside a group.
    if (stopAtNestedGroups && isToolbarGroupComponent(component)) {
        return;
    }
    for (const auto &child : component->getChildren()) {
        // When root itself is a group, we still walk children but stop at nested groups.
        if (stopAtNestedGroups && isToolbarGroupComponent(child)) {
            continue;
        }
        collectInputFields(child, out, stopAtNestedGroups);
    }
}

std::vector<TextInputComponentInstance::Shared> ViewHierarchyNavigator::getAllInputFields(
    ComponentInstance::Shared rootComponent) {
    std::vector<TextInputComponentInstance::Shared> textInputs;
    if (!rootComponent) {
        return textInputs;
    }
    // If root is a group, collect only inside it (do not leave group).
    if (isToolbarGroupComponent(rootComponent)) {
        for (const auto &child : rootComponent->getChildren()) {
            collectInputFields(child, textInputs, true);
        }
        return textInputs;
    }
    collectInputFields(rootComponent, textInputs, true);
    return textInputs;
}

TextInputComponentInstance::Shared ViewHierarchyNavigator::findTextInputInDirection(
    ComponentInstance::Shared currentFocus,
    const std::string& direction,
    ComponentInstance::Shared groupBoundary) {
    if (!currentFocus) {
        return nullptr;
    }
    auto parentComponent = currentFocus->getParent().lock();
    if (!parentComponent) {
        return nullptr;
    }

    // Do not leave the group: if parent is outside groupBoundary, stop.
    // When groupBoundary is set, stop once we would climb above the group.
    if (groupBoundary && currentFocus.get() == groupBoundary.get()) {
        DLOG(INFO) << "ViewHierarchyNavigator - reached group boundary, stop";
        return nullptr;
    }

    const auto& siblings = parentComponent->getChildren();
    int currentIndex = -1;
    for (size_t i = 0; i < siblings.size(); ++i) {
        if (siblings[i].get() == currentFocus.get()) {
            currentIndex = static_cast<int>(i);
            break;
        }
    }
    if (currentIndex == -1) {
        return nullptr;
    }

    if (direction == "next") {
        for (size_t i = currentIndex + 1; i < siblings.size(); ++i) {
            // Skip other groups as opaque when outside, or nested groups
            if (isToolbarGroupComponent(siblings[i]) &&
                (!groupBoundary || siblings[i].get() != groupBoundary.get())) {
                // When inside a group, siblings shouldn't be other groups typically;
                // when outside, do not enter other groups for global next/prev of ungrouped focus.
                if (!groupBoundary) {
                    continue; // ungrouped focus: skip entire groups
                }
            }
            auto result = findTextInputOrGoDeeper(siblings[i], direction);
            if (result) {
                return result;
            }
        }
    } else {
        for (int i = currentIndex - 1; i >= 0; --i) {
            if (isToolbarGroupComponent(siblings[i]) && !groupBoundary) {
                continue;
            }
            auto result = findTextInputOrGoDeeper(siblings[i], direction);
            if (result) {
                return result;
            }
        }
    }

    // Climb: if parent is the group boundary, stop (do not leave group).
    if (groupBoundary && parentComponent.get() == groupBoundary.get()) {
        DLOG(INFO) << "ViewHierarchyNavigator - parent is group boundary, stop";
        return nullptr;
    }
    return findTextInputInDirection(parentComponent, direction, groupBoundary);
}

TextInputComponentInstance::Shared ViewHierarchyNavigator::findTextInputInHierarchy(
    ComponentInstance::Shared component, const std::string& direction) {
    if (!component) {
        return nullptr;
    }
    // Do not enter a group when searching "through" a sibling branch from outside.
    // (Grouped traversal always starts inside group and never leaves.)
    if (isToolbarGroupComponent(component)) {
        return nullptr;
    }
    const auto& children = component->getChildren();
    if (direction == "next") {
        for (const auto& child : children) {
            auto result = findTextInputOrGoDeeper(child, direction);
            if (result) {
                return result;
            }
        }
    } else {
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
    ComponentInstance::Shared child, const std::string& direction) {
    if (!child) {
        return nullptr;
    }
    auto textInput = isValidTextInput(child);
    if (textInput) {
        return textInput;
    }
    if (isToolbarGroupComponent(child)) {
        // Opaque for external traversal
        return nullptr;
    }
    return findTextInputInHierarchy(child, direction);
}

TextInputComponentInstance::Shared ViewHierarchyNavigator::isValidTextInput(
    ComponentInstance::Shared component) {
    if (!component) {
        return nullptr;
    }
    const std::string& name = component->getComponentName();
    if (name.find("TextInput") != std::string::npos ||
        name.find("TextArea") != std::string::npos) {
        auto textInput = std::dynamic_pointer_cast<TextInputComponentInstance>(component);
        if (textInput) {
            return textInput;
        }
    }
    return nullptr;
}

} // namespace rnoh
