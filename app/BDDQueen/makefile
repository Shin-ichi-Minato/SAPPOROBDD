all: 
	gcc -Wall -Wextra -Wshadow -DB_64 -O3 -I../../include bddqueen.c ../../lib/BDD64.a -o bddqueen

32: 
	gcc -Wall -Wextra -Wshadow -O3 -I../../include bddqueen.c ../../lib/BDD32.a -o bddqueen

clean:
	rm -f *.o *.exe *~

