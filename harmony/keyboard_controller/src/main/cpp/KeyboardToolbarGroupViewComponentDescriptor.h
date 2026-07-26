/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 */

#ifndef KEYBOARDTOOLBARGROUPVIEWCOMPONENTDESCRIPTOR_H
#define KEYBOARDTOOLBARGROUPVIEWCOMPONENTDESCRIPTOR_H

#pragma once

#include <react/renderer/components/view/ConcreteViewShadowNode.h>
#include <react/renderer/components/view/ViewProps.h>
#include <react/renderer/core/ConcreteComponentDescriptor.h>

namespace facebook {
namespace react {

inline const char KeyboardToolbarGroupViewComponentName[] =
    "KeyboardToolbarGroupView";

class KeyboardToolbarGroupViewProps : public ViewProps {
public:
    KeyboardToolbarGroupViewProps() = default;
    KeyboardToolbarGroupViewProps(
        const PropsParserContext &context,
        const KeyboardToolbarGroupViewProps &sourceProps,
        const RawProps &rawProps)
        : ViewProps(context, sourceProps, rawProps) {}
};

using KeyboardToolbarGroupViewShadowNode = ConcreteViewShadowNode<
    KeyboardToolbarGroupViewComponentName,
    KeyboardToolbarGroupViewProps,
    ViewEventEmitter>;

class KeyboardToolbarGroupViewComponentDescriptor final
    : public ConcreteComponentDescriptor<KeyboardToolbarGroupViewShadowNode> {
public:
    KeyboardToolbarGroupViewComponentDescriptor(
        ComponentDescriptorParameters const &parameters)
        : ConcreteComponentDescriptor(parameters) {}
};

} // namespace react
} // namespace facebook

#endif
