/*
* osx window
*/

const Window = require('./window');
const addon = require('bindings')('dock.node');

// addon wrap
class AddonWrap {
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

// class
class MacWindow extends Window {
  static get AddonWrap() {
    return AddonWrap;
  }
}

// exports
module.exports = MacWindow;
