CC = gcc
LIBS = $(shell pkg-config --cflags --libs liblo)

sloo: sl.c sloonicorn.c
	$(CC) -Wall -Werror -lm $(LIBS) -o $@ $^

clean:
	rm ./sloo
