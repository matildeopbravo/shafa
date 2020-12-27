CFLAGS = -std=c17 -Wall -Wextra -Wdouble-promotion -Werror=pedantic -Werror=vla -pedantic-errors -Wfatal-errors -flto -Ofast -march=native -mtune=native
CC = gcc

all: main.o dynamic_arrays.o module_c.o
	$(CC) $(CFLAGS) -o shafa $^

run: all
	./shafa

f: module_f.c module_f.h dynamic_arrays.c dynamic_arrays.h
	$(CC) $(CFLAGS) module_f.h dynamic_arrays.h dynamic_arrays.c -o $@ $<

t: module_t.c
	$(CC) $(CFLAGS) -o $@ $<
clean:
	@rm -rf *.o c t f
