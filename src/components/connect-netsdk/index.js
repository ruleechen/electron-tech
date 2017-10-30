/*
* index
*/

const path = require('path');
const edge = require('electron-edge-js');

let sdkExports;

const getExportFunc = methodName => (
  edge.func({
    assemblyFile: path.resolve(`${__dirname}/output/netsdk.dll`),
    typeName: 'netsdk.exports',
    methodName,
  })
);

const getSdkExports = () => {
  if (!sdkExports) {
    sdkExports = {
      launchLync: getExportFunc('LaunchLync'),
      registerEvents: getExportFunc('RegisterEvents'),
      searchContacts: getExportFunc('SearchContacts'),
      sendMessage: getExportFunc('SendMessage'),
      destroy: getExportFunc('Destroy'),
    };
  }
  return sdkExports;
};

// exports
module.exports = {

  launchLync(callback) {
    return getSdkExports().launchLync({
    }, callback || (() => { }));
  },

  registerEvents({
    appStateChanged,
    accountStateChanged,
    conversationAdded,
    conversationRemoved,
    callback,
  }) {
    return getSdkExports().registerEvents({
      appStateChanged,
      accountStateChanged,
      conversationAdded,
      conversationRemoved,
    }, callback || (() => { }));
  },

  searchContacts(searchText, callback) {
    return getSdkExports().searchContacts({
      searchText,
    }, callback);
  },

  sendMessage(contactUri, message, callback) {
    return getSdkExports().sendMessage({
      contactUri,
      message,
    }, callback);
  },

  destroy(callback) {
    return getSdkExports().destroy({
    }, callback || (() => { }));
  },
};
