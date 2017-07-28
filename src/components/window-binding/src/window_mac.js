/*
* macOS window
*/

const EventEmitter = require('events');
const Window = require('./window');
const Addon = require('./helpers/addon_mac');

class SfbWindow extends EventEmitter {
  constructor() {
    super();
    this.sfbWindowId = SfbWindow.loadWindowId();
    if (!this.sfbWindowId) {
      throw new Error('"sfbWindowId" notfound');
    }
  }

  static loadWindowId() {
    return Addon.findWindowId({
      ownerName: 'Skype for Business',
    });
  }

  show() {
    // Addon.showWindow(this.sfbHwnd);
  }

  hide() {
    // Addon.hideWindow(this.sfbHwnd);
  }

  isVisible() {
    // return Addon.isWindowVisible(this.sfbHwnd);
  }

  bringToTop() {
    // Addon.setForegroundWindow(this.sfbHwnd);
  }

  isMinimized() {
    return Addon.isWindowMinimized(this.sfbWindowId);
  }

  minimize() {
    // Addon.minimizeWindow(this.sfbHwnd);
  }

  restore() {
    // Addon.restoreWindow(this.sfbHwnd);
  }

  setPosition(x, y) {
    // Addon.setPosition ?
  }

  getRect() {
    return Addon.getWindowRect(this.sfbWindowId);
  }

  hook() {
    Addon.setWinEventHookLocationChange((windowId) => {
      if (windowId === this.sfbWindowId) {
        const rect = this.getRect();
        this.emit('move', rect);
      }
    });
    Addon.setWinEventHookForeground((windowId) => {
      if (windowId === this.sfbWindowId) {
        this.emit('foreground');
      }
    });
  }

  unhook() {
    Addon.unhookWinEvents();
  }
}

class RcWindow extends EventEmitter {
  constructor({ browserWindow }) {
    super();
    this.browserWindow = browserWindow;
    this.rcWindowId = RcWindow.loadWindowId();
    if (!this.rcWindowId) {
      throw new Error('"rcWindowId" notfound');
    }
  }

  static loadWindowId() {
    return Addon.findWindowId({
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
    // Addon.setForegroundWindow(this.rcHwnd);
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
    this.browserWindow.on('move', this.moveHandler = () => {
      const rect = this.getRect();
      this.emit('move', rect);
    });
    this.browserWindow.on('focus', this.focusHandler = () => {
      this.emit('foreground');
    });
  }

  unhook() {
    if (this.moveHandler) {
      this.browserWindow.removeListener('move', this.moveHandler);
      delete this.moveHandler;
    }
    if (this.focusHandler) {
      this.browserWindow.removeListener('focus', this.focusHandler);
      delete this.focusHandler;
    }
  }
}

// class
class MacWindow extends Window {
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

  bind() {
    this.rcWindow.hook();
    this.sfbWindow.hook();
    // show window
    this.sfbWindow.show();
    // sync position
    const rect = this.sfbWindow.getRect();
    this.rcWindow.setPosition(rect.right, rect.top);
  }

  unbind() {
    this.rcWindow.unhook();
    this.sfbWindow.unhook();
    Addon.destroy();
  }
}

// exports
module.exports = MacWindow;
