/*
* windows window
*/

const Window = require('./window');
const addon = require('bindings')('dock.node');

// addon wrap
class AddonWrap {
  static findWindowHwnd(className, windowName) {
    return addon.findWindowHwnd(className || null, windowName || null);
  }

  static getForegroundWindow() {
    return addon.getForegroundWindow();
  }

  static setForegroundWindow(hwnd) {
    return !!addon.setForegroundWindow(hwnd);
  }

  static getWindowRect(hwnd) {
    return addon.getWindowRect(hwnd);
  }

  static isWindowVisible(hwnd) {
    return !!addon.isWindowVisible(hwnd);
  }

  static showWindow(hwnd) {
    return !!addon.showWindow(hwnd);
  }

  static testCallback(callback) {
    addon.testCallback(callback);
  }
}

// class
class WinWindow extends Window {
  constructor(hwnd) {
    this._hwnd = hwnd;
  }

  static get AddonWrap() {
    return AddonWrap;
  }

  get left() {
    return this._left;
  }

  get top() {
    return this._top;
  }

  get foreground() {
    return false;
  }

  setPosition({ left, top }) {
    this._left = left;
    this._top = top;
  }

  dockIn(win) {
    if (!(win instanceof WinWindow)) {
      throw new Error("Invalid window object");
    }
    // regoister
    this._dockWin = win;
    this.on('move', this._selfMove = () => {
      if (this.foreground) {
        win.setPosition({
          left: this.left - 100,
          top: this.top - 100,
        });
      }
    });
    win.on('move', this._dockWinMove = () => {
      if (this.foreground) {
        this.setPosition({
          left: win.left - 100,
          top: win.top - 100,
        });
      }
    });
    // event
    this.emit("dockin", win);
    win.emit("dockin", this);
  }

  dockOut() {
    if (!(this._dockWin instanceof WinWindow)) {
      return;
    }
    // unreg
    this.removeListener("move", this._selfMove);
    this._dockWin.removeListener("move", this._dockWinMove);
    this._selfMove = null;
    this._dockWinMove = null;
    this._dockWin = null;
    // event
    this.emit("dockout", win);
    win.emit("dockout", this);
  }
}

// exports
module.exports = WinWindow;
