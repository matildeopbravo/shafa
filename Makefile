CFLAGS = -std=c17 -Wall -Wextra -Wdouble-promotion -Werror=pedantic -Werror=vla -pedantic-errors -Wfatal-errors
OPT = -flto -Ofast -march=native -mtune=native
CC = gcc

all: shafa.o dynamic_arrays.o data.o module_c.o module_f.o module_t.o
	$(CC) $(CFLAGS) $(OPT) -o shafa $^

clean:
	@rm -rf *.o c t f
