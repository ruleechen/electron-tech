/*
* index
*/

const path = require('path');
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
      searchContacts: getExportFunc('SearchContacts'),
      sendMessage: getExportFunc('SendMessage'),
    };
  }
  return sdkExports;
};

// exports
module.exports = {

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
};
