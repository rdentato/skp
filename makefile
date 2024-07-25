

distr:
	rm -rf distr
	mkdir -p distr/bin
	mkdir -p distr/src
	cp src/skp.[ch] distr/src

deps/tng: deps/tng.c
	cd deps; $(CC) -O3 -Wall -o tng tng.c 
	strip deps/tng

