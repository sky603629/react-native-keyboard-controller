/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 */

#ifndef KEYBOARDTOOLBARGROUPVIEWJSIBINDER_H
#define KEYBOARDTOOLBARGROUPVIEWJSIBINDER_H

#pragma once
#include "RNOHCorePackage/ComponentBinders/ViewComponentJSIBinder.h"

namespace rnoh {
// Marker view only — no custom props; used to scope Prev/Next focus traversal.
class KeyboardToolbarGroupViewJSIBinder : public ViewComponentJSIBinder {
protected:
    facebook::jsi::Object createNativeProps(facebook::jsi::Runtime &rt) override
    {
        return ViewComponentJSIBinder::createNativeProps(rt);
    }
};
} // namespace rnoh

#endif
