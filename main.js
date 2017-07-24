const electron = require('electron');
const ipc = require('electron').ipcMain;

// Module to control application life.
const app = electron.app;
// Module to create native browser window.
const BrowserWindow = electron.BrowserWindow;

const path = require('path');
const url = require('url');

// const edge = require('electron-edge');
const dock = require('./dock');
const isWin = /^win/.test(process.platform);

// Keep a global reference of the window object, if you don't, the window will
// be closed automatically when the JavaScript object is garbage collected.
let mainWindow;
let appTray;
let dockWindow;

const createWindow = ({ isDock }) => {
  // Create the browser window.
  mainWindow = new BrowserWindow({
    width: 300,
    height: 600,
    show: false,
    frame: !isDock,
    movable: true,
    closable: true,
    resizable: false,
    minimizable: true,
    maximizable: false,
    fullscreenable: false,
    skipTaskbar: !!isDock,
  });

  // and load the index.html of the app.
  mainWindow.loadURL(url.format({
    pathname: path.join(__dirname, 'index.html'),
    protocol: 'file:',
    slashes: true
  }));

  // Open the DevTools.
  // mainWindow.webContents.openDevTools()

  mainWindow.once('ready-to-show', () => {
    mainWindow.show();
    if (isWin) {
      dockWindow = new dock.WinWindow({
        browserWindow: mainWindow,
      });
      dockWindow.tie();
    }
  });

  // Emitted when the window is closed.
  mainWindow.on('closed', () => {
    // Dereference the window object, usually you would store windows
    // in an array if your app supports multi windows, this is the time
    // when you should delete the corresponding element.
    mainWindow = null;

    // destroy
    if (dockWindow) {
      dockWindow.destroy();
      dockWindow = null;
    }
  });

  appTray = new electron.Tray(isWin ?
    './resource/tray-icon/app-win.ico' :
    './resource/tray-icon/app-mac.png'
  );
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
    }
  ]);
  appTray.setContextMenu(contextMenu);
  appTray.setToolTip('RingCentral for Skype for Business');
  if (!isWin) {
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
  createWindow({
    isDock: true,
  });
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
    createWindow({
      isDock: true,
    });
  }
});

ipc.on('async-message', (event, arg) => {
  // const method = edge.func({
  //   assemblyFile: 'D:/rc/git/favorite/electron-quick-start/csharp/Win32Hook/Win32Hook/bin/Debug/Win32Hook.dll',
  //   typeName: 'Win32Hook.Startup',
  //   methodName: 'Invoke'
  // });
  // method('pong', (error, result) => {
  //   event.sender.send('reply', result);
  // });
  const wrap = isWin ? dock.WinWindow.AddonWrap : dock.MacWindow.AddonWrap;
  wrap.testCallback('rulee', (a, b, c) => {
    event.sender.send('async-message-reply', a + ' ' + b + ' ' + c);
  });
});

ipc.on('sync-message', (event, arg) => {
  const wrap = isWin ? dock.WinWindow.AddonWrap : dock.MacWindow.AddonWrap;
  event.returnValue = wrap.helloWorld();

  if (isWin) {
    const rcHwnd = wrap.findWindowHwnd({
      className: 'Chrome_WidgetWin_1',
      windowName: 'Hello World!',
    });
    const sfbHwnd = wrap.findWindowHwnd({
      className: 'CommunicatorMainWindowClass',
      windowName: 'Skype for Business ',
    });
    const dd0 = wrap.findWindowHwnd({
      className: 'LyncTabFrameHostWindowClass',
      windowName: null,
    });
    const dd1 = wrap.findWindowHwnd({
      className: 'LyncConversationWindowClass',
      windowName: null,
    });
    const dd2 = wrap.findWindowHwnd({
      className: 'NetUIListViewItem',
      windowName: null,
    });
    console.log('found: ' + dd2);
    console.log('rcHwnd: ' + rcHwnd);
    console.log('sfbHwnd: ' + sfbHwnd);
  }
});
