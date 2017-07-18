/*
* osx window
*/

const Window = require('./window');
const addon = require('bindings')('dock.node');

// addon wrap
class AddonWrap {
  static helloMac() {
    return addon.helloMac('test');
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
