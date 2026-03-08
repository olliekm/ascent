CC = gcc
CFLAGS = -Wall -Werror -g -fsanitize=address

# The final program
distributed_ml: main.o server.o worker.o model.o protocol.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

main.o: main.c server.h worker.h protocol.h
	$(CC) $(CFLAGS) -c main.c

server.o: server.c server.h model.h protocol.h
	$(CC) $(CFLAGS) -c server.c

worker.o: worker.c worker.h model.h protocol.h
	$(CC) $(CFLAGS) -c worker.c

model.o: model.c model.h
	$(CC) $(CFLAGS) -c model.c


protocol.o: protocol.c protocol.h
	$(CC) $(CFLAGS) -c protocol.c

clean:
	rm -f *.o distributed_ml

.PHONY: clean
