/*
* index
*/

const WinWindow = require('./src/window_win');
const MacWindow = require('./src/window_mac');

const isWindows = /^win/.test(process.platform);
const isMacOS = !isWindows;
const Core = isWindows ? WinWindow : MacWindow;

// exports
module.exports = {
  isWindows,
  isMacOS,
  Core,
};
