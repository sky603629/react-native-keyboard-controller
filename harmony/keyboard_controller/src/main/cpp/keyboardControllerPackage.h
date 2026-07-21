/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 */

#ifndef TESTER_HARMONY_KEYBOARD_CONTROLLER_SRC_MAIN_CPP_KEYBOARDCONTROLLERPACKAGE_H
#define TESTER_HARMONY_KEYBOARD_CONTROLLER_SRC_MAIN_CPP_KEYBOARDCONTROLLERPACKAGE_H

#ifndef KEYBOARDCONTROLLERPACKAGE_H
#define KEYBOARDCONTROLLERPACKAGE_H

#include "KeyboardGestureAreaComponentInstance.h"
#include "KeyboardToolbarGroupViewComponentInstance.h"
#include "RNOH/Package.h"
#include "RNKeyboardController.h"
#include "RNStatusBarManagerCompat.h"
#include "KeyboardControllerViewComponentInstance.h"
namespace rnoh {
 class KeyboardControllerComponentInstanceFactoryDelegate : public ComponentInstanceFactoryDelegate {
 public:
        ComponentInstance::Shared create(ComponentInstance::Context ctx) override {
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
        }
    };
class KeyboardControllerTurboModuleFactoryDelegate : public TurboModuleFactoryDelegate {
public:
    SharedTurboModule createTurboModule(Context ctx, const std::string &name) const override {
        if (name == "KeyboardController") {
            return std::make_shared<KeyboardController>(ctx, name);
        }
          if (name == "StatusBarManagerCompat") {
            return std::make_shared<StatusBarManagerCompat>(ctx, name);
        }
        return nullptr;
    };
};
class KeyboardControllerPackage : public Package{
public:
    KeyboardControllerPackage(Package::Context ctx) : Package(ctx) {}
    std::unique_ptr<TurboModuleFactoryDelegate> createTurboModuleFactoryDelegate() override;
    std::vector<facebook::react::ComponentDescriptorProvider> createComponentDescriptorProviders() override;
    ComponentJSIBinderByString createComponentJSIBinderByName() override;
    ComponentInstance::Shared createComponentInstance(const ComponentInstance::Context &ctx) override;
    ComponentInstanceFactoryDelegate::Shared createComponentInstanceFactoryDelegate() override;
};
} // namespace rnoh

#endif

#endif
