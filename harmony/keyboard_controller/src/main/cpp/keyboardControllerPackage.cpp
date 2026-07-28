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
 
#include "keyboardControllerPackage.h"
#include "KeyboardControllerViewJSIBinder.h"
#include "KeyboardGestureAreaJSIBinder.h"
#include "KeyboardToolbarGroupViewJSIBinder.h"


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
