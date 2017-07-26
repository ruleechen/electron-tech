/*
* macOS addon
*/

const addon = require('bindings')('wb.node');

class AddonClass {
  static findWindowId({ ownerName, windowName }) {
    return addon.findWindowId(ownerName, windowName);
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
