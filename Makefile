CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

dmake:
	 docker run --rm -v ~/Programming/mayocc:/mayocc -w /mayocc compilerbook make mayocc

dtest: mayocc
	docker run --rm -v ~/Programming/mayocc:/mayocc -w /mayocc compilerbook make test

mayocc: $(OBJS)
	$(CC) -o mayocc $(OBJS) $(LDFLAGS)

$(OBJS): mayocc.h

test: mayocc
	./test.sh

clean:
	rm -f mayocc *.o *~ tmp*

.PHONY: test clean dtest dmake