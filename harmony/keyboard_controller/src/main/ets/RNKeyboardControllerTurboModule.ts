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

import { TurboModule } from "@rnoh/react-native-openharmony/ts";
import common from '@ohos.app.ability.common';
import window from '@ohos.window';
import inputMethod from '@ohos.inputMethod';
import Logger from './Logger';
import { KeyboardControllerEventName, KeyboardStatusType } from './Type';
import { BusinessError } from '@kit.BasicServicesKit';
import { JSON } from '@kit.ArkTS';
import { ConfigurationConstant } from '@kit.AbilityKit';

declare function px2vp(px: number): number;

interface RNKeyboardControllerSpec {
  getConstants(): {};

  setInputMode(mode: number): void;

  setDefaultMode(): void;

  dismiss(keepFocus: boolean): void;

  setFocusTo(direction: string): void;

  addListener(eventName: string, listener: (event) => void,): void;

  removeListeners(count: number): void;
}

export class RNKeyboardControllerTurboModule extends TurboModule implements RNKeyboardControllerSpec {
  private context: common.UIAbilityContext;
  private keyboardHeight: number = 0;
  private keyboardStatus: KeyboardStatusType = KeyboardStatusType.HIDE;
  private eventListeners: KeyboardControllerEventName[];
  private currentWindow:window.Window;
  private enabled:boolean = false;
  private nativeKeyboardWillEventEnabled:boolean = false;
  private keyboardHeightChangeCallback: ((data: number) => void) | undefined = undefined;
  private keyboardWillShowCallback: ((keyboardInfo: window.KeyboardInfo) => void) | undefined = undefined;
  private keyboardWillHideCallback: ((keyboardInfo: window.KeyboardInfo) => void) | undefined = undefined;
  private readonly keyboardAnimationDuration: number = 250;
  private currentKeyboardAnimationDuration: number = this.keyboardAnimationDuration;
  private focusedInputTarget: number = -1;
  private cleanUpCallbacks: (() => void)[] = [];
  constructor(ctx) {
    super(ctx);
    this.context = this.ctx.uiAbilityContext;
    this.eventListeners = this.supportListeners();

    // 订阅 C++ 层发来的 focusDidSet 消息
    this.cleanUpCallbacks.push(
      this.ctx.rnInstance.cppEventEmitter.subscribe("focusDidSet", (payload: { current: number, count: number, target?: number }) => {
        this.focusedInputTarget = payload.target ?? -1;
        Logger.info('###turboModule received focusDidSet from cpp', String(payload.current) + ',' + String(payload.count) + ',' + String(this.focusedInputTarget));
        if (this.eventListeners.includes(KeyboardControllerEventName.FOCUS_DID_SET)) {
          this.ctx.rnInstance.emitDeviceEvent(KeyboardControllerEventName.FOCUS_DID_SET, {
            current: payload.current,
            count: payload.count,
            target: this.focusedInputTarget
          });
        }
      })
    );
  }

  readonly getConstants: () => {};

  // set mode
  setInputMode(mode: number): void {
    Logger.info('harmonyOS not support setInputMode')
  }

  setDefaultMode(): void {
    Logger.info('harmonyOS not support setDefaultMode')
  }

  /**
   * @description 键盘隐藏
   * */
  dismiss(keepFocus: boolean): void {
    let inputMethodController = inputMethod.getController();
    inputMethodController.stopInputSession()

  }
  /**
   * @description 键盘隐藏
   * */
  private  show(): void {
    let inputMethodController = inputMethod.getController();
    inputMethodController.showSoftKeyboard()

  }

  setFocusTo(direction: string): void {
    this.ctx.rnInstance.postMessageToCpp('setFocusTo', direction);
  }

  /**
   * @description 添加监听键盘事件
   */
  addListener(eventName: KeyboardControllerEventName) {
    let supportEvents = this.supportListeners()
    let bo = supportEvents.includes(eventName)
    if (bo && !this.eventListeners.includes(eventName)) {
      this.eventListeners.push(eventName)
    }
  }

  /**
   * @description 删除监听事件
   * */
  removeListeners(count: number): void {
    let num = this.eventListeners.length;
    if (num>0) {
      this.eventListeners = [];
    }
  }

  private supportListeners() {
    return [
      KeyboardControllerEventName.KEYBOARD_WILL_SHOW,
      KeyboardControllerEventName.KEYBOARD_DID_SHOW,
      KeyboardControllerEventName.KEYBOARD_WILL_HIDE,
      KeyboardControllerEventName.KEYBOARD_DID_HIDE,
      KeyboardControllerEventName.FOCUS_DID_SET
    ];
  }

  private async setWindowSystemBarEnable(statusBarTranslucent: boolean, navigationBarTranslucent: boolean) {

    let windowInstance: window.Window | undefined = undefined;
    windowInstance = await window.getLastWindow(this.context);
    let systemBarProperty = windowInstance.getWindowSystemBarProperties();
    let newSystemBarProperty: window.SystemBarProperties = {
      ...systemBarProperty,
      statusBarColor: statusBarTranslucent ? "#00000000" : systemBarProperty.statusBarColor,
      navigationBarColor: navigationBarTranslucent ? "#00000000" : systemBarProperty.navigationBarColor
    };
    try {
      let promise = windowInstance.setWindowSystemBarProperties(newSystemBarProperty);
      promise.then(() => {
        Logger.info('Succeeded in setting the system bar properties.');
      }).catch((err: BusinessError) => {
        Logger.error(`Failed to set the system bar properties. Cause code: ${err.code}, message: ${err.message}`);
      });
    } catch (exception) {
      Logger.error(`Failed to set the system bar properties. Cause code: ${exception.code}, message: ${exception.message}`);
    }
  }

  private emitKeyboardEvent(eventName: KeyboardControllerEventName, height: number, duration: number = this.currentKeyboardAnimationDuration): void {
    if (!this.eventListeners.includes(eventName)) {
      return;
    }

    this.ctx.rnInstance.emitDeviceEvent(eventName, {
      duration: duration,
      timestamp: new Date().getTime() / 1000,
      target: this.focusedInputTarget,
      height: height,
      type: "default",
      appearance: this.getKeyboardAppearance()
    });
  }

  private keyboardControllerEventHandle(previousHeight: number, height: number, emitSyntheticWill: boolean = true) {
    Logger.info('###turboModule keyboardControllerEventHandle', String(previousHeight) + ',' + String(height));
    if(!this.enabled){
      return
    }

    const wasVisible = previousHeight > 0;
    const isVisible = height > 0;

    if (emitSyntheticWill && !wasVisible && isVisible) {
      Logger.info('###turboModule keyboardWillShow');
      this.emitKeyboardEvent(KeyboardControllerEventName.KEYBOARD_WILL_SHOW, height);
    } else if (emitSyntheticWill && wasVisible && !isVisible) {
      Logger.info('###turboModule keyboardWillHide');
      this.emitKeyboardEvent(KeyboardControllerEventName.KEYBOARD_WILL_HIDE, 0);
    }

    if (isVisible) {
      Logger.info('###turboModule keyboardDidShow');
      this.emitKeyboardEvent(KeyboardControllerEventName.KEYBOARD_DID_SHOW, height);
    } else if (wasVisible) {
      Logger.info('###turboModule keyboardDidHide');
      this.emitKeyboardEvent(KeyboardControllerEventName.KEYBOARD_DID_HIDE, 0);
    }
  }

  private getKeyboardHeightFromAvoidArea(fallbackHeight: number = 0): number {
    try {
      const keyboardAvoidArea = this.currentWindow?.getWindowAvoidArea(window.AvoidAreaType.TYPE_KEYBOARD).bottomRect;
      const height = Math.ceil(px2vp(keyboardAvoidArea.height));

      return height > 0 ? height : fallbackHeight;
    } catch (exception) {
      Logger.error('### Failed to get keyboard avoid area. Cause: ' + JSON.stringify(exception));
    }

    return fallbackHeight;
  }

  private getKeyboardHeightFromKeyboardInfo(keyboardInfo: window.KeyboardInfo, fallbackHeight: number = 0): number {
    const keyboardInfoPayload = keyboardInfo as unknown as Record<string, object>;
    const endRect = keyboardInfoPayload.endRect as Record<string, number> | undefined;
    const height = Math.ceil(px2vp(endRect?.height ?? 0));

    return height > 0 ? height : fallbackHeight;
  }

  private getKeyboardDurationFromKeyboardInfo(keyboardInfo: window.KeyboardInfo): number {
    const keyboardInfoPayload = keyboardInfo as unknown as Record<string, object>;
    const config = keyboardInfoPayload.config as Record<string, number> | undefined;
    const duration = config?.duration;

    return duration && duration > 0 ? duration : this.keyboardAnimationDuration;
  }

  private registerNativeKeyboardWillEvents(): boolean {
    if (!this.currentWindow) {
      return false;
    }

    this.unregisterNativeKeyboardWillEvents();

    this.keyboardWillShowCallback = (keyboardInfo: window.KeyboardInfo) => {
      if (!this.enabled) {
        return;
      }
      const height = this.getKeyboardHeightFromKeyboardInfo(
        keyboardInfo,
        this.getKeyboardHeightFromAvoidArea(this.keyboardHeight),
      );
      this.currentKeyboardAnimationDuration = this.getKeyboardDurationFromKeyboardInfo(keyboardInfo);
      Logger.info('###turboModule native keyboardWillShow', JSON.stringify(keyboardInfo));
      this.emitKeyboardEvent(KeyboardControllerEventName.KEYBOARD_WILL_SHOW, height, this.currentKeyboardAnimationDuration);
    };
    this.keyboardWillHideCallback = (keyboardInfo: window.KeyboardInfo) => {
      if (!this.enabled) {
        return;
      }
      this.currentKeyboardAnimationDuration = this.getKeyboardDurationFromKeyboardInfo(keyboardInfo);
      Logger.info('###turboModule native keyboardWillHide', JSON.stringify(keyboardInfo));
      this.emitKeyboardEvent(KeyboardControllerEventName.KEYBOARD_WILL_HIDE, 0, this.currentKeyboardAnimationDuration);
    };

    try {
      this.currentWindow.on('keyboardWillShow', this.keyboardWillShowCallback);
      this.currentWindow.on('keyboardWillHide', this.keyboardWillHideCallback);
      this.nativeKeyboardWillEventEnabled = true;
      Logger.info('###turboModule native keyboard will events enabled');
      return true;
    } catch (exception) {
      Logger.error('### Failed to enable native keyboard will events, fallback to keyboardHeightChange. Cause: ' + JSON.stringify(exception));
      this.unregisterNativeKeyboardWillEvents();
    }

    return false;
  }

  private unregisterNativeKeyboardWillEvents(): void {
    if (!this.currentWindow) {
      this.nativeKeyboardWillEventEnabled = false;
      return;
    }

    try {
      if (this.keyboardWillShowCallback) {
        this.currentWindow.off('keyboardWillShow', this.keyboardWillShowCallback);
      }
      if (this.keyboardWillHideCallback) {
        this.currentWindow.off('keyboardWillHide', this.keyboardWillHideCallback);
      }
    } catch (exception) {
      Logger.error('### Failed to close native keyboard will events. Cause: ' + JSON.stringify(exception));
    }

    this.keyboardWillShowCallback = undefined;
    this.keyboardWillHideCallback = undefined;
    this.nativeKeyboardWillEventEnabled = false;
  }

  private unregisterKeyboardHeightChangeEvent(): void {
    if (!this.currentWindow || !this.keyboardHeightChangeCallback) {
      return;
    }

    try {
      this.currentWindow.off('keyboardHeightChange', this.keyboardHeightChangeCallback);
      Logger.info('### close keyboardHeightChange observer');
    } catch (exception) {
      Logger.error('### Failed to close the listener for keyboard height changes. Cause: ' + JSON.stringify(exception));
    }

    this.keyboardHeightChangeCallback = undefined;
  }

  private async startKeyboardObserver(open:boolean) {
    Logger.info("###turboModule startKeyboardObserver",String(open));
    this.enabled=open;
    this.currentWindow = await window.getLastWindow(this.context);
    if (!this.currentWindow) {
      throw ('windowInstance is null')
      return
    }
    if(!open){
      Logger.info("###turboModule Close KeyboardObserver");
      this.unregisterNativeKeyboardWillEvents();
      this.unregisterKeyboardHeightChangeEvent();
      this.keyboardStatus = KeyboardStatusType.HIDE;
      this.keyboardHeight = 0;
      this.focusedInputTarget = -1;
    }else{
      const nativeWillRegistered = this.registerNativeKeyboardWillEvents();
      Logger.info('###turboModule keyboard will event source', nativeWillRegistered ? 'native' : 'keyboardHeightChange');
      try {
        this.unregisterKeyboardHeightChangeEvent();
        this.keyboardHeightChangeCallback = (data) => {
          let height = this.getKeyboardHeightFromAvoidArea(Math.ceil(px2vp(data)))
          if(open){
            const previousHeight = this.keyboardHeight || 0;

            if(previousHeight == height ){
              return
            }
            if (height > 0) {
              this.keyboardStatus = KeyboardStatusType.SHOW;
            } else {
              this.keyboardStatus = KeyboardStatusType.HIDE;
            }
            this.keyboardHeight = height;
            this.keyboardControllerEventHandle(previousHeight, height, !this.nativeKeyboardWillEventEnabled);
            this.ctx.rnInstance.postMessageToCpp('keyboardHeightChange', height);
          }

        };
        this.currentWindow.on('keyboardHeightChange', this.keyboardHeightChangeCallback);
      } catch (exception) {
        Logger.error('Failed to enable the listener for keyboard height changes. Cause: ' + JSON.stringify(exception));
      }
    }

  }

  private getKeyboardAppearance(): String {
    const colorMode = this.context.config.colorMode;
    if (colorMode == ConfigurationConstant.ColorMode.COLOR_MODE_DARK) {
      return "dark";
    }
    else if (colorMode == ConfigurationConstant.ColorMode.COLOR_MODE_LIGHT) {
      return "light";
    }
    return "default";
  }

  private async setWindowLayoutFullScreen(preserveEdgeToEdge:boolean){
    let windowInstance: window.Window | undefined = undefined;
    windowInstance = await window.getLastWindow(this.context);
    try {
      let promise = windowInstance.setWindowLayoutFullScreen(preserveEdgeToEdge);
      promise.then(() => {
        Logger.info('Succeeded in setWindowLayoutFullScreen .' + preserveEdgeToEdge);
      }).catch((err: BusinessError) => {
        Logger.error(`Failed to setWindowLayoutFullScreen. Cause code: ${err.code}, message: ${err.message}`);
      });
    } catch (exception) {
      Logger.error(`Failed to setWindowLayoutFullScreen. Cause code: ${exception.code}, message: ${exception.message}`);
    }
  }
}
