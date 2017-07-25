/*
* macOS window
*/

const Window = require('./window');
const Addon = require('./helpers/addon_mac');

// class
class MacWindow extends Window {
  constructor({ browserWindow }) {
    super();
  }

  static get Addon() {
    return Addon;
  }

  bind() {
  }

  unbind() {
  }
}

// exports
module.exports = MacWindow;
