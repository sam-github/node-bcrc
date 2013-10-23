var bcrc = require('./build/Release/bcrc');

var obj = new bcrc.Basic(8, 0x8005);
console.log( "bcrc", obj );
console.log( "proto", obj.__proto__ );
console.log( "reset", obj.reset() );
console.log( "chain", obj.reset().process("input").checksum() );
