/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 */

#include "KeyboardToolbarGroupViewComponentInstance.h"

#include <glog/logging.h>
#include <hilog/log.h>
#define KC_GROUP_DOMAIN 0xD001C00
#define KC_GROUP_TAG "KC_GROUP"
#define KC_GROUP_LOG(fmt, ...) OH_LOG_Print(LOG_APP, LOG_INFO, KC_GROUP_DOMAIN, KC_GROUP_TAG, fmt, ##__VA_ARGS__)


namespace rnoh {

KeyboardToolbarGroupViewComponentInstance::KeyboardToolbarGroupViewComponentInstance(
    Context context)
    : CppComponentInstance(std::move(context)) {
    m_stackNode.setHitTestMode(ARKUI_HIT_TEST_MODE_DEFAULT);
    DLOG(INFO) << "KeyboardToolbarGroupViewComponentInstance created";
    KC_GROUP_LOG("GroupView CREATE tag=%{public}d", static_cast<int>(getTag()));
}

void KeyboardToolbarGroupViewComponentInstance::onChildInserted(
    ComponentInstance::Shared const &childComponentInstance,
    std::size_t index) {
    CppComponentInstance::onChildInserted(childComponentInstance, index);
    m_stackNode.insertChild(childComponentInstance->getLocalRootArkUINode(), index);
    KC_GROUP_LOG("GroupView childInserted parentTag=%{public}d childName=%{public}s childTag=%{public}d idx=%{public}zu",
        static_cast<int>(getTag()),
        childComponentInstance->getComponentName().c_str(),
        static_cast<int>(childComponentInstance->getTag()),
        index);
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
