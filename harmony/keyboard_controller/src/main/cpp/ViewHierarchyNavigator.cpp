/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 */

#include "ViewHierarchyNavigator.h"
#include <glog/logging.h>

#include <hilog/log.h>
#ifndef KC_GROUP_LOG
#define KC_GROUP_DOMAIN 0xD001C00
#define KC_GROUP_TAG "KC_GROUP"
#define KC_GROUP_LOG(fmt, ...) \
    OH_LOG_Print(LOG_APP, LOG_INFO, KC_GROUP_DOMAIN, KC_GROUP_TAG, fmt, ##__VA_ARGS__)
#endif


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
    bool isGroup = name == kToolbarGroupName ||
           name.find("KeyboardToolbarGroup") != std::string::npos;
    return isGroup;
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
    const std::string& direction,
    ComponentInstance::Shared currentFocus) {
    if (!currentFocus) {
        return nullptr;
    }
    int dir = (direction == "next") ? 1 : -1;
    DLOG(INFO) << "ViewHierarchyNavigator::setFocusTo dir=" << direction
               << " tag=" << currentFocus->getTag()
               << " inGroup=" << (findGroupAncestor(currentFocus) != nullptr);
    {
        auto g = findGroupAncestor(currentFocus);
        KC_GROUP_LOG("nav.setFocusTo dir=%{public}s tag=%{public}d inGroup=%{public}d name=%{public}s",
            direction.c_str(),
            static_cast<int>(currentFocus->getTag()),
            g != nullptr ? 1 : 0,
            currentFocus->getComponentName().c_str());
    }
    return findTextInputInDirection(currentFocus, dir);
}

void ViewHierarchyNavigator::collectInputFields(
    ComponentInstance::Shared component,
    std::vector<TextInputComponentInstance::Shared>& out,
    bool skipGroups) {
    if (!component) {
        return;
    }
    if (auto ti = isValidTextInput(component)) {
        out.push_back(ti);
        return;
    }
    // Global scan: Group is opaque — do not include its inputs.
    if (skipGroups && isToolbarGroupComponent(component)) {
        return;
    }
    for (const auto &child : component->getChildren()) {
        if (skipGroups && isToolbarGroupComponent(child)) {
            KC_GROUP_LOG("collect SKIP group child name=%{public}s tag=%{public}d",
                child->getComponentName().c_str(), static_cast<int>(child->getTag()));
            continue;
        }
        collectInputFields(child, out, skipGroups);
    }
}

std::vector<TextInputComponentInstance::Shared> ViewHierarchyNavigator::getAllInputFields(
    ComponentInstance::Shared rootComponent) {
    std::vector<TextInputComponentInstance::Shared> textInputs;
    if (!rootComponent) {
        return textInputs;
    }
    // Group as root: only inputs inside this group.
    if (isToolbarGroupComponent(rootComponent)) {
        for (const auto &child : rootComponent->getChildren()) {
            collectInputFields(child, textInputs, true);
        }
        DLOG(INFO) << "getAllInputFields(group) count=" << textInputs.size();
        KC_GROUP_LOG("getAllInputFields(GROUP) count=%{public}zu rootTag=%{public}d",
            textInputs.size(), static_cast<int>(rootComponent->getTag()));
        return textInputs;
    }
    // Global: skip groups entirely (ungrouped inputs only).
    collectInputFields(rootComponent, textInputs, true);
    DLOG(INFO) << "getAllInputFields(global) count=" << textInputs.size();
    KC_GROUP_LOG("getAllInputFields(GLOBAL) count=%{public}zu rootTag=%{public}d",
        textInputs.size(), rootComponent ? static_cast<int>(rootComponent->getTag()) : -1);
    return textInputs;
}

TextInputComponentInstance::Shared ViewHierarchyNavigator::findTextInputInDirection(
    ComponentInstance::Shared currentFocus,
    int direction) {
    if (!currentFocus) {
        return nullptr;
    }
    auto parentComponent = currentFocus->getParent().lock();
    if (!parentComponent) {
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

    if (direction > 0) {
        for (size_t i = currentIndex + 1; i < siblings.size(); ++i) {
            auto result = findTextInputOrGoDeeper(siblings[i], direction);
            if (result) {
                return result;
            }
        }
    } else {
        for (int i = currentIndex - 1; i >= 0; --i) {
            auto result = findTextInputOrGoDeeper(siblings[i], direction);
            if (result) {
                return result;
            }
        }
    }

    // Do not leave the group (upstream Android).
    if (isToolbarGroupComponent(parentComponent)) {
        DLOG(INFO) << "findTextInputInDirection: hit group boundary, stop";
        KC_GROUP_LOG("nav.boundary STOP dir=%{public}d parentIsGroup=1", direction);
        return nullptr;
    }

    return findTextInputInDirection(parentComponent, direction);
}

TextInputComponentInstance::Shared ViewHierarchyNavigator::findTextInputInHierarchy(
    ComponentInstance::Shared component,
    int direction) {
    if (!component) {
        return nullptr;
    }
    if (isToolbarGroupComponent(component)) {
        return nullptr;
    }
    const auto& children = component->getChildren();
    if (direction > 0) {
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
    ComponentInstance::Shared child,
    int direction) {
    if (!child) {
        return nullptr;
    }
    if (auto ti = isValidTextInput(child)) {
        return ti;
    }
    if (isToolbarGroupComponent(child)) {
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
        return std::dynamic_pointer_cast<TextInputComponentInstance>(component);
    }
    return nullptr;
}

} // namespace rnoh
