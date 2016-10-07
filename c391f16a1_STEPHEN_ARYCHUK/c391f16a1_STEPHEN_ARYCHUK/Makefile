CC = gcc
CFLAGS = -lpthread -ldl

all: q2 q3 q4 q5 q6 q7 q8 q9

q2: 
	$(CC) -g q2.c -o q2 sqlite3.c $(CFLAGS)

q3: 
	$(CC) -g q3.c -o q3 sqlite3.c $(CFLAGS)

q4: 
	$(CC) -g q4.c -o q4 sqlite3.c -lm $(CFLAGS)

q5:
	$(CC) -g q5.c -o q5 sqlite3.c -lm $(CFLAGS)

q6: 
	$(CC) -g q6.c -o q6 sqlite3.c $(CFLAGS)

q7: 
	$(CC) -g q7.c -o q7 sqlite3.c $(CFLAGS)

q8: 
	$(CC) -g q8.c -o q8 sqlite3.c $(CFLAGS)

q9: 
	$(CC) -g q9.c -o q9 sqlite3.c $(CFLAGS)

clean:
	rm -f q2 q3 q4 q5 q6 q7 q8 q9 *.o *~