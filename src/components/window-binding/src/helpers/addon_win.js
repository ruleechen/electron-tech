/*
* windows addon
*/

const addon = require('bindings')('windowbinding.node');

class AddonClass {
  static helloWorld() {
    return addon.helloWorld('test');
  }

  static findWindowHwnd({ className, windowName }) {
    return addon.findWindowHwnd(className, windowName);
  }

  static bringWindowToTop(hwnd) {
    return !!addon.bringWindowToTop(hwnd);
  }

  static setForegroundWindow(hwnd) {
    return !!addon.setForegroundWindow(hwnd);
  }

  static getWindowRect(hwnd) {
    return addon.getWindowRect(hwnd);
  }

  static isWindowVisible(hwnd) {
    return !!addon.isWindowVisible(hwnd);
  }

  static showWindow(hwnd) {
    return !!addon.showWindow(hwnd);
  }

  static hideWindow(hwnd) {
    return !!addon.hideWindow(hwnd);
  }

  static isWindowMinimized(hwnd) {
    return !!addon.isWindowMinimized(hwnd);
  }

  static minimizeWindow(hwnd) {
    return !!addon.minimizeWindow(hwnd);
  }

  static restoreWindow(hwnd) {
    return !!addon.restoreWindow(hwnd);
  }

  static unhookWinEvents() {
    return addon.unhookWinEvents();
  }

  static setWinEventHookObjectHide(callback) {
    return addon.setWinEventHookObjectHide(callback);
  }

  static setWinEventHookObjectShow(callback) {
    return addon.setWinEventHookObjectShow(callback);
  }

  static setWinEventHookLocationChange(callback) {
    return addon.setWinEventHookLocationChange(callback);
  }

  static setWinEventHookMinimizeStart(callback) {
    return addon.setWinEventHookMinimizeStart(callback);
  }

  static setWinEventHookMinimizeEnd(callback) {
    return addon.setWinEventHookMinimizeEnd(callback);
  }

  static setWinEventHookForeground(callback) {
    return addon.setWinEventHookForeground(callback);
  }

  static testCallback(name, callback) {
    return addon.testCallback(name, callback);
  }

  static destroy() {
    return addon.destroy();
  }
}

module.exports = AddonClass;
