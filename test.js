var bcrc = require('./build/Release/bcrc');

var obj = new bcrc.Basic(8, 0x8005);
console.dir( obj );
console.log( obj.method() );
console.log( obj.method().method() );
