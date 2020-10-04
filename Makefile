CC = gcc
CCFLAGS =  -O2  -Wall   -pedantic
NAME = os_lab1

all: 
	$(CC) $(CCFLAGS) main.c -o $(NAME) -lm
debug: 
	$(CC) $(CCFLAGS) -g3 main.c -o $(NAME) -lm
clean:
	rm -f *.o $(NAME)