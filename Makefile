objects := rngs.c list.c
install: 
	gcc -Wall -Wextra msq.c -O2 -o msq.o $(objects) -lm
