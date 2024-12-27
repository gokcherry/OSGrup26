grup26: grup26.o
	gcc -o grup26 grup26.c

grup26.o: grup26.c grup26.h
	gcc -Wall -Wextra -std=c11 -c grup26.c
