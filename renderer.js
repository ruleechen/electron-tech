// This file is required by the index.html file and will
// be executed in the renderer process for that window.
// All of the Node.js APIs are available in this process.

const ipc = require('electron').ipcRenderer;

const btnSync = document.getElementById('btnSync');
const btnAsync = document.getElementById('btnAsync');

const showResult = function (html) {
    document.getElementById('result').innerHTML = html;
};

btnAsync.addEventListener('click', function (ev) {
    ipc.send('async-message', 'ping');
});
ipc.on('async-message-reply', function (ev, arg) {
    showResult(`async-response: ${JSON.stringify(arg)}`);
});

btnSync.addEventListener('click', function (ev) {
    const reply = ipc.sendSync('sync-message', 'ping');
    showResult(`sync-response: ${JSON.stringify(reply)}`);
});
