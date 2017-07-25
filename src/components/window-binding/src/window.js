/*
* window
*/

const EventEmitter = require('events');

// class
class Window extends EventEmitter {
  constructor() {
    super();
  }

  bind() {
    throw new Error('Should "bind" be overwrited by children classes.');
  }

  unbind() {
    throw new Error('Should "unbind" be overwrited by children classes.');
  }
}

// exports
module.exports = Window;

