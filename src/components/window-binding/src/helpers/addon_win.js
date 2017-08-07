/*
* windows addon
*/

const EventEmitter = require('events');
const addon = require('bindings')('wb.node');

const emitter = new EventEmitter();
const setWinEventHook = (key, callback) => {
  if (!emitter.listeners(key).length) {
    addon[key]((windowId) => {
      emitter.emit(key, windowId);
    });
  }
  emitter.on(key, callback);
}

class AddonClass {
  static findWindowHwnd({ className, windowName }) {
    return addon.findWindowHwnd(className, windowName);
  }

  static allowSetForegroundWindow(hwnd) {
    return addon.allowSetForegroundWindow(hwnd || '-1');
  }

  static setAlwaysOnTop(hwnd, onTop) {
    return addon.setAlwaysOnTop(hwnd, onTop);
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

  static setWindowRect(hwnd, left, top, right, bottom) {
    if (!left && left !== 0) { left = -32000; }
    if (!top && top !== 0) { top = -32000; }
    if (!right && right !== 0) { right = -1; }
    if (!bottom && bottom !== 0) { bottom = -1; }
    return addon.setWindowRect(hwnd, left, top, right, bottom);
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
    emitter.removeAllListeners();
    return addon.unhookWinEvents();
  }

  static setWinEventHookObjectCreate(callback) {
    setWinEventHook('setWinEventHookObjectCreate', callback);
  }

  static setWinEventHookObjectDestroy(callback) {
    setWinEventHook('setWinEventHookObjectDestroy', callback);
  }

  static setWinEventHookObjectHide(callback) {
    setWinEventHook('setWinEventHookObjectHide', callback);
  }

  static setWinEventHookObjectShow(callback) {
    setWinEventHook('setWinEventHookObjectShow', callback);
  }

  static setWinEventHookLocationChange(callback) {
    setWinEventHook('setWinEventHookLocationChange', callback);
  }

  static setWinEventHookMinimizeStart(callback) {
    setWinEventHook('setWinEventHookMinimizeStart', callback);
  }

  static setWinEventHookMinimizeEnd(callback) {
    setWinEventHook('setWinEventHookMinimizeEnd', callback);
  }

  static setWinEventHookForeground(callback) {
    setWinEventHook('setWinEventHookForeground', callback);
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
