/*
* index
*/

const path = require('path');
const edge = require('electron-edge');

const assemblyFile = path.resolve('./src/output/netsdk.dll');

const getPresenseFunc = edge.func({
  assemblyFile,
  typeName: 'Win32Hook.Startup',
  methodName: 'Invoke',
});

module.exports = {

  connect(userName, password) {

  },

  getPresense() {
    return getPresenseFunc();
  },

  searchContacts(searchText) {

  },

  sendMessage(messagae) {

  },
};
