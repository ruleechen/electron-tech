/*
* osx window
*/

const EventEmitter = require('events');
const addon = require('bindings')('dock.node');

// class
class MacWindow extends EventEmitter {
  static helloMac() {
    return addon.helloMac('test');
  }
}

// exports
module.exports = MacWindow;
