CC = cc
CFLAGS = -Wall -Wextra -fPIC
GENERATES = prog prog-a prog-so liboutput_static.a liboutput.so README
TRASH = *.o *~ test-*.out

.PHONY: all test clean

all: $(GENERATES)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

prog.o fun.o: outlib.h

prog: prog.o fun.o const.o
	$(CC) $^ -o $@

liboutput_static.a: fun.o const.o
	ar rcs $@ $^

liboutput.so: fun.o const.o
	$(CC) -shared $^ -o $@

prog-a: prog.o liboutput_static.a
	$(CC) $^ -o $@

prog-so: prog.o liboutput.so
	$(CC) prog.o -L. -loutput -o $@

README: prog
	./prog 2> $@

test: all
	./prog > test-prog-0.out 2>&1
	./prog hello > test-prog-1.out 2>&1
	./prog one two three > test-prog-3.out 2>&1
	./prog-a > test-prog-a-0.out 2>&1
	./prog-a hello > test-prog-a-1.out 2>&1
	./prog-a one two three > test-prog-a-3.out 2>&1
	LD_LIBRARY_PATH=. ./prog-so > test-prog-so-0.out 2>&1
	LD_LIBRARY_PATH=. ./prog-so hello > test-prog-so-1.out 2>&1
	LD_LIBRARY_PATH=. ./prog-so one two three > test-prog-so-3.out 2>&1
	cmp test-prog-0.out test-prog-a-0.out
	cmp test-prog-0.out test-prog-so-0.out
	cmp test-prog-1.out test-prog-a-1.out
	cmp test-prog-1.out test-prog-so-1.out
	cmp test-prog-3.out test-prog-a-3.out
	cmp test-prog-3.out test-prog-so-3.out

clean:
	rm -f $(GENERATES) $(TRASH)
