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

#ifndef VIEW_HIERARCHY_NAVIGATOR_H
#define VIEW_HIERARCHY_NAVIGATOR_H

#include <string>
#include <vector>
#include <memory>
#include "RNOH/ComponentInstance.h"
#include "RNOHCorePackage/ComponentInstances/TextInputComponentInstance.h"

namespace rnoh {

/**
 * ViewHierarchyNavigator - 视图层级导航器
 * 
 * 通过遍历组件树来查找下一个/上一个输入框，
 * 实现逻辑与 iOS 版本保持一致。
 */
class ViewHierarchyNavigator {
public:
    /**
     * 设置焦点到指定方向的输入框
     * @param direction 方向 "next" | "prev"
     * @param currentFocus 当前焦点的组件实例
     * @return 找到的目标输入框，如果没有则返回nullptr
     */
    static TextInputComponentInstance::Shared setFocusTo(
        const std::string& direction,
        ComponentInstance::Shared currentFocus
    );

    /**
     * 获取指定组件树下的所有输入框
     * @param rootComponent 根组件
     * @return 所有输入框的列表
     */
    static std::vector<TextInputComponentInstance::Shared> getAllInputFields(
        ComponentInstance::Shared rootComponent
    );

    /**
     * 查找距离当前组件最近的 KeyboardToolbar.Group 祖先
     * @param component 当前组件
     * @return 最近的 Group 祖先，不在 Group 内时返回 nullptr
     */
    static ComponentInstance::Shared findGroupAncestor(
        ComponentInstance::Shared component
    );

private:
    static bool isToolbarGroupComponent(
        ComponentInstance::Shared component
    );

    /**
     * 根据方向查找输入框
     * @param currentFocus 当前焦点组件
     * @param direction "next" 或 "prev"
     * @return 找到的输入框，如果没有则返回nullptr
     */
    static TextInputComponentInstance::Shared findTextInputInDirection(
        ComponentInstance::Shared currentFocus,
        int direction
    );

    /**
     * 在视图层级中查找输入框（递归向下）
     * @param component 要搜索的组件
     * @param direction 方向，影响遍历顺序
     * @return 找到的输入框，如果没有则返回nullptr
     */
    static TextInputComponentInstance::Shared findTextInputInHierarchy(
        ComponentInstance::Shared component,
        int direction
    );

    /**
     * 查找输入框或递归深入
     * @param child 子组件
     * @param direction 方向
     * @return 找到的输入框，如果没有则返回nullptr
     */
    static TextInputComponentInstance::Shared findTextInputOrGoDeeper(
        ComponentInstance::Shared child,
        int direction
    );

    /**
     * 检查组件是否是有效的输入框
     * @param component 要检查的组件
     * @return 如果是有效输入框则返回转换后的指针，否则返回nullptr
     */
    static TextInputComponentInstance::Shared isValidTextInput(
        ComponentInstance::Shared component
    );

    static void collectInputFields(
        ComponentInstance::Shared component,
        std::vector<TextInputComponentInstance::Shared>& out,
        bool skipGroups
    );
};

} // namespace rnoh
#endif // VIEW_HIERARCHY_NAVIGATOR_H
