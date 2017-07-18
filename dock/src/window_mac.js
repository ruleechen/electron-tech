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
}

// exports
module.exports = MacWindow;
