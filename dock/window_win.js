/*
* windows window
*/

const EventEmitter = require('events');
const addon = require('bindings')('dock.node');

// class
class WinWindow extends EventEmitter {
  constructor(hwnd) {
    this._hwnd = hwnd;
  }

  static findWindowHwnd(className, windowName) {
    return addon.findWindowHwnd(className || null, windowName || null);
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
