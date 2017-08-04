/*
* macOS addon
*/

const EventEmitter = require('events');
const addon = require('bindings')('wb.node');

const emitter = new EventEmitter();
const setWinEventHook = (key, callback) => {
  if (!emitter.listeners(key).length) {
    addon[key]((...args) => {
      emitter.emit(key, ...args);
    });
  }
  emitter.on(key, callback);
}

class AddonClass {
  static findWindowId({ ownerName, windowName }) {
    const id = addon.findWindowId(ownerName, windowName);
    return id === -1 ? null : id;
  }

  static setForegroundWindow(windowId) {
    return addon.setForegroundWindow(windowId);
  }

  static getWindowRect(windowId) {
    const rect = addon.getWindowRect(windowId);
    const isEmpty =
      (rect.left === 0) &&
      (rect.top === 0) &&
      (rect.right === 0) &&
      (rect.bottom === 0);
    return isEmpty ? null : rect;
  }

  static isWindowMinimized(windowId) {
    return addon.isWindowMinimized(windowId);
  }

  static unhookWinEvents() {
    emitter.removeAllListeners();
    return addon.unhookWinEvents();
  }

  static setWinEventHookForeground(callback) {
    setWinEventHook('setWinEventHookForeground', callback);
  }

  static setWinEventHookLocationChange(callback) {
    setWinEventHook('setWinEventHookLocationChange', callback);
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
