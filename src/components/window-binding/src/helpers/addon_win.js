/*
* windows addon
*/

const EventEmitter = require('events');
const addon = require('bindings')('wb.node');

const emitter = new EventEmitter();
const setWinEventHook = (key, hwnd, callback) => {
  const name = `${key}.${hwnd}`;
  if (!emitter.listeners(name).length) {
    addon[key](hwnd, (windowId) => {
      emitter.emit(name, windowId);
    });
  }
  emitter.on(name, callback);
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

  static setWinEventHookObjectCreate(hwnd, callback) {
    setWinEventHook('setWinEventHookObjectCreate', hwnd, callback);
  }

  static setWinEventHookObjectDestroy(hwnd, callback) {
    setWinEventHook('setWinEventHookObjectDestroy', hwnd, callback);
  }

  static setWinEventHookObjectShow(hwnd, callback) {
    setWinEventHook('setWinEventHookObjectShow', hwnd, callback);
  }

  static setWinEventHookObjectHide(hwnd, callback) {
    setWinEventHook('setWinEventHookObjectHide', hwnd, callback);
  }

  static setWinEventHookLocationChange(hwnd, callback) {
    setWinEventHook('setWinEventHookLocationChange', hwnd, callback);
  }

  static setWinEventHookMinimizeStart(hwnd, callback) {
    setWinEventHook('setWinEventHookMinimizeStart', hwnd, callback);
  }

  static setWinEventHookMinimizeEnd(hwnd, callback) {
    setWinEventHook('setWinEventHookMinimizeEnd', hwnd, callback);
  }

  static setWinEventHookForeground(hwnd, callback) {
    setWinEventHook('setWinEventHookForeground', hwnd, callback);
  }

  static initContactListAutomation(hwnd, callback) {
    return addon.initContactListAutomation(hwnd, callback);
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
