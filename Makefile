CFLAGS=-std=c11 -g -static -I./include
SRCS=$(wildcard src/*.c)
OBJS=$(SRCS:.c=.o)

9cc: $(OBJS)
	$(CC) -o 9cc $(OBJS) $(LDFLAGS)

$(OBJS): include/9cc.h

test: 9cc
	./test.sh

clean:
	rm -f 9cc src/*.o *~ tmp*

.PHONY: test clean
