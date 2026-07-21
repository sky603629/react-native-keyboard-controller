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
 * ViewHierarchyNavigator - focus traversal for Toolbar Prev/Next.
 * Aligns with upstream Android/iOS: when focus is inside KeyboardToolbarGroupView,
 * scanning does not leave the group (sibling groups / outside inputs are skipped).
 */
class ViewHierarchyNavigator {
public:
    static TextInputComponentInstance::Shared setFocusTo(
        const std::string& direction,
        ComponentInstance::Shared currentFocus
    );

    static std::vector<TextInputComponentInstance::Shared> getAllInputFields(
        ComponentInstance::Shared rootComponent
    );

    /** Closest KeyboardToolbarGroupView ancestor, or nullptr. */
    static ComponentInstance::Shared findGroupAncestor(
        ComponentInstance::Shared component
    );

    static bool isToolbarGroupComponent(ComponentInstance::Shared component);

private:
    static TextInputComponentInstance::Shared findTextInputInDirection(
        ComponentInstance::Shared currentFocus,
        const std::string& direction,
        ComponentInstance::Shared groupBoundary
    );

    static TextInputComponentInstance::Shared findTextInputInHierarchy(
        ComponentInstance::Shared component,
        const std::string& direction
    );

    static TextInputComponentInstance::Shared findTextInputOrGoDeeper(
        ComponentInstance::Shared child,
        const std::string& direction
    );

    static TextInputComponentInstance::Shared isValidTextInput(
        ComponentInstance::Shared component
    );

    static void collectInputFields(
        ComponentInstance::Shared component,
        std::vector<TextInputComponentInstance::Shared>& out,
        bool stopAtNestedGroups
    );
};

} // namespace rnoh
#endif // VIEW_HIERARCHY_NAVIGATOR_H
