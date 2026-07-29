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
import { BusinessError, deviceInfo } from '@kit.BasicServicesKit';
import { JSON } from '@kit.ArkTS';
import { ConfigurationConstant } from '@kit.AbilityKit';
import { KeyboardAvoidMode } from '@kit.ArkUI';

declare function px2vp(px: number): number;

const NATIVE_KEYBOARD_WILL_EVENT_MIN_API = 20;
const LEGACY_KEYBOARD_ANIMATION_DURATION = 150;
const WINDOW_SESSION_MANAGER = 'SystemCapability.Window.SessionManager';

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
  private currentWindow: window.Window | undefined = undefined;
  private enabled: boolean = false;
  private cleanUpCallbacks: (() => void)[] = [];
  private previousKeyboardAvoidMode: KeyboardAvoidMode | undefined = undefined;
  private keyboardAvoidModeControlled: boolean = false;
  private supportsNativeWillEvents: boolean = false;
  private keyboardHeightListenerRegistered: boolean = false;
  private nativeWillShowListenerRegistered: boolean = false;
  private nativeWillHideListenerRegistered: boolean = false;
  private observerGeneration: number = 0;
  private focusedInputTarget: number = -1;
  private focusedInputKeyboardType: string = 'default';
  private currentKeyboardAnimationDuration: number = 0;

  private readonly keyboardHeightChangeHandler = (_data: number): void => {
    if (!this.enabled || !this.currentWindow) {
      return;
    }
    try {
      const keyboardAvoidArea = this.currentWindow.getWindowAvoidArea(window.AvoidAreaType.TYPE_KEYBOARD).bottomRect;
      const height = Math.ceil(px2vp(keyboardAvoidArea.height));
      if (this.keyboardHeight === height) {
        return;
      }
      const wasVisible = this.keyboardHeight > 0;
      const isVisible = height > 0;
      this.keyboardStatus = height > 0 ? KeyboardStatusType.SHOW : KeyboardStatusType.HIDE;
      this.keyboardHeight = height;
      if (wasVisible !== isVisible) {
        this.keyboardControllerEventHandle(this.keyboardStatus, height);
      }
      this.ctx.rnInstance.postMessageToCpp('keyboardHeightChange', height);
    } catch (exception) {
      Logger.error('Failed to handle keyboard height change. Cause: ' + JSON.stringify(exception));
    }
  };

  private readonly keyboardWillShowHandler = (info: window.KeyboardInfo): void => {
    if (!this.enabled) {
      return;
    }
    const duration = this.getKeyboardDurationFromKeyboardInfo(info);
    this.currentKeyboardAnimationDuration = duration;
    const height = Math.ceil(px2vp(info.endRect.height));
    this.emitKeyboardEvent(KeyboardControllerEventName.KEYBOARD_WILL_SHOW, height, duration);
  };

  private readonly keyboardWillHideHandler = (info: window.KeyboardInfo): void => {
    if (!this.enabled) {
      return;
    }
    const duration = this.getKeyboardDurationFromKeyboardInfo(info);
    this.currentKeyboardAnimationDuration = duration;
    this.emitKeyboardEvent(KeyboardControllerEventName.KEYBOARD_WILL_HIDE, 0, duration);
  };
  constructor(ctx) {
    super(ctx);
    this.context = this.ctx.uiAbilityContext;
    this.eventListeners = this.supportListeners();

    // 订阅 C++ 层发来的 focusDidSet 消息
    this.cleanUpCallbacks.push(
      this.ctx.rnInstance.cppEventEmitter.subscribe("focusDidSet", (payload: { current: number, count: number }) => {
        Logger.info('###turboModule received focusDidSet from cpp', String(payload.current) + ',' + String(payload.count));
        if (this.eventListeners.includes(KeyboardControllerEventName.FOCUS_DID_SET)) {
          this.ctx.rnInstance.emitDeviceEvent(KeyboardControllerEventName.FOCUS_DID_SET, {
            current: payload.current,
            count: payload.count
          });
        }
      })
    );

    this.cleanUpCallbacks.push(
      this.ctx.rnInstance.cppEventEmitter.subscribe(
        'focusedInputChanged',
        (payload: { target: number, type: string }) => {
          this.focusedInputTarget = payload.target;
          this.focusedInputKeyboardType = payload.type;
        }
      )
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
    // DeviceEventEmitter does not report which event was removed. Keep native
    // emission enabled so removing one JS subscription cannot disable others.
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

  private keyboardControllerEventHandle(keyboardStatus: number, height: number) {
    if (!this.enabled) {
      return;
    }

    const legacyDuration = deviceInfo.sdkApiVersion < NATIVE_KEYBOARD_WILL_EVENT_MIN_API
      ? LEGACY_KEYBOARD_ANIMATION_DURATION
      : 0;
    const duration = this.supportsNativeWillEvents ? this.currentKeyboardAnimationDuration : legacyDuration;
    if (keyboardStatus === KeyboardStatusType.HIDE) {
      if (!this.supportsNativeWillEvents) {
        this.emitKeyboardEvent(KeyboardControllerEventName.KEYBOARD_WILL_HIDE, 0, duration);
      }
      this.emitKeyboardEvent(KeyboardControllerEventName.KEYBOARD_DID_HIDE, 0, duration);
      this.currentKeyboardAnimationDuration = 0;
    }
    if (keyboardStatus === KeyboardStatusType.SHOW) {
      if (!this.supportsNativeWillEvents) {
        this.emitKeyboardEvent(KeyboardControllerEventName.KEYBOARD_WILL_SHOW, height, duration);
      }
      this.emitKeyboardEvent(KeyboardControllerEventName.KEYBOARD_DID_SHOW, height, duration);
    }
  }

  private emitKeyboardEvent(eventName: KeyboardControllerEventName, height: number, duration: number): void {
    if (!this.enabled || !this.eventListeners.includes(eventName)) {
      return;
    }
    this.ctx.rnInstance.emitDeviceEvent(eventName, {
      height,
      duration,
      timestamp: new Date().getTime(),
      target: this.focusedInputTarget,
      type: this.focusedInputKeyboardType,
      appearance: this.getKeyboardAppearance(),
    });
  }

  private async startKeyboardObserver(
    open: boolean,
    disableSystemKeyboardAvoidance: boolean = false
  ): Promise<void> {
    const generation = ++this.observerGeneration;
    this.enabled = open;
    if (!open) {
      this.stopKeyboardObserver();
      return;
    }

    let currentWindow: window.Window;
    try {
      currentWindow = await window.getLastWindow(this.context);
    } catch (exception) {
      Logger.error('Failed to get the current window. Cause: ' + JSON.stringify(exception));
      return;
    }
    if (!this.enabled || generation !== this.observerGeneration) {
      return;
    }

    this.currentWindow = currentWindow;
    this.setKeyboardAvoidModeEnabled(disableSystemKeyboardAvoidance);
    this.supportsNativeWillEvents =
      deviceInfo.sdkApiVersion >= NATIVE_KEYBOARD_WILL_EVENT_MIN_API &&
      canIUse(WINDOW_SESSION_MANAGER);
    this.registerKeyboardHeightListener();
    this.registerNativeWillListeners();
  }

  private stopKeyboardObserver(): void {
    this.unregisterNativeWillListeners();
    if (this.currentWindow && this.keyboardHeightListenerRegistered) {
      try {
        this.currentWindow.off('keyboardHeightChange', this.keyboardHeightChangeHandler);
      } catch (exception) {
        Logger.error('Failed to close keyboardHeightChange. Cause: ' + JSON.stringify(exception));
      }
    }
    this.keyboardHeightListenerRegistered = false;
    this.setKeyboardAvoidModeEnabled(false);
    this.currentWindow = undefined;
    this.keyboardHeight = 0;
    this.keyboardStatus = KeyboardStatusType.HIDE;
    this.currentKeyboardAnimationDuration = 0;
    this.supportsNativeWillEvents = false;
  }

  private registerKeyboardHeightListener(): void {
    if (!this.currentWindow || this.keyboardHeightListenerRegistered) {
      return;
    }
    try {
      this.currentWindow.on('keyboardHeightChange', this.keyboardHeightChangeHandler);
      this.keyboardHeightListenerRegistered = true;
    } catch (exception) {
      Logger.error('Failed to enable keyboardHeightChange. Cause: ' + JSON.stringify(exception));
    }
  }

  private registerNativeWillListeners(): void {
    if (!this.currentWindow || !this.supportsNativeWillEvents ||
        this.nativeWillShowListenerRegistered || this.nativeWillHideListenerRegistered) {
      return;
    }

    try {
      this.currentWindow.on('keyboardWillShow', this.keyboardWillShowHandler);
      this.nativeWillShowListenerRegistered = true;
      this.currentWindow.on('keyboardWillHide', this.keyboardWillHideHandler);
      this.nativeWillHideListenerRegistered = true;
    } catch (exception) {
      this.unregisterNativeWillListeners();
      this.supportsNativeWillEvents = false;
      Logger.error('Native keyboard will events unavailable; using confirmed-state fallback. Cause: ' +
        JSON.stringify(exception));
    }
  }

  private unregisterNativeWillListeners(): void {
    if (!this.currentWindow) {
      this.nativeWillShowListenerRegistered = false;
      this.nativeWillHideListenerRegistered = false;
      return;
    }
    if (this.nativeWillShowListenerRegistered) {
      try {
        this.currentWindow.off('keyboardWillShow', this.keyboardWillShowHandler);
      } catch (exception) {
        Logger.error('Failed to close keyboardWillShow. Cause: ' + JSON.stringify(exception));
      }
      this.nativeWillShowListenerRegistered = false;
    }
    if (this.nativeWillHideListenerRegistered) {
      try {
        this.currentWindow.off('keyboardWillHide', this.keyboardWillHideHandler);
      } catch (exception) {
        Logger.error('Failed to close keyboardWillHide. Cause: ' + JSON.stringify(exception));
      }
      this.nativeWillHideListenerRegistered = false;
    }
  }

  private getKeyboardDurationFromKeyboardInfo(info: window.KeyboardInfo): number {
    if (info.animated === false) {
      return 0;
    }
    const duration = info.config?.duration;
    return typeof duration === 'number' && duration >= 0 ? duration : 0;
  }

  __onDestroy__(): void {
    this.enabled = false;
    ++this.observerGeneration;
    this.stopKeyboardObserver();
    this.cleanUpCallbacks.forEach((callback) => callback());
    this.cleanUpCallbacks = [];
    super.__onDestroy__();
  }

  private setKeyboardAvoidModeEnabled(enabled: boolean): void {
    if (!this.currentWindow) {
      return;
    }
    try {
      const uiContext = this.currentWindow.getUIContext();
      if (enabled) {
        if (!this.keyboardAvoidModeControlled) {
          this.previousKeyboardAvoidMode = uiContext.getKeyboardAvoidMode();
          this.keyboardAvoidModeControlled = true;
        }
        uiContext.setKeyboardAvoidMode(KeyboardAvoidMode.NONE);
      } else if (this.keyboardAvoidModeControlled) {
        if (this.previousKeyboardAvoidMode !== undefined) {
          uiContext.setKeyboardAvoidMode(this.previousKeyboardAvoidMode);
        }
        this.previousKeyboardAvoidMode = undefined;
        this.keyboardAvoidModeControlled = false;
      }
    } catch (exception) {
      Logger.error('Failed to update KeyboardAvoidMode. Cause: ' + JSON.stringify(exception));
    }
  }

  private getKeyboardAppearance(): string {
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
