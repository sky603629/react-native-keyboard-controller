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

// 原生 keyboardWillShow/keyboardWillHide 事件(API 20+)的起始版本
const NATIVE_KEYBOARD_WILL_EVENT_MIN_API = 20;

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
  private keyboardHeight: number;
  private keyboardStatus: KeyboardStatusType;
  private eventListeners: KeyboardControllerEventName[];
  private currentWindow:window.Window;
  private enabled:boolean;
  private cleanUpCallbacks: (() => void)[] = [];
  private previousKeyboardAvoidMode: KeyboardAvoidMode | undefined = undefined;
  private keyboardAvoidModeControlled: boolean = false;
  // 是否支持 API20+ 原生 keyboardWillShow/keyboardWillHide 事件;
  // true 时走原生"即将"回调, false 时降级为 did 时同时机补发 will(仿 Android 模拟)
  private supportsNativeWillEvents: boolean = false;
  // 原生 will 事件是否已注册(避免重复 on / 关闭时确保 off)
  private nativeWillListenersRegistered: boolean = false;
  // 焦点输入框信息缓存(由 C++ onFocus 早期推送, 供 will/did payload 使用), 解决时序倒置
  private focusedInputTarget: number = -1;
  private focusedInputKeyboardType: string = "default";
  // 键盘动画时长缓存: will 回调取 KeyboardInfo.config.duration 并缓存, did 读缓存
  private readonly defaultKeyboardAnimationDuration: number = 250;
  private currentKeyboardAnimationDuration: number = this.defaultKeyboardAnimationDuration;
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

    // 订阅 C++ 层焦点输入框信息(target + type), 由 onFocus 早期推送, 供键盘事件 payload 使用
    this.cleanUpCallbacks.push(
      this.ctx.rnInstance.cppEventEmitter.subscribe(
        "focusedInputChanged",
        (payload: { target: number, type: string }) => {
          this.focusedInputTarget = payload.target;
          this.focusedInputKeyboardType = payload.type;
          Logger.info('###turboModule focusedInputChanged',
            'target=' + this.focusedInputTarget + ' type=' + this.focusedInputKeyboardType);
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
    Logger.info('removeListeners count=' + count);
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
    Logger.info('###turboModule keyboardControllerEventHandle', String(keyboardStatus) + ',' + String(height));
    if(!this.enabled){
      return
    }

    if (this.keyboardStatus == KeyboardStatusType.HIDE) {
      // 键盘隐藏
      Logger.info('###turboModule keyboardControllerEventHandle HIDE');
      // 低版本无原生 will 事件 -> 在 did 时同时机补发 will(仿 Android 模拟);
      // 高版本 will 已由原生 keyboardWillHide 回调单独发射, 这里只发 did
      if (!this.supportsNativeWillEvents) {
        this.emitKeyboardEvent(KeyboardControllerEventName.KEYBOARD_WILL_HIDE, height);
      }
      this.emitKeyboardEvent(KeyboardControllerEventName.KEYBOARD_DID_HIDE, height);
    }
    if (this.keyboardStatus == KeyboardStatusType.SHOW) {
      // 键盘显示
      Logger.info('###turboModule keyboardControllerEventHandle SHOW');
      if (!this.supportsNativeWillEvents) {
        this.emitKeyboardEvent(KeyboardControllerEventName.KEYBOARD_WILL_SHOW, height);
      }
      this.emitKeyboardEvent(KeyboardControllerEventName.KEYBOARD_DID_SHOW, height);
    }
  }

  /**
   * 统一发射键盘事件. payload 对齐上游 KeyboardEventData:
   * height/duration/timestamp/target/type/appearance
   * - duration: will 路径取 KeyboardInfo.config.duration; did 路径读 currentKeyboardAnimationDuration 缓存
   * - target/type: 读 C++ onFocus 早期推送的 focusedInputTarget/focusedInputKeyboardType 缓存
   * */
  private emitKeyboardEvent(eventName: KeyboardControllerEventName, height: number, duration?: number) {
    if (!this.enabled) {
      return;
    }
    if (!this.eventListeners.includes(eventName)) {
      return;
    }
    this.ctx.rnInstance.emitDeviceEvent(eventName, {
      height: height,
      duration: duration ?? this.currentKeyboardAnimationDuration,
      timestamp: new Date().getTime(),
      target: this.focusedInputTarget,
      type: this.focusedInputKeyboardType,
      appearance: this.getKeyboardAppearance(),
    });
  }

  private async startKeyboardObserver(open:boolean, disableSystemKeyboardAvoidance: boolean = false) {
    Logger.info("###turboModule startKeyboardObserver",String(open));
    this.enabled=open;
    this.currentWindow = await window.getLastWindow(this.context);
    if (!this.currentWindow) {
      throw ('windowInstance is null')
      return
    }
    this.setKeyboardAvoidModeEnabled(open && disableSystemKeyboardAvoidance);
    // 版本隔离: 仅 API20+ 设备支持原生 keyboardWillShow/keyboardWillHide 事件(且仅 Phone/Tablet/PC/2in1)
    this.supportsNativeWillEvents = (deviceInfo.sdkApiVersion >= NATIVE_KEYBOARD_WILL_EVENT_MIN_API);
    if(!open){
      // 关闭时重置 payload 缓存, 避免下次开启时残留旧焦点信息
      this.focusedInputTarget = -1;
      this.focusedInputKeyboardType = "default";
      this.currentKeyboardAnimationDuration = this.defaultKeyboardAnimationDuration;
      try {
        Logger.info("###turboModule Close KeyboardObserver");
        // 先关闭原生 will 监听(若有), 再关 keyboardHeightChange
        this.offNativeWillListeners();
        this.currentWindow.off('keyboardHeightChange', (data) => {
          this.keyboardStatus = KeyboardStatusType.HIDE;
          this.keyboardHeight = 0;
          Logger.info('### close keyboardHeightChange observer');
        });
      } catch (exception) {
        Logger.error('### Failed to close the listener for keyboard height changes. Cause: ' + JSON.stringify(exception));
      }
    }else{
      try {
        this.currentWindow.on('keyboardHeightChange', (data) => {
          const keyboardAvoidArea = this.currentWindow?.getWindowAvoidArea(window.AvoidAreaType.TYPE_KEYBOARD).bottomRect;
          let height = Math.ceil(px2vp(keyboardAvoidArea.height))
          if(open){
            if(this.keyboardHeight == height ){
              return
            }
            if (height > 0) {
              this.keyboardStatus = KeyboardStatusType.SHOW;
            } else {
              this.keyboardStatus = KeyboardStatusType.HIDE;
            }
            this.keyboardHeight = height;
            this.keyboardControllerEventHandle(this.keyboardStatus, height);
            this.ctx.rnInstance.postMessageToCpp('keyboardHeightChange', height);
          }

        });
      } catch (exception) {
        Logger.error('Failed to enable the listener for keyboard height changes. Cause: ' + JSON.stringify(exception));
      }
      // API20+: 注册原生 keyboardWillShow/keyboardWillHide, 获得真正的"即将"语义
      this.onNativeWillListeners();
    }

  }

  /**
   * 注册原生 keyboardWillShow/keyboardWillHide 监听(API 20+).
   * 注册失败(SessionManager 未就绪 / 非 Phone/Tablet/PC 设备返回 801 等) -> 降级为 supportsNativeWillEvents=false,
   * 此时 keyboardControllerEventHandle 会在 did 时同时机补发 will, 保证 will 监听不空.
   * */
  private onNativeWillListeners() {
    if (!this.supportsNativeWillEvents || !this.currentWindow) {
      return;
    }
    if (this.nativeWillListenersRegistered) {
      return;
    }
    try {
      this.currentWindow.on('keyboardWillShow', (info: window.KeyboardInfo) => {
        Logger.info('###turboModule native keyboardWillShow, KeyboardInfo=' + JSON.stringify(info));
        if (!this.enabled) {
          return;
        }
        // 取真实动画时长并缓存, 供后续 did 事件使用(对齐 iOS/Android payload.duration)
        const d = this.getKeyboardDurationFromKeyboardInfo(info);
        this.currentKeyboardAnimationDuration = d;
        // will 的 height 用避让区高度兜底, 与 did 同源(对齐 iOS will/did 同值结论)
        this.emitKeyboardEvent(KeyboardControllerEventName.KEYBOARD_WILL_SHOW, this.getKeyboardHeightForWill(info), d);
      });
      this.currentWindow.on('keyboardWillHide', (info: window.KeyboardInfo) => {
        Logger.info('###turboModule native keyboardWillHide, KeyboardInfo=' + JSON.stringify(info));
        if (!this.enabled) {
          return;
        }
        const d = this.getKeyboardDurationFromKeyboardInfo(info);
        this.currentKeyboardAnimationDuration = d;
        this.emitKeyboardEvent(KeyboardControllerEventName.KEYBOARD_WILL_HIDE, 0, d);
      });
      this.nativeWillListenersRegistered = true;
      Logger.info('###turboModule native keyboardWillShow/Hide registered (API' + deviceInfo.sdkApiVersion + ')');
    } catch (e) {
      // 设备/系统不支持 -> 降级, did 时补发 will
      this.supportsNativeWillEvents = false;
      this.nativeWillListenersRegistered = false;
      Logger.error('### native keyboardWillShow/Hide not supported, fallback to did-complement: ' + JSON.stringify(e));
    }
  }

  private offNativeWillListeners() {
    if (!this.currentWindow || !this.nativeWillListenersRegistered) {
      return;
    }
    try {
      this.currentWindow.off('keyboardWillShow');
      this.currentWindow.off('keyboardWillHide');
    } catch (e) {
      Logger.error('### off native keyboardWillShow/Hide failed: ' + JSON.stringify(e));
    }
    this.nativeWillListenersRegistered = false;
  }

  /**
   * 取 will 事件的键盘高度. KeyboardInfo 精确字段需真机 JSON.stringify 确认,
   * 兜底用避让区高度保证与 did 同源.
   * */
  private getKeyboardHeightForWill(info: window.KeyboardInfo): number {
    try {
      const avoidArea = this.currentWindow?.getWindowAvoidArea(window.AvoidAreaType.TYPE_KEYBOARD).bottomRect;
      return Math.ceil(px2vp(avoidArea.height));
    } catch (e) {
      Logger.error('### getKeyboardHeightForWill failed: ' + JSON.stringify(e));
      return 0;
    }
  }

  /**
   * 从 KeyboardInfo.config 取键盘动画时长(毫秒).
   * 真机结构: {"config":{"curve":1,"duration":150,"param":[...]}}
   * 取不到或非法时降级默认值 250(对齐 Android 边界情况).
   * */
  private getKeyboardDurationFromKeyboardInfo(info: window.KeyboardInfo): number {
    try {
      const payload = info as object as Record<string, Object>;
      const config = payload['config'] as Record<string, Object> | undefined;
      const d = config ? config['duration'] : undefined;
      return (typeof d === 'number' && d > 0) ? d : this.defaultKeyboardAnimationDuration;
    } catch (e) {
      return this.defaultKeyboardAnimationDuration;
    }
  }

  private setKeyboardAvoidModeEnabled(enabled: boolean): void {
    try {
      if (!this.currentWindow) {
        return;
      }
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
