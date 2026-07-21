/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 */

#ifndef VIEW_HIERARCHY_NAVIGATOR_H
#define VIEW_HIERARCHY_NAVIGATOR_H

#include <string>
#include <vector>
#include <memory>
#include "RNOH/ComponentInstance.h"
#include "RNOHCorePackage/ComponentInstances/TextInputComponentInstance.h"

namespace rnoh {

/**
 * Focus traversal for Toolbar Prev/Next (upstream Android-aligned).
 */
class ViewHierarchyNavigator {
public:
    static TextInputComponentInstance::Shared setFocusTo(
        const std::string& direction,
        ComponentInstance::Shared currentFocus);

    static std::vector<TextInputComponentInstance::Shared> getAllInputFields(
        ComponentInstance::Shared rootComponent);

    static ComponentInstance::Shared findGroupAncestor(
        ComponentInstance::Shared component);

    static bool isToolbarGroupComponent(ComponentInstance::Shared component);

private:
    static TextInputComponentInstance::Shared findTextInputInDirection(
        ComponentInstance::Shared currentFocus,
        int direction);

    static TextInputComponentInstance::Shared findTextInputInHierarchy(
        ComponentInstance::Shared component,
        int direction);

    static TextInputComponentInstance::Shared findTextInputOrGoDeeper(
        ComponentInstance::Shared child,
        int direction);

    static TextInputComponentInstance::Shared isValidTextInput(
        ComponentInstance::Shared component);

    static void collectInputFields(
        ComponentInstance::Shared component,
        std::vector<TextInputComponentInstance::Shared>& out,
        bool skipGroups);
};

} // namespace rnoh
#endif
