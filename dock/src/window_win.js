/*
* windows window
*/

const Window = require('./window');
const Addon = require('./helpers/addon_win');
const freezeForeground = require('./helpers/freeze').create({ timeout: 200 });

class SfbWindow extends Window {
  constructor() {
    super();
    this.sfbHwnd = SfbWindow.loadHwnd();
    if (!this.sfbHwnd) {
      throw new Error('"sfbHwnd" notfound');
    }
  }

  static loadHwnd() {
    return Addon.findWindowHwnd({
      className: 'CommunicatorMainWindowClass',
      windowName: 'Skype for Business ',
    });
  }

  show() {
    Addon.showWindow(this.sfbHwnd);
  }

  hide() {
    Addon.hideWindow(this.sfbHwnd);
  }

  isVisible() {
    return Addon.isWindowVisible(this.sfbHwnd);
  }

  bringToTop() {
    Addon.setForegroundWindow(this.sfbHwnd);
  }

  isMinimized() {
    return Addon.isWindowMinimized(this.sfbHwnd);
  }

  minimize() {
    Addon.minimizeWindow(this.sfbHwnd);
  }

  restore() {
    Addon.restoreWindow(this.sfbHwnd);
  }

  setPosition(x, y) {
    // Addon.setPosition ?
  }

  getRect() {
    return Addon.getWindowRect(this.sfbHwnd);
  }

  hook() {
    Addon.setWinEventHookObjectHide((hwnd) => {
      if (hwnd === this.sfbHwnd) {
        this.emit('hide');
      }
    });
    Addon.setWinEventHookMinimizeStart((hwnd) => {
      if (hwnd === this.sfbHwnd) {
        this.emit('minimize-start');
      }
    });
    Addon.setWinEventHookLocationChange((hwnd) => {
      if (hwnd === this.sfbHwnd) {
        const rect = this.getRect();
        this.emit('move', rect);
      }
    });
    Addon.setWinEventHookForeground((hwnd) => {
      if (hwnd === this.sfbHwnd) {
        if (!freezeForeground()) {
          this.emit('foreground');
        }
      }
    });
  }

  unhook() {
    Addon.unhookWinEvents();
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
    return Addon.findWindowHwnd({
      className: 'Chrome_WidgetWin_1',
      windowName: 'RingCentral for Skype for Business',
    });
  }

  show() {
    this.browserWindow.show();
  }

  hide() {
    this.browserWindow.hide();
  }

  isVisible() {
    return this.browserWindow.isVisible();
  }

  bringToTop() {
    Addon.setForegroundWindow(this.rcHwnd);
  }

  isMinimized() {
    return this.browserWindow.isMinimized();
  }

  minimize() {
    this.browserWindow.minimize();
  }

  restore() {
    this.browserWindow.restore();
  }

  setPosition(x, y) {
    if (x === -32000 || y === -32000) { return; }
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
      if (this.sfbWindow.isMinimized()) {
        this.sfbWindow.restore();
        return;
      }
      if (!this.sfbWindow.isVisible()) {
        this.sfbWindow.show();
        return;
      }
      setTimeout(() => {
        this.sfbWindow.bringToTop();
        this.rcWindow.bringToTop();
      }, 0);
    });
    this.sfbWindow.on('foreground', () => {
      if (this.rcWindow.isMinimized()) {
        this.rcWindow.restore();
        return;
      }
      if (!this.rcWindow.isVisible()) {
        this.rcWindow.show();
        return;
      }
      setTimeout(() => {
        this.rcWindow.bringToTop();
        this.sfbWindow.bringToTop();
      }, 0);
    });
    this.sfbWindow.on('hide', () => {
      this.rcWindow.hide();
    });
    this.sfbWindow.on('minimize-start', () => {
      this.rcWindow.minimize();
    });
    // move
    this.sfbWindow.on('move', (rect) => {
      // sync position
      this.rcWindow.setPosition(rect.right, rect.top);
    });
  }

  static get Addon() {
    return Addon;
  }

  tie() {
    this.rcWindow.hook();
    this.sfbWindow.hook();
    // show window
    this.sfbWindow.show();
    // sync position
    const rect = this.sfbWindow.getRect();
    this.rcWindow.setPosition(rect.right, rect.top);
  }

  destroy() {
    this.rcWindow.unhook();
    this.sfbWindow.unhook();
    Addon.destroy();
  }
}

// exports
module.exports = WinWindow;
