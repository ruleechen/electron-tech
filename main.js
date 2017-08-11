const electron = require('electron');
const ipc = require('electron').ipcMain;

// Module to control application life.
const app = electron.app;
// Module to create native browser window.
const BrowserWindow = electron.BrowserWindow;

const path = require('path');
const url = require('url');
const WindowBinding = require('./src/components/window-binding');

let ConnectNetSdk;
if (WindowBinding.isWindows) {
  ConnectNetSdk = require('./src/components/connect-netsdk');
  ConnectNetSdk.registerEvents({
    appStateChanged: (args) => {
      console.log(args);
    },
    accountStateChanged: (args) => {
      console.log(args);
    },
    conversationAdded: () => {
      console.log('conversation added');
    },
    conversationRemoved: () => {
      console.log('conversation removed');
    },
  });
}

// Keep a global reference of the window object, if you don't, the window will
// be closed automatically when the JavaScript object is garbage collected.
let mainWindow;
let aboutWindow;

let mainWindowBinding;
let appTray;

const createAboutWindow = () => {
  if (aboutWindow) {
    aboutWindow.close();
  }

  aboutWindow = new BrowserWindow({
    width: 400,
    height: 250,
    show: true,
    frame: true,
    movable: true,
    closable: true,
    resizable: false,
    minimizable: false,
    maximizable: false,
    fullscreenable: false,
    skipTaskbar: true,
  });

  aboutWindow.setMenu(null);
  aboutWindow.setAlwaysOnTop(true);

  aboutWindow.loadURL(url.format({
    pathname: path.join(__dirname, 'src/views/about/index.html'),
    protocol: 'file:',
    slashes: true,
  }));

  aboutWindow.on('closed', () => {
    aboutWindow = null;
  });
};

const createMainWindow = () => {
  // Create the browser window.
  mainWindow = new BrowserWindow({
    width: 500,
    height: 100,
    show: false,
    frame: false,
    movable: true,
    closable: true,
    resizable: false,
    minimizable: true,
    maximizable: false,
    fullscreenable: false,
    skipTaskbar: true,
  });

  mainWindow.setMenu(null);

  // and load the index.html of the app.
  mainWindow.loadURL(url.format({
    pathname: path.join(__dirname, 'src/views/index/index.html'),
    protocol: 'file:',
    slashes: true,
  }));

  // Open the DevTools.
  // mainWindow.webContents.openDevTools()

  mainWindow.once('ready-to-show', () => {
    if (WindowBinding.isMacOS) {
      mainWindow.show();
    }
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
    if (ConnectNetSdk) {
      try {
        ConnectNetSdk.destroy();
      } catch (ex) {
        console.error(ex);
      }
    }
  });

  const iconPath = WindowBinding.isWindows ?
    './src/resources/tray-icon/app-win.ico' :
    './src/resources/tray-icon/app-mac.png';
  appTray = new electron.Tray(iconPath);

  appTray.setToolTip('RingCentral for Skype for Business');

  appTray.on('double-click', () => {
    mainWindow.show();
  });

  appTray.setContextMenu(electron.Menu.buildFromTemplate([
    {
      label: 'About',
      click() {
        createAboutWindow();
      },
    },
    {
      label: 'Show',
      click() {
        mainWindow.show();
      },
    },
    {
      label: 'Quit',
      role: 'quit',
    },
  ]));

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
  createMainWindow();
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
    createMainWindow();
  }
});

ipc.on('async-message', (event, arg) => {
  // const addon = WindowBinding.Core.Addon;
  // addon.testCallback('rulee', (a, b, c) => {
  //   event.sender.send('async-message-reply', `${a} ${b} ${c}`);
  // });
  ConnectNetSdk.searchContacts('demo', (err, res) => {
    event.sender.send('async-message-reply', JSON.stringify(err || res));
  });
});

const buttons = [];
const showButtons = (itemInfos) => {
  // TODO:
};

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
    addon.initContactListAutomation(sfbHwnd, () => {
      const infos = addon.getContactListItemInfos(sfbHwnd);
      showButtons(infos);
      const infosJson = JSON.stringify(infos);
      console.log(`rects sub: ${infosJson}`);
    });
    const itemInfos = addon.getContactListItemInfos(sfbHwnd);
    showButtons(itemInfos);
    const itemInfosJson = JSON.stringify(itemInfos);
    console.log(`found: ${dd2}`);
    console.log(`rcHwnd: ${rcHwnd}`);
    console.log(`sfbHwnd: ${sfbHwnd}`);
    console.log(`rects: ${itemInfosJson}`);
  }
  if (WindowBinding.isMacOS) {
    const windowId = addon.findWindowId({
      ownerName: 'Skype for Business',
    });
    const minimized = addon.isWindowMinimized(windowId);
    const rect = addon.getWindowRect(windowId);
    console.log(`windowId: ${windowId}`);
    console.log(`minimized: ${minimized}`);
    console.log(`rect: ${JSON.stringify(rect)}`);
    setTimeout(() => {
      const setted = addon.setForegroundWindow(windowId);
      console.log(`foreground: ${setted}`);
    }, 1000);

    addon.setWinEventHookForeground(null, (pid) => {
      console.log(`pid: ${pid}`);
      const rect1 = addon.getWindowRect(windowId);
      console.log(`rect: ${JSON.stringify(rect1)}`);
    });
  }
});
