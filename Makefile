CC = gcc
LIBS = $(shell pkg-config --cflags --libs liblo)

sloo: mh.c sl.c sloonicorn.c
	$(CC) -Wall -Werror -lm $(LIBS) -o $@ $^

clean:
	rm ./sloo
