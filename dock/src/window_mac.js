/*
* macOS window
*/

const Window = require('./window');
const Addon = require('./helpers/addon_mac');

// class
class MacWindow extends Window {
  static get Addon() {
    return Addon;
  }
}

// exports
module.exports = MacWindow;
