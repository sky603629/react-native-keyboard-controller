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
import window from '@ohos.window';
import common from '@ohos.app.ability.common';
import { BusinessError } from '@kit.BasicServicesKit';
import Logger  from './Logger';


interface StatusBarManagerCompatSpec {
  getConstants(): {};
  setHidden(hidden: boolean): void;
  setColor(color: number, animated: boolean): void;
  setTranslucent(translucent: boolean): void;
  setStyle(style: string): void;

}
export  class RNStatusBarManagerCompatTurboModule extends TurboModule implements StatusBarManagerCompatSpec{
  private context: common.UIAbilityContext;
  constructor(ctx) {
    super(ctx);
    this.context = this.ctx.uiAbilityContext;
  }
  private  async  setWindowSystemBarProperties(properties:window.SystemBarProperties){
    let windowInstance: window.Window | undefined = undefined;
    windowInstance = await window.getLastWindow(this.context);
    let systemBarProperty = windowInstance.getWindowSystemBarProperties();
    let newSystemBarProperty: window.SystemBarProperties = {
      ...systemBarProperty,
      ...properties
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
  readonly  getConstants:()=>{};
  // set mode
  async  setHidden(hidden: boolean): Promise<void> {
    Logger.info("###turboModule setHidden");
    let windowInstance: window.Window | undefined = undefined;
    windowInstance = await window.getLastWindow(this.context);
    let names: Array<'status' | 'navigation'> = ['navigation'];
    hidden && names.push('status');
    try {
      let promise = windowInstance.setWindowSystemBarEnable(names);
      promise.then(() => {
        Logger.info('Succeeded in setting the system bar to be invisible.');
      }).catch((err: BusinessError) => {
        Logger.error(`Failed to set the system bar to be invisible. Cause code: ${err.code}, message: ${err.message}`);
      });
    } catch (exception) {
      Logger.error(`Failed to set the system bar to be invisible. Cause code: ${exception.code}, message: ${exception.message}`);
    }
  }

  async  setColor(color: number, animated: boolean): Promise<void> {
    Logger.info("###turboModule setColor");
    let newSystemBarProperty: window.SystemBarProperties = {
      statusBarColor: this.toHarmonyColor(color),
      enableStatusBarAnimation:animated
    };
    this.setWindowSystemBarProperties(newSystemBarProperty)
  }

  async  setTranslucent(translucent: boolean): Promise<void> {
    Logger.info("###turboModule setTranslucent");
    let windowInstance: window.Window | undefined = undefined;
    windowInstance = await window.getLastWindow(this.context);
    let systemBarProperty = windowInstance.getWindowSystemBarProperties();
    let newSystemBarProperty: window.SystemBarProperties = {
      statusBarColor:translucent?'#00000000':systemBarProperty.statusBarColor,
    };
    this.setWindowSystemBarProperties(newSystemBarProperty)
  }

  setStyle(style: 'dark-content'|'light-content'): void {
    Logger.info("###turboModule setStyle");
     let themeFlag= style == "dark-content"
    let newSystemBarProperty: window.SystemBarProperties = {
      statusBarContentColor:themeFlag?"#000000":"#ffffff",
    };
    this.setWindowSystemBarProperties(newSystemBarProperty)
  }

  private toHarmonyColor(color: number): string {
    const unsignedColor = color >>> 0;
    const hex = unsignedColor > 0xffffff
      ? unsignedColor.toString(16).padStart(8, '0')
      : unsignedColor.toString(16).padStart(6, '0');

    return `#${hex}`;
  }

}


