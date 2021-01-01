CFLAGS = -std=c17 -Wall -Wextra -Wdouble-promotion -Werror=pedantic -Werror=vla -pedantic-errors -Wfatal-errors -flto -Ofast -march=native -mtune=native
CC = gcc

all: main.o dynamic_arrays.o data.o module_c.o module_f.o module_t.o
	$(CC) $(CFLAGS) -o shafa $^

module_f.o: data.c data.h dynamic_arrays.h dynamic_arrays.c module_f.h module_f.c data.h data.c

run: all
	./shafa

t: module_t.c module_t.h dynamic_arrays.c dynamic_arrays.h data.h data.c
	$(CC) $(CFLAGS) module_t.h dynamic_arrays.h dynamic_arrays.c data.h data.c -o $@ $<
clean:
	@rm -rf *.o c t f
