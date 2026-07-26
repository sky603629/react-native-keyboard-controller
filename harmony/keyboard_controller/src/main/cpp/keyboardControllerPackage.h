/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 */
/**
 * MIT License
 *
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
