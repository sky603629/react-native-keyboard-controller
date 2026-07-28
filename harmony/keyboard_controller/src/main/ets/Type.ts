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

export  enum  KeyboardControllerEventName {
  KEYBOARD_WILL_SHOW="KeyboardController::keyboardWillShow",
  KEYBOARD_DID_SHOW="KeyboardController::keyboardDidShow",
  KEYBOARD_WILL_HIDE="KeyboardController::keyboardWillHide",
  KEYBOARD_DID_HIDE="KeyboardController::keyboardDidHide",
  FOCUS_DID_SET="KeyboardController::focusDidSet",
  LAYOUT_DID_SYNCHRONIZE="KeyboardController::layoutDidSynchronize"
}
export type KeyboardChangeResult = {
  code:number;
  message:string;
  data:number | undefined
}
export  type KeyboardEvent ={
     height:number;
     duration:number;
    timestamp:number;
    target:number;
}
export  enum KeyboardStatusType {
  HIDE = 0,
  SHOW = 1
};
export  type RawTouchPoint={
  pointerId: number;
  windowX: number;
  windowY: number;
}
export  type RawTouchEvent = {
  action: number;
  actionTouch: RawTouchPoint;
  touchPoints: RawTouchPoint[];
  sourceType: number;
  timestamp: number;
  rootTag:number;
};
