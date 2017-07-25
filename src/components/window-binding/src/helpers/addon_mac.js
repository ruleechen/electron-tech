/*
* macOS addon
*/

const addon = require('bindings')('window-binding.node');

class AddonClass {
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