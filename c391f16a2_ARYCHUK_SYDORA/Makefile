CC = gcc
CFLAGS = -lpthread -ldl

all: q0 q4 q5 q7 q8

q0:
	$(CC) -g q0.c -o q0 sqlite3.c $(CFLAGS)

q4: 
	$(CC) -g q4.c -o q4 sqlite3.c -lm $(CFLAGS)

q5:
	$(CC) -g q5.c -o q5 sqlite3.c -lm $(CFLAGS)

q7: 
	$(CC) -g q7.c -o q7 sqlite3.c $(CFLAGS)

q8: 
	$(CC) -g q8.c -o q8 sqlite3.c $(CFLAGS)

clean:
	rm -f q0 q4 q5 q7 q8 *.o *~
