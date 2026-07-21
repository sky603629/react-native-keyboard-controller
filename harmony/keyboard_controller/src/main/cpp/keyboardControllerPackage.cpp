/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 */

#include "keyboardControllerPackage.h"
#include "KeyboardControllerViewJSIBinder.h"
#include "KeyboardGestureAreaJSIBinder.h"
#include "KeyboardToolbarGroupViewJSIBinder.h"
#include "KeyboardToolbarGroupViewComponentDescriptor.h"
#include "KeyboardToolbarGroupViewComponentInstance.h"
#include "KeyboardControllerViewComponentDescriptor.h"
#include "KeyboardGestureAreaComponentDescriptor.h"

using namespace facebook;
using namespace rnoh;

std::vector<facebook::react::ComponentDescriptorProvider>
KeyboardControllerPackage::createComponentDescriptorProviders() {
    return {facebook::react::concreteComponentDescriptorProvider<
                facebook::react::KeyboardControllerViewComponentDescriptor>(),
            facebook::react::concreteComponentDescriptorProvider<
                facebook::react::KeyboardGestureAreaComponentDescriptor>(),
            facebook::react::concreteComponentDescriptorProvider<
                facebook::react::KeyboardToolbarGroupViewComponentDescriptor>()};
}
ComponentJSIBinderByString KeyboardControllerPackage::createComponentJSIBinderByName() {
    return {{"RNKeyboardControllerView", std::make_shared<KeyboardControllerViewJSIBinder>()},
            {"RNKeyboardGestureArea", std::make_shared<KeyboardGestureAreaJSIBinder>()},
            {"KeyboardToolbarGroupView", std::make_shared<KeyboardToolbarGroupViewJSIBinder>()}};
}

std::unique_ptr<TurboModuleFactoryDelegate> KeyboardControllerPackage::createTurboModuleFactoryDelegate() {
    return std::make_unique<KeyboardControllerTurboModuleFactoryDelegate>();
}

ComponentInstanceFactoryDelegate::Shared KeyboardControllerPackage::createComponentInstanceFactoryDelegate() {
    return std::make_shared<KeyboardControllerComponentInstanceFactoryDelegate>();
};

ComponentInstance::Shared KeyboardControllerPackage::createComponentInstance(const ComponentInstance::Context &ctx) {
    if (ctx.componentName == "RNKeyboardControllerView") {
        return std::make_shared<KeyboardControllerViewComponentInstance>(ctx);
    }
    if (ctx.componentName == "RNKeyboardGestureArea") {
        return std::make_shared<KeyboardGestureAreaComponentInstance>(ctx);
    }
    if (ctx.componentName == "KeyboardToolbarGroupView") {
        return std::make_shared<KeyboardToolbarGroupViewComponentInstance>(ctx);
    }
    return nullptr;
};
