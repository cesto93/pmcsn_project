objects := rngs.c utils/list.c utils/utils.c utils/csv.c transient.c steady.c 
objects_stat := utils/utils.c utils/csv.c
install: 
	gcc -Wall -Wextra msq.c -O3 -o msq.o $(objects) -lm
	gcc -Wall -Wextra stat.c -O3 -o stat.o $(objects_stat)  -lm
	
prof: 
	gcc -Wall -Wextra -pg msq.c -O3 -o msq.o $(objects) -lm
