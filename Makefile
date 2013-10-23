do:
	node-gyp configure
	make -C build all
	node test.js
