CFLAGS=-std=c11 -g -static

mayocc: mayocc.c
	 docker run --rm -v ~/Programming/mayocc:/mayocc -w /mayocc compilerbook cc -std=c11 -g -static mayocc.c -o mayocc
test: mayocc
	docker run --rm -v ~/Programming/mayocc:/mayocc -w /mayocc compilerbook ./test.sh

clean:
	rm -f mayocc *.o *~ tmp*

.PHONY: test clean