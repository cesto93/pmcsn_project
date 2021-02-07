objects := rngs.c list.c utils.c transient.c
install: 
	gcc -Wall -Wextra msq.c -O3 -o msq.o $(objects) -lm
	gcc -Wall -Wextra stat.c -O3 -o stat.o utils.c -lm
	
prof: 
	gcc -Wall -Wextra -pg msq.c -O3 -o msq.o $(objects) -lm
