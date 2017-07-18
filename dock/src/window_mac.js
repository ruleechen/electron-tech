/*
* osx window
*/

const Window = require('./window');
const addon = require('bindings')('dock.node');

// class
class MacWindow extends Window {
  static helloMac() {
    return addon.helloMac('test');
  }
}

// exports
module.exports = MacWindow;
