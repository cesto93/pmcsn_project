objects := rngs.c list.c
install: 
	gcc -Wall -Wextra msq.c -O2 -o msq.o $(objects) -lm
	gcc -Wall -Wextra stat.c -O2 -o stat.o 
