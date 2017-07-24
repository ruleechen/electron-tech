/*
* windows window
*/

const Window = require('./window');
const addon = require('bindings')('dock.node');
const freezeForeground = require('./freeze').create({ timeout: 200 });

class AddonWrap {
  static helloWorld() {
    return addon.helloWorld('test');
  }

  static findWindowHwnd({ className, windowName }) {
    return addon.findWindowHwnd(className, windowName);
  }

  static bringWindowToTop(hwnd) {
    return !!addon.bringWindowToTop(hwnd);
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

  static unhookWinEvents() {
    return addon.unhookWinEvents();
  }

  static setWinEventHookObjectHide(callback) {
    return addon.setWinEventHookObjectHide(callback);
  }

  static setWinEventHookObjectShow(callback) {
    return addon.setWinEventHookObjectShow(callback);
  }

  static setWinEventHookLocationChange(callback) {
    return addon.setWinEventHookLocationChange(callback);
  }

  static setWinEventHookMinimizeStart(callback) {
    return addon.setWinEventHookMinimizeStart(callback);
  }

  static setWinEventHookMinimizeEnd(callback) {
    return addon.setWinEventHookMinimizeEnd(callback);
  }

  static setWinEventHookForeground(callback) {
    return addon.setWinEventHookForeground(callback);
  }

  static testCallback(name, callback) {
    return addon.testCallback(name, callback);
  }

  static destroy() {
    return addon.destroy();
  }
}

class SfbWindow extends Window {
  constructor() {
    super();
    this.sfbHwnd = SfbWindow.loadHwnd();
    if (!this.sfbHwnd) {
      throw new Error('"sfbHwnd" notfound');
    }
  }

  static loadHwnd() {
    return AddonWrap.findWindowHwnd({
      className: 'CommunicatorMainWindowClass',
      windowName: 'Skype for Business ',
    });
  }

  isVisible() {
    return AddonWrap.isWindowVisible(this.sfbHwnd);
  }

  showWindow() {
    if (!this.isVisible()) {
      AddonWrap.showWindow(this.sfbHwnd);
    }
    this.bringToTop();
  }

  bringToTop() {
    AddonWrap.setForegroundWindow(this.sfbHwnd);
  }

  setPosition(x, y) {
    // AddonWrap.setPosition ?
  }

  getRect() {
    return AddonWrap.getWindowRect(this.sfbHwnd);
  }

  hook() {
    AddonWrap.setWinEventHookLocationChange((hwnd) => {
      if (hwnd === this.sfbHwnd) {
        const rect = this.getRect();
        this.emit('move', rect);
      }
    });
    AddonWrap.setWinEventHookForeground((hwnd) => {
      if (hwnd === this.sfbHwnd) {
        if (!freezeForeground()) {
          this.emit('foreground');
        }
      }
    });
  }

  unhook() {
    AddonWrap.unhookWinEvents();
  }
}

class RcWindow extends Window {
  constructor({ browserWindow }) {
    super();
    this.browserWindow = browserWindow;
    this.rcHwnd = RcWindow.loadHwnd();
    if (!this.rcHwnd) {
      throw new Error('"rcHwnd" notfound');
    }
  }

  static loadHwnd() {
    return AddonWrap.findWindowHwnd({
      className: 'Chrome_WidgetWin_1',
      windowName: 'RingCentral for Skype for Business',
    });
  }

  isVisible() {
    return this.browserWindow.isVisible();
  }

  showWindow() {
    if (!this.isVisible()) {
      AddonWrap.showWindow(this.rcHwnd);
    }
    this.bringToTop();
  }

  bringToTop() {
    AddonWrap.setForegroundWindow(this.rcHwnd);
  }

  setPosition(x, y) {
    this.browserWindow.setPosition(x, y);
  }

  getRect() {
    const size = this.browserWindow.getSize();
    const position = this.browserWindow.getPosition();
    return {
      left: position[0],
      top: position[1],
      right: position[0] + size[0],
      bottom: position[1] + size[1],
    };
  }

  hook() {
    this.browserWindow.on('move', this._move = () => {
      const rect = this.getRect();
      this.emit('move', rect);
    });
    this.browserWindow.on('focus', this._focus = () => {
      if (!freezeForeground()) {
        this.emit('foreground');
      }
    });
  }

  unhook() {
    if (this._move) {
      this.browserWindow.removeListener('move', this._move);
      delete this._move;
    }
    if (this._focus) {
      this.browserWindow.removeListener('focus', this._focus);
      delete this._focus;
    }
  }
}

// class
class WinWindow extends Window {
  constructor({ browserWindow }) {
    super();
    // new
    this.rcWindow = new RcWindow({ browserWindow });
    this.sfbWindow = new SfbWindow();
    // foreground
    this.rcWindow.on('foreground', () => {
      setTimeout(() => {
        this.sfbWindow.bringToTop();
        this.rcWindow.bringToTop();
      }, 0);
    });
    this.sfbWindow.on('foreground', () => {
      setTimeout(() => {
        this.rcWindow.bringToTop();
        this.sfbWindow.bringToTop();
      }, 0);
    });
    // move
    this.sfbWindow.on('move', (rect) => {
      // sync position
      this.rcWindow.setPosition(rect.right, rect.top);
    });
  }

  tie() {
    this.rcWindow.hook();
    this.sfbWindow.hook();
    // show window
    this.sfbWindow.showWindow();
    // sync position
    const rect = this.sfbWindow.getRect();
    this.rcWindow.setPosition(rect.right, rect.top);
  }

  destroy() {
    this.rcWindow.unhook();
    this.sfbWindow.unhook();
    AddonWrap.destroy();
  }
}

// exports
module.exports = WinWindow;
