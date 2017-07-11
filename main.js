const electron = require('electron');
const ipc = require('electron').ipcMain;
const edge = require('electron-edge');
const dock = require('./dock');

// Module to control application life.
const app = electron.app;
// Module to create native browser window.
const BrowserWindow = electron.BrowserWindow;

const path = require('path');
const url = require('url');

// Keep a global reference of the window object, if you don't, the window will
// be closed automatically when the JavaScript object is garbage collected.
let mainWindow;

function createWindow() {
  // Create the browser window.
  mainWindow = new BrowserWindow({
    width: 800,
    height: 600
  });

  // and load the index.html of the app.
  mainWindow.loadURL(url.format({
    pathname: path.join(__dirname, 'index.html'),
    protocol: 'file:',
    slashes: true
  }));

  // Open the DevTools.
  // mainWindow.webContents.openDevTools()

  // Emitted when the window is closed.
  mainWindow.on('closed', function () {
    // Dereference the window object, usually you would store windows
    // in an array if your app supports multi windows, this is the time
    // when you should delete the corresponding element.
    mainWindow = null
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

// In this file you can include the rest of your app's specific main process
// code. You can also put them in separate files and require them here.

ipc.on('async-message', function (event, arg) {
  var method = edge.func({
    assemblyFile: 'D:/rc/git/favorite/electron-quick-start/csharp/Win32Hook/Win32Hook/bin/Debug/Win32Hook.dll',
    typeName: 'Win32Hook.Startup',
    methodName: 'Invoke'
  });
  method('pong', function (error, result) {
    event.sender.send('reply', result);
  });
});

ipc.on('sync-message', function (event, arg) {
  var hwnd = dock.WinWindow.findWindowHwnd("CommunicatorMainWindowClass");
  // var rect = dock.WinWindow.getWindowRect(hwnd);
  // dock.WinWindow.setForegroundWindow(hwnd);
  // var json = JSON.stringify(rect);
  event.returnValue = dock.WinWindow.isWindowVisible(hwnd);
  mainWindow.setPosition(100, 100);
});

// setTimeout(function () {
//   const hwnd1 = dock.WinWindow.findWindowHwnd("CommunicatorMainWindowClass");
//   const hwnd2 = dock.WinWindow.findWindowHwnd(null, "RingCentral for Skype for Business");

//   if (hwnd1 && hwnd2) {
//     const window1 = new dock.WinWindow(hwnd1);
//     const window2 = new dock.WinWindow(hwnd2);
//     window1.dockIn(window2);
//   }
// }, 1000);
