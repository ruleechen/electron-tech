/*
* macOS addon
*/

const addon = require('bindings')('wb.node');

class AddonClass {
  static findWindowId({ ownerName, windowName }) {
    const id = addon.findWindowId(ownerName, windowName);
    return id === -1 ? null : id;
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
