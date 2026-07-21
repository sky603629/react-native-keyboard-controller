/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 */

#include "KeyboardToolbarGroupViewComponentInstance.h"

#include <glog/logging.h>

namespace rnoh {

KeyboardToolbarGroupViewComponentInstance::KeyboardToolbarGroupViewComponentInstance(
    Context context)
    : CppComponentInstance(std::move(context)) {
    m_stackNode.setHitTestMode(ARKUI_HIT_TEST_MODE_DEFAULT);
    DLOG(INFO) << "KeyboardToolbarGroupViewComponentInstance created";
}

void KeyboardToolbarGroupViewComponentInstance::onChildInserted(
    ComponentInstance::Shared const &childComponentInstance,
    std::size_t index) {
    CppComponentInstance::onChildInserted(childComponentInstance, index);
    m_stackNode.insertChild(childComponentInstance->getLocalRootArkUINode(), index);
}

void KeyboardToolbarGroupViewComponentInstance::onChildRemoved(
    ComponentInstance::Shared const &childComponentInstance) {
    CppComponentInstance::onChildRemoved(childComponentInstance);
    m_stackNode.removeChild(childComponentInstance->getLocalRootArkUINode());
}

void KeyboardToolbarGroupViewComponentInstance::onPropsChanged(
    SharedConcreteProps const &props) {
    CppComponentInstance::onPropsChanged(props);
}

ArkUINode &KeyboardToolbarGroupViewComponentInstance::getLocalRootArkUINode() {
    return m_stackNode;
}

} // namespace rnoh
