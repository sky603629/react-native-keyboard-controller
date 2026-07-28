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


#include "RNKeyboardController.h"

#include <ReactCommon/TurboModuleUtils.h>
#include <react/renderer/dom/DOM.h>
#include <react/renderer/uimanager/UIManagerBinding.h>

#include <utility>

namespace rnoh {
using namespace facebook;
KeyboardController::KeyboardController(const ArkTSTurboModule::Context ctx, const std::string name) : ArkTSTurboModule(ctx, name)
{
    methodMap_ = {
        ARK_METHOD_METADATA(getConstants, 0),
        ARK_METHOD_METADATA(setInputMode, 1),
        ARK_METHOD_METADATA(setDefaultMode, 0),
        ARK_METHOD_METADATA(preload, 0),
        ARK_METHOD_METADATA(dismiss, 2),
        ARK_METHOD_METADATA(setFocusTo, 1),
        ARK_METHOD_METADATA(addListener, 1),
        ARK_METHOD_METADATA(removeListeners, 1),
    };

    methodMap_["viewPositionInWindow"] = MethodMetadata{
        1,
        [](jsi::Runtime& runtime,
           react::TurboModule&,
           const jsi::Value* args,
           size_t count) -> jsi::Value {
            if (count < 1 || !args[0].isNumber()) {
                return react::createPromiseAsJSIValue(
                    runtime,
                    [](jsi::Runtime&, std::shared_ptr<react::Promise> promise) {
                        promise->reject("viewPositionInWindow requires a numeric view tag");
                    });
            }

            const auto tag = static_cast<react::Tag>(args[0].asNumber());
            return react::createPromiseAsJSIValue(
                runtime,
                [tag](jsi::Runtime& promiseRuntime, std::shared_ptr<react::Promise> promise) {
                    auto binding = react::UIManagerBinding::getBinding(promiseRuntime);
                    if (!binding) {
                        promise->reject("UIManagerBinding is unavailable");
                        return;
                    }

                    auto& uiManager = binding->getUIManager();
                    auto shadowNode = uiManager.findShadowNodeByTag_DEPRECATED(tag);
                    if (!shadowNode) {
                        promise->reject("Could not find ShadowNode for tag");
                        return;
                    }

                    auto revisionProvider = uiManager.getShadowTreeRevisionProvider();
                    auto currentRevision = revisionProvider == nullptr
                        ? nullptr
                        : revisionProvider->getCurrentRevision(shadowNode->getSurfaceId());
                    if (!currentRevision) {
                        promise->reject("Could not find current ShadowTree revision");
                        return;
                    }

                    const auto rect = react::dom::measureInWindow(currentRevision, *shadowNode);
                    jsi::Object result(promiseRuntime);
                    result.setProperty(promiseRuntime, "x", rect.x);
                    result.setProperty(promiseRuntime, "y", rect.y);
                    result.setProperty(promiseRuntime, "width", rect.width);
                    result.setProperty(promiseRuntime, "height", rect.height);
                    promise->resolve(jsi::Value(std::move(result)));
                });
        }};
}

} // namespace rnoh
