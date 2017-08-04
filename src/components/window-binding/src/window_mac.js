/*
* macOS window
*/

const EventEmitter = require('events');
const Window = require('./window');
const Addon = require('./helpers/addon_mac');
const freezeForeground = require('./helpers/freeze').create({ timeout: 200 });

class SfbWindow extends EventEmitter {
  constructor() {
    super();
    this.inited = 0;
    SfbWindow.monitorWindowId((windowId) => {
      this.windowId = windowId;
      if (windowId) {
        this.inited = (new Date()).getTime();
        this.emit('inited');
      } else {
        this.inited = 0;
        this.emit('losed');
      }
    });
  }

  static loadWindowId() {
    return Addon.findWindowId({
      ownerName: 'Skype for Business',
    });
  }

  static monitorWindowId(changed) {
    setImmediate(() => {
      let windowId = SfbWindow.loadWindowId();
      changed(windowId);
      const cb = () => {
        const ret = SfbWindow.loadWindowId();
        if (ret !== windowId) {
          windowId = ret;
          changed(windowId);
        }
      };
      setInterval(cb, 512);
      // Addon.setWinEventHookObjectCreate(cb);
      // Addon.setWinEventHookObjectDestroy(cb);
    });
  }

  show() {
    // if (!this.windowId) { return; }
    // Addon.showWindow(this.windowId);
  }

  hide() {
    // if (!this.windowId) { return; }
    // Addon.hideWindow(this.windowId);
  }

  isVisible() {
    // if (!this.windowId) { return false; }
    // return Addon.isWindowVisible(this.windowId);
  }

  bringToTop() {
    // if (!this.windowId) { return; }
    // Addon.setForegroundWindow(this.windowId);
  }

  isMinimized() {
    if (!this.windowId) { return false; }
    return Addon.isWindowMinimized(this.windowId);
  }

  minimize() {
    // if (!this.windowId) { return; }
    // Addon.minimizeWindow(this.windowId);
  }

  restore() {
    // if (!this.windowId) { return; }
    // Addon.restoreWindow(this.windowId);
  }

  setPosition(x, y) {
    // if (!this.windowId) { return; }
    // Addon.setPosition ?
  }

  getRect() {
    if (!this.windowId) { return null; }
    return Addon.getWindowRect(this.windowId);
  }

  hook() {
    Addon.setWinEventHookLocationChange((windowId) => {
      if (windowId === this.windowId) {
        const rect = this.getRect();
        this.emit('move', rect);
      }
    });
    Addon.setWinEventHookForeground((windowId) => {
      if (!freezeForeground()) {
        this.emit('foreground', windowId);
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
    this.windowId = RcWindow.loadWindowId();
    if (!this.windowId) {
      throw new Error('"windowId" notfound');
    }
    this.inited = (new Date()).getTime();
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
    this.browserWindow.focus();
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
    // if (x === -32000 || y === -32000) { return; }
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
    this.browserWindow.on('show', this.showHandler = () => {
      this.emit('show');
    });
    this.browserWindow.on('hide', this.hideHandler = () => {
      this.emit('hide');
    });
    this.browserWindow.on('move', this.moveHandler = () => {
      const rect = this.getRect();
      this.emit('move', rect);
    });
    this.browserWindow.on('focus', this.focusHandler = () => {
      if (!freezeForeground()) {
        this.emit('foreground');
      }
    });
    this.browserWindow.on('minimize', this.minimizeHandler = () => {
      this.emit('minimize-start');
    });
    this.browserWindow.on('restore', this.restoreHandler = () => {
      this.emit('restore-start');
    });
  }

  unhook() {
    if (this.showHandler) {
      this.browserWindow.removeListener('show', this.showHandler);
      delete this.showHandler;
    }
    if (this.hideHandler) {
      this.browserWindow.removeListener('hide', this.hideHandler);
      delete this.hideHandler;
    }
    if (this.moveHandler) {
      this.browserWindow.removeListener('move', this.moveHandler);
      delete this.moveHandler;
    }
    if (this.focusHandler) {
      this.browserWindow.removeListener('focus', this.focusHandler);
      delete this.focusHandler;
    }
    if (this.minimizeHandler) {
      this.browserWindow.removeListener('minimize', this.minimizeHandler);
      delete this.minimizeHandler;
    }
    if (this.restoreHandler) {
      this.browserWindow.removeListener('restore', this.restoreHandler);
      delete this.restoreHandler;
    }
  }
}

// class
class MacWindow extends Window {
  constructor({ browserWindow }) {
    super();

    this.rcWindow = new RcWindow({ browserWindow });
    this.sfbWindow = new SfbWindow();

    const syncWithSfbPosition = (sfbRect) => {
      const rect = sfbRect || this.sfbWindow.getRect();
      if (rect) {
        this.rcWindow.setPosition(rect.right, rect.top);
      }
    };

    this.sfbWindow.on('foreground', (windowId) => {
      syncWithSfbPosition();
      if (windowId === this.sfbWindow.windowId) {
        if (this.rcWindow.isMinimized()) {
          this.rcWindow.restore();
        } else if (!this.rcWindow.isVisible()) {
          this.rcWindow.show();
        }
      } else if (windowId !== this.rcWindow.windowId) {
        this.rcWindow.hide();
      }
    });

    this.sfbWindow.on('move', (rect) => {
      syncWithSfbPosition(rect);
    });

    this.sfbWindow.on('inited', () => {
      this.sfbWindow.show();
      this.sfbWindow.bringToTop();
      this.rcWindow.show();
      syncWithSfbPosition();
    });

    this.sfbWindow.on('losed', () => {
      this.sfbWindow.hide();
    });
  }

  static get Addon() {
    return Addon;
  }

  bind() {
    this.rcWindow.hook();
    this.sfbWindow.hook();
  }

  unbind() {
    this.rcWindow.unhook();
    this.sfbWindow.unhook();
    Addon.destroy();
  }
}

// exports
module.exports = MacWindow;
