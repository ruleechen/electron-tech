/*
* macOS addon
*/

const addon = require('bindings')('wb.node');

class AddonClass {
  static findWindowId({ ownerName, windowName }) {
    const id = addon.findWindowId(ownerName, windowName);
    return id === -1 ? null : id;
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
