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
    let l = left;
    let t = top;
    let r = right;
    let b = bottom;
    if (!l && l !== 0) { l = -32000; }
    if (!t && t !== 0) { t = -32000; }
    if (!r && r !== 0) { r = -1; }
    if (!b && b !== 0) { b = -1; }
    return addon.setWindowRect(hwnd, l, t, r, b);
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

  static initAutomation(hwnd, callback) {
    return addon.initAutomation(hwnd, callback);
  }

  static getContactListItemInfos(hwnd) {
    return addon.getContactListItemInfos(hwnd).sort((a, b) => (a.top - b.top));
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
