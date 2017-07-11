/*
* osx window
*/

const EventEmitter = require('events');
const addon = require('bindings')('dock.node');

// class
class MacWindow extends EventEmitter {
}

// exports
module.exports = MacWindow;
