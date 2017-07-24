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
let dockWindow;

function createWindow() {
  // Create the browser window.
  mainWindow = new BrowserWindow({
    width: 300,
    height: 600,
    show: false,
    frame: false,
    movable: false,
    closable: false,
    resizable: false,
    minimizable: true,
    maximizable: false,
    fullscreenable: false,
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
  mainWindow.on('closed', function () {
    // Dereference the window object, usually you would store windows
    // in an array if your app supports multi windows, this is the time
    // when you should delete the corresponding element.
    mainWindow = null;

    // destroy
    if (dockWindow) {
      dockWindow.destroy();
    }
  });
}

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.on('ready', createWindow);

// Quit when all windows are closed.
app.on('window-all-closed', function () {
  // On OS X it is common for applications and their menu bar
  // to stay active until the user quits explicitly with Cmd + Q
  if (process.platform !== 'darwin') {
    app.quit();
  }
});

app.on('activate', function () {
  // On OS X it's common to re-create a window in the app when the
  // dock icon is clicked and there are no other windows open.
  if (mainWindow === null) {
    createWindow();
  }
});

ipc.on('async-message', function (event, arg) {
  // const method = edge.func({
  //   assemblyFile: 'D:/rc/git/favorite/electron-quick-start/csharp/Win32Hook/Win32Hook/bin/Debug/Win32Hook.dll',
  //   typeName: 'Win32Hook.Startup',
  //   methodName: 'Invoke'
  // });
  // method('pong', function (error, result) {
  //   event.sender.send('reply', result);
  // });
  const wrap = isWin ? dock.WinWindow.AddonWrap : dock.MacWindow.AddonWrap;
  wrap.testCallback('rulee', function (a, b, c) {
    event.sender.send('async-message-reply', a + ' ' + b + ' ' + c);
  });
});

ipc.on('sync-message', function (event, arg) {
  const wrap = isWin ? dock.WinWindow.AddonWrap : dock.MacWindow.AddonWrap;
  event.returnValue = wrap.helloWorld();

  if (isWin) {
    var rcHwnd = wrap.findWindowHwnd({
      className: 'Chrome_WidgetWin_1',
      windowName: 'Hello World!',
    });
    var sfbHwnd = wrap.findWindowHwnd({
      className: 'CommunicatorMainWindowClass',
      windowName: 'Skype for Business ',
    });
    var dd0 = wrap.findWindowHwnd({
      className: 'LyncTabFrameHostWindowClass',
      windowName: null,
    });
    var dd1 = wrap.findWindowHwnd({
      className: 'LyncConversationWindowClass',
      windowName: null,
    });
    var dd2 = wrap.findWindowHwnd({
      className: 'NetUIListViewItem',
      windowName: null,
    });
    console.log('found: ' + dd2);
    console.log('rcHwnd: ' + rcHwnd);
    console.log('sfbHwnd: ' + sfbHwnd);
  }
});

