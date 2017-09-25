/*
* button
*/

const ipc = require('electron').ipcRenderer;

const createButton = (contactName) => {
  const div = document.createElement('div');
  div.className = 'circle';
  div.style.left = '0';
  div.addEventListener('click', () => {
    alert(contactName);
  });
  document.body.appendChild(div);
  return div;
};

ipc.on('sync-buttons', (event, { sidebarRect, listItemInfos }) => {
  const curButtons = window.sfbButtons || {};
  const newButtons = {};
  if (listItemInfos.length) {
    const baseTop = listItemInfos[0].top;
    const scorll = sidebarRect.top - baseTop;
    listItemInfos.forEach((item) => {
      if (item.name && item.contactName) {
        let btn = curButtons[item.name];
        if (!btn) { btn = createButton(item.contactName); }
        const top = item.top - baseTop - scorll;
        btn.style.top = `${top}px`;
        newButtons[item.name] = btn;
      }
    });
  }
  Object.keys(curButtons).forEach((key) => {
    if (!newButtons[key]) {
      const btn = curButtons[key];
      btn.parentNode.removeChild(btn);
    }
    delete curButtons[key];
  });
  window.sfbButtons = newButtons;
});
