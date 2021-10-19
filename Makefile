CFLAGS=-std=c11 -g -static

dmake: mayocc.c
	 docker run --rm -v ~/Programming/mayocc:/mayocc -w /mayocc compilerbook make mayocc

dtest: mayocc
	docker run --rm -v ~/Programming/mayocc:/mayocc -w /mayocc compilerbook make test

mayocc: mayocc.c

test: mayocc
	./test.sh

clean:
	rm -f mayocc *.o *~ tmp*

.PHONY: test clean