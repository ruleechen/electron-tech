/*
* index
*/

const ipc = require('electron').ipcRenderer;

const btnSync = document.getElementById('btnSync');
const btnAsync = document.getElementById('btnAsync');

const showResult = (html) => {
  document.getElementById('result').innerHTML = html;
};

btnAsync.addEventListener('click', () => {
  ipc.send('async-message', 'ping');
});
ipc.on('async-message-reply', (ev, arg) => {
  showResult(`async-response: ${JSON.stringify(arg)}`);
});

btnSync.addEventListener('click', () => {
  const reply = ipc.sendSync('sync-message', 'ping');
  showResult(`sync-response: ${JSON.stringify(reply)}`);
});
