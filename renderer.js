// This file is required by the index.html file and will
// be executed in the renderer process for that window.
// All of the Node.js APIs are available in this process.

const ipc = require('electron').ipcRenderer;

const btnTest = document.getElementById('test');
const btnInteractiveDotNet = document.getElementById('interactiveDotNet');

const showResult = function (html) {
    document.getElementById('result').innerHTML = html;
};

btnTest.addEventListener('click', function (event) {
    ipc.send('async-message', 'ping');
});

ipc.on('reply', function (event, arg) {
    showResult(`async-response: ${JSON.stringify(arg)}`);
});

btnInteractiveDotNet.addEventListener('click', function (event) {
    const reply = ipc.sendSync('sync-message', 'ping');
    showResult(`sync-response: ${JSON.stringify(reply)}`);
});