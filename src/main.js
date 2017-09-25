const electron = require('electron');
const ipc = require('electron').ipcMain;

// Module to control application life.
const app = electron.app;
// Module to create native browser window.
const BrowserWindow = electron.BrowserWindow;

const path = require('path');
const url = require('url');
const WindowBinding = require('./components/window-binding');

let ConnectNetSdk;
if (WindowBinding.isWindows) {
  ConnectNetSdk = require('./components/connect-netsdk');
}

// Keep a global reference of the window object, if you don't, the window will
// be closed automatically when the JavaScript object is garbage collected.
let mainWindow;
let sidebarWindow;
let aboutWindow;
let cachedSfbHwnd

let mainWindowBinding;
let appTray;
let sidebarWindowReadyToSend;

const initWindowBinding = () => {
  if (mainWindow && sidebarWindow) {
    mainWindowBinding = new WindowBinding.Core({
      browserWindow: mainWindow,
      sidebarWindow,
    }).bind();
  }
};

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
    pathname: path.join(__dirname, 'views/about/index.html'),
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
    pathname: path.join(__dirname, 'views/index/index.html'),
    protocol: 'file:',
    slashes: true,
  }));

  // Open the DevTools.
  // mainWindow.webContents.openDevTools()

  mainWindow.once('ready-to-show', () => {
    if (WindowBinding.isMacOS) {
      mainWindow.show();
    }
    initWindowBinding();
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

  // return;

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
};

const getSidebarRect = () => {
  const size = sidebarWindow.getSize();
  const position = sidebarWindow.getPosition();
  return {
    left: position[0],
    top: position[1],
    right: position[0] + size[0],
    bottom: position[1] + size[1],
  };
}

const syncSidebarRect = (sfbHwnd) => {
  cachedSfbHwnd = sfbHwnd;
  const addon = WindowBinding.Core.Addon;
  const rect = addon.getContactListViewInfo(sfbHwnd || cachedSfbHwnd);
  if (rect) {
    sidebarWindow.setPosition(rect.right - 80, rect.top);
    sidebarWindow.setSize(50, rect.bottom - rect.top);
  }
};

const createShowButtonsFunc = (sfbHwnd) => {
  const addon = WindowBinding.Core.Addon;
  let timeoutId;
  const syncButtons = () => {
    const infos = addon.getContactListItemInfos(sfbHwnd);
    if (!infos.length) {
      sidebarWindow.setPosition(-1000, -1000);
    }
    if (sidebarWindow) {
      syncSidebarRect(sfbHwnd);
      if (sidebarWindowReadyToSend) {
        const sidebarRect = getSidebarRect();
        sidebarWindow.webContents.send('sync-buttons', {
          listItemInfos: infos,
          sidebarRect,
        });
      }
    }
  };
  return () => {
    clearTimeout(timeoutId);
    timeoutId = setTimeout(syncButtons, 16);
  };
};

const bindAutomation = () => {
  console.log('bindAutomation');
  const addon = WindowBinding.Core.Addon;
  const sfbHwnd = addon.findWindowHwnd({
    className: 'CommunicatorMainWindowClass',
    windowName: 'Skype for Business ',
  });
  const showButtons = createShowButtonsFunc(sfbHwnd);
  showButtons();
  addon.initContactListAutomation(sfbHwnd, () => {
    showButtons();
  });
};

let currentAccountState;
if (ConnectNetSdk) {
  ConnectNetSdk.registerEvents({
    appStateChanged: (args) => {
      console.log(args);
    },
    accountStateChanged: (args) => {
      console.log(args);
      currentAccountState = args.AccountState;
      if (currentAccountState === 'SignedIn') {
        bindAutomation();
      }
    },
    conversationAdded: () => {
      console.log('conversation added');
    },
    conversationRemoved: () => {
      console.log('conversation removed');
    },
  });
}

const createSidebarWindow = () => {
  sidebarWindow = new BrowserWindow({
    x: 100,
    y: 100,
    width: 50,
    height: 100,
    show: true,
    frame: false,
    movable: true,
    closable: true,
    resizable: false,
    minimizable: false,
    maximizable: false,
    fullscreenable: false,
    skipTaskbar: true,
    transparent: true,
  });

  sidebarWindow.setMenu(null);
  sidebarWindow.setAlwaysOnTop(true);

  setTimeout(() => {
    sidebarWindow.loadURL(url.format({
      pathname: path.join(__dirname, 'views/sidebar/index.html'),
      protocol: 'file:',
      slashes: true,
    }));
  }, 16);

  sidebarWindow.on('closed', () => {
    sidebarWindow = null;
  });

  sidebarWindow.once('ready-to-show', () => {
    if (currentAccountState === 'SignedIn') {
      sidebarWindow.show();
    } else {
      sidebarWindow.hide();
    }
    initWindowBinding();
  });

  sidebarWindow.webContents.on('did-finish-load', () => {
    sidebarWindowReadyToSend = true;

  })
};

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.on('ready', () => {
  createMainWindow();
  createSidebarWindow();
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
  if (sidebarWindow === null) {
    createSidebarWindow();
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
