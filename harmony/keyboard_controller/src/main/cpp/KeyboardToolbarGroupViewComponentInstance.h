/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 */

#ifndef KEYBOARDTOOLBARGROUPVIEWCOMPONENTINSTANCE_H
#define KEYBOARDTOOLBARGROUPVIEWCOMPONENTINSTANCE_H

#pragma once

#include "KeyboardToolbarGroupViewComponentDescriptor.h"
#include "RNOH/CppComponentInstance.h"
#include "RNOH/arkui/StackNode.h"

namespace rnoh {

/**
 * Marker host for KeyboardToolbar.Group.
 * Children are normal TextInputs; ViewHierarchyNavigator stops at this
 * component name when scanning for next/prev inputs outside the group.
 */
class KeyboardToolbarGroupViewComponentInstance
    : public CppComponentInstance<facebook::react::KeyboardToolbarGroupViewShadowNode> {
public:
    explicit KeyboardToolbarGroupViewComponentInstance(Context context);

    void onChildInserted(ComponentInstance::Shared const &childComponentInstance, std::size_t index) override;
    void onChildRemoved(ComponentInstance::Shared const &childComponentInstance) override;
    void onPropsChanged(SharedConcreteProps const &props) override;
    ArkUINode &getLocalRootArkUINode() override;

private:
    StackNode m_stackNode;
};

} // namespace rnoh

#endif
