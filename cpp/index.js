/*
* package index
*/

var addon = require('bindings')('rcsfb.node');

module.exports = {
    getWindowRect: function (threadName) {
        var ret = addon.getWindowRect(threadName);
        var parts = ret.split('|');
        return {
            left: parts[0],
            top: parts[1],
            right: parts[2],
            bottom: parts[3]
        };
    }
};
