const electron = require('electron');
const ipc = require('electron').ipcMain;

// Module to control application life.
const app = electron.app;
// Module to create native browser window.
const BrowserWindow = electron.BrowserWindow;

const path = require('path');
const url = require('url');
const WindowBinding = require('./src/components/window-binding');

// Keep a global reference of the window object, if you don't, the window will
// be closed automatically when the JavaScript object is garbage collected.
let mainWindow;
let mainWindowBinding;
let appTray;

const createWindow = () => {
  // Create the browser window.
  mainWindow = new BrowserWindow({
    width: 300,
    height: 600,
    show: false,
    frame: true,
    movable: true,
    closable: true,
    resizable: false,
    minimizable: true,
    maximizable: false,
    fullscreenable: false,
    skipTaskbar: false,
  });

  // and load the index.html of the app.
  mainWindow.loadURL(url.format({
    pathname: path.join(__dirname, 'src/view/index/index.html'),
    protocol: 'file:',
    slashes: true,
  }));

  // Open the DevTools.
  // mainWindow.webContents.openDevTools()

  mainWindow.once('ready-to-show', () => {
    mainWindow.show();
    mainWindowBinding = new WindowBinding.Core({
      browserWindow: mainWindow,
    });
    mainWindowBinding.bind();
  });

  // Emitted when the window is closed.
  mainWindow.on('closed', () => {
    // Dereference the window object, usually you would store windows
    // in an array if your app supports multi windows, this is the time
    // when you should delete the corresponding element.
    mainWindow = null;

    // destroy
    if (mainWindowBinding) {
      mainWindowBinding.unbind();
      mainWindowBinding = null;
    }
  });

  const iconPath = WindowBinding.isWindows ?
    './src/resources/tray-icon/app-win.ico' :
    './src/resources/tray-icon/app-mac.png';
  appTray = new electron.Tray(iconPath);
  const contextMenu = electron.Menu.buildFromTemplate([
    {
      label: 'About',
      click() {
        console.log('About Clicked');
      },
    },
    {
      label: 'Quit',
      role: 'quit',
    },
  ]);
  appTray.setContextMenu(contextMenu);
  appTray.setToolTip('RingCentral for Skype for Business');
  if (WindowBinding.isMacOS) {
    mainWindow.on('show', () => {
      appTray.setHighlightMode('always');
    });
    mainWindow.on('hide', () => {
      appTray.setHighlightMode('never');
    });
  }
}

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.on('ready', () => {
  createWindow();
});

// Quit when all windows are closed.
app.on('window-all-closed', () => {
  // On OS X it is common for applications and their menu bar
  // to stay active until the user quits explicitly with Cmd + Q
  if (process.platform !== 'darwin') {
    app.quit();
  }
});

app.on('activate', () => {
  // On OS X it's common to re-create a window in the app when the
  // dock icon is clicked and there are no other windows open.
  if (mainWindow === null) {
    createWindow();
  }
});

ipc.on('async-message', (event, arg) => {
  const addon = WindowBinding.Core.Addon;
  addon.testCallback('rulee', (a, b, c) => {
    event.sender.send('async-message-reply', `${a} ${b} ${c}`);
  });
});

ipc.on('sync-message', (event, arg) => {
  const addon = WindowBinding.Core.Addon;
  event.returnValue = addon.helloWorld();

  if (WindowBinding.isWindows) {
    const rcHwnd = addon.findWindowHwnd({
      className: 'Chrome_WidgetWin_1',
      windowName: 'RingCentral for Skype for Business',
    });
    const sfbHwnd = addon.findWindowHwnd({
      className: 'CommunicatorMainWindowClass',
      windowName: 'Skype for Business ',
    });
    const dd0 = addon.findWindowHwnd({
      className: 'LyncTabFrameHostWindowClass',
      windowName: null,
    });
    const dd1 = addon.findWindowHwnd({
      className: 'LyncConversationWindowClass',
      windowName: null,
    });
    const dd2 = addon.findWindowHwnd({
      className: 'NetUIListViewItem',
      windowName: null,
    });
    console.log(`found: ${dd2}`);
    console.log(`rcHwnd: ${rcHwnd}`);
    console.log(`sfbHwnd: ${sfbHwnd}`);
  }
  if (WindowBinding.isMacOS) {
    const windowId = addon.findWindowId({
      ownerName: 'Skype for Business',
    });
    console.log(windowId);
  }
});
