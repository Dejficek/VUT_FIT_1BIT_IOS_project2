CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic -pthread

all: proj2.c
	gcc $(CFLAGS) -o proj2 proj2.c
clean:
	rm proj2