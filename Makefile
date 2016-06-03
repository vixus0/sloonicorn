sloo: sl.c sloonicorn.c
	gcc -Wall -Werror -lm -llo -o $@ $^

clean:
	rm ./sloo
