/*
* index
*/

const path = require('path');
const EventEmitter = require('events');
const edge = require('electron-edge');

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
      registerEvents: getExportFunc('RegisterEvents'),
      searchContacts: getExportFunc('SearchContacts'),
      sendMessage: getExportFunc('SendMessage'),
      destroy: getExportFunc('Destroy'),
    };
  }
  return sdkExports;
};

const emitter = new EventEmitter();
const registerEvent = (key, callback) => {
  if (!emitter.listeners(key).length) {
    getSdkExports().registerEvents({
      [key]: (...args) => {
        emitter.emit(key, ...args);
      },
    }, () => { });
  }
  emitter.on(key, callback);
}

// exports
module.exports = {

  stateChanged(callback) {
    registerEvent('stateChanged', callback);
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
