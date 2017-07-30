/*
* windows addon
*/

const addon = require('bindings')('wb.node');

class AddonClass {
  static findWindowHwnd({ className, windowName }) {
    return addon.findWindowHwnd(className, windowName);
  }

  static bringWindowToTop(hwnd) {
    return addon.bringWindowToTop(hwnd);
  }

  static setForegroundWindow(hwnd) {
    return addon.setForegroundWindow(hwnd);
  }

  static getWindowRect(hwnd) {
    const rect = addon.getWindowRect(hwnd);
    const isEmpty =
      (rect.left === 0) &&
      (rect.top === 0) &&
      (rect.right === 0) &&
      (rect.bottom === 0);
    return isEmpty ? null : rect;
  }

  static isWindowVisible(hwnd) {
    return addon.isWindowVisible(hwnd);
  }

  static showWindow(hwnd) {
    return addon.showWindow(hwnd);
  }

  static hideWindow(hwnd) {
    return addon.hideWindow(hwnd);
  }

  static isWindowMinimized(hwnd) {
    return addon.isWindowMinimized(hwnd);
  }

  static minimizeWindow(hwnd) {
    return addon.minimizeWindow(hwnd);
  }

  static restoreWindow(hwnd) {
    return addon.restoreWindow(hwnd);
  }

  static unhookWinEvents() {
    return addon.unhookWinEvents();
  }

  static setWinEventHookObjectCreate(callback) {
    return addon.setWinEventHookObjectCreate(callback);
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

  static helloWorld() {
    return addon.helloWorld('test');
  }

  static testCallback(name, callback) {
    return addon.testCallback(name, callback);
  }

  static destroy() {
    return addon.destroy();
  }
}

module.exports = AddonClass;
