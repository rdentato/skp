
skpgen:
	cd examples/skpgen; make

pkg:
	rm -rf dist
	mkdir -p dist/bin
	mkdir -p dist/src
	cp src/skp.[ch] dist/src
	cd examples/skpgen; make -B skpgen
	cp examples/skpgen/skpgen dist/bin

	