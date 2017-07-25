/*
* index
*/

const Window = require('./src/window');
const WinWindow = require('./src/window_win');
const MacWindow = require('./src/window_mac');

const isWindows = /^win/.test(process.platform);
const isMacOS = !isWindows;
const Core = isWindows ? WinWindow : MacWindow;

module.exports = {
  isWindows,
  isMacOS,
  Core,
};
