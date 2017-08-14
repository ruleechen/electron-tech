/*
* windows window
*/

const EventEmitter = require('events');
const Window = require('./window');
const Addon = require('./helpers/addon_win');
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
    return Addon.findWindowHwnd({
      className: 'CommunicatorMainWindowClass',
      windowName: 'Skype for Business ',
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
      Addon.setWinEventHookObjectCreate(null, cb);
      Addon.setWinEventHookObjectDestroy(null, cb);
    });
  }

  show() {
    if (!this.windowId) { return; }
    Addon.showWindow(this.windowId);
  }

  hide() {
    if (!this.windowId) { return; }
    Addon.hideWindow(this.windowId);
  }

  isVisible() {
    if (!this.windowId) { return false; }
    return Addon.isWindowVisible(this.windowId);
  }

  bringToTop() {
    if (!this.windowId) { return; }
    Addon.setForegroundWindow(this.windowId);
  }

  isMinimized() {
    if (!this.windowId) { return false; }
    return Addon.isWindowMinimized(this.windowId);
  }

  minimize() {
    if (!this.windowId) { return; }
    Addon.minimizeWindow(this.windowId);
  }

  restore() {
    if (!this.windowId) { return; }
    Addon.restoreWindow(this.windowId);
  }

  setRect(rect) {
    if (!this.windowId) { return; }
    Addon.setWindowRect(this.windowId, rect.left, rect.top, rect.right, rect.bottom);
  }

  getRect() {
    if (!this.windowId) { return null; }
    return Addon.getWindowRect(this.windowId);
  }

  hook() {
    Addon.setWinEventHookObjectShow(this.windowId, (windowId) => {
      if (windowId === this.windowId) {
        this.emit('show');
      }
    });
    Addon.setWinEventHookObjectHide(this.windowId, (windowId) => {
      if (windowId === this.windowId) {
        this.emit('hide');
      }
    });
    Addon.setWinEventHookMinimizeStart(this.windowId, (windowId) => {
      if (windowId === this.windowId) {
        this.emit('minimize-start');
      }
    });
    Addon.setWinEventHookMinimizeEnd(this.windowId, (windowId) => {
      if (windowId === this.windowId) {
        this.emit('restore-start');
      }
    });
    Addon.setWinEventHookLocationChange(null, (windowId) => {
      if (windowId === this.windowId) {
        const rect = this.getRect();
        this.emit('move', rect);
      }
    });
    Addon.setWinEventHookForeground(null, (windowId) => {
      if (windowId !== this.windowId) {
        return;
      }
      if (!freezeForeground()) {
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
    this.windowId = RcWindow.loadWindowId();
    if (!this.windowId) {
      throw new Error('"windowId" notfound');
    }
    this.inited = (new Date()).getTime();
  }

  static loadWindowId() {
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

  bringToTop(focus) {
    this.browserWindow.setAlwaysOnTop(true);
    if (focus) { this.browserWindow.focus(); }
    this.browserWindow.setAlwaysOnTop(false);
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

  setRect(rect) {
    this.browserWindow.setPosition(rect.left, rect.top);
    this.browserWindow.setSize(rect.right - rect.left, rect.bottom - rect.top);
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
    Addon.setWinEventHookForeground(null, (windowId) => {
      if (windowId !== this.windowId) {
        return;
      }
      if (!freezeForeground()) {
        this.emit('foreground');
      }
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
class WinWindow extends Window {
  constructor({ browserWindow }) {
    super();
    this.rcWindow = new RcWindow({ browserWindow });
    this.sfbWindow = new SfbWindow();
  }

  static get Addon() {
    return Addon;
  }

  syncWithSfbPosition(sfbRect) {
    const rect = sfbRect || this.sfbWindow.getRect();
    if (rect) {
      this.rcWindow.setRect({
        left: rect.left,
        right: rect.right,
        top: rect.bottom,
        bottom: rect.bottom + 100,
      });
    }
  }

  onSfbInited() {
    this.sfbWindow.on('show', () => {
      this.rcWindow.show();
    });
    this.sfbWindow.on('hide', () => {
      // right after sfb is hidden --> rc become foreground --> show sfb again
      // so here we need the freezeForeground before hide sfb
      freezeForeground();
      this.rcWindow.hide();
    });

    this.rcWindow.on('minimize-start', () => {
      // right after sfb is hidden --> rc become foreground --> show sfb again
      // so here we need the freezeForeground before hide sfb
      freezeForeground();
    });
    this.sfbWindow.on('minimize-start', () => {
      this.rcWindow.minimize();
    });
    this.sfbWindow.on('restore-start', () => {
      this.rcWindow.restore();
    });

    this.rcWindow.on('foreground', () => {
      this.syncWithSfbPosition();
      if (this.sfbWindow.isMinimized()) {
        this.sfbWindow.restore();
        return;
      }
      if (!this.sfbWindow.isVisible()) {
        this.sfbWindow.show();
        return;
      }
      this.sfbWindow.bringToTop();
      this.rcWindow.bringToTop(true);
    });
    this.sfbWindow.on('foreground', () => {
      this.syncWithSfbPosition();
      if (!this.setForegroundWindowAllowed) {
        // The calling process must already be able to set the foreground window
        // So here we use sfb process to enable the rc electron process
        Addon.allowSetForegroundWindow(this.rcWindow.windowId);
        this.setForegroundWindowAllowed = true;
      }
      if (this.rcWindow.isMinimized()) {
        this.rcWindow.restore();
        return;
      }
      if (!this.rcWindow.isVisible()) {
        this.rcWindow.show();
        return;
      }
      this.rcWindow.bringToTop(false);
      this.sfbWindow.bringToTop();
    });

    this.sfbWindow.on('move', (rect) => {
      this.syncWithSfbPosition(rect);
    });
  }

  onSfbLost() {
    this.sfbWindow.hide();
  }

  bind() {
    this.sfbWindow.on('inited', () => {
      this.rcWindow.hook();
      this.sfbWindow.hook();
      this.onSfbInited();
      this.sfbWindow.show();
      this.sfbWindow.bringToTop();
      this.rcWindow.show();
      this.syncWithSfbPosition();
    });

    this.sfbWindow.on('losed', () => {
      this.rcWindow.unhook();
      this.sfbWindow.unhook();
      this.onSfbLost();
    });

    return this;
  }

  unbind() {
    this.rcWindow.unhook();
    this.sfbWindow.unhook();
    Addon.destroy();
    return this;
  }
}

// exports
module.exports = WinWindow;
