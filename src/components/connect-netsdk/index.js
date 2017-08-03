/*
* index
*/

const edge = require('electron-edge');

let sdkExports;

const getExportFunc = methodName => (
  edge.func({
    assemblyFile: `${__dirname}/output/netsdk.dll`,
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

module.exports = {

  searchContacts(searchText) {
    return getSdkExports().searchContacts(searchText);
  },

  sendMessage(contact, messagae) {
    return getSdkExports().sendMessage(contact, messagae);
  },
};
