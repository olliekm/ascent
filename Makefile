CC = gcc
CFLAGS = -Wall -Werror -g -fsanitize=address

# The final program
all: server worker

# Sever executable
server: server_main.o server.o model.o protocol.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

# Worker executable
worker: worker_main.o worker.o model.o protocol.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

server_main.o: server_main.c server.h
	$(CC) $(CFLAGS) -c server_main.c

server.o: server.c server.h model.h protocol.h
	$(CC) $(CFLAGS) -c server.c

worker_main.o: worker_main.c worker.h model.h
	$(CC) $(CFLAGS) -c worker_main.c

worker.o: worker.c worker.h model.h protocol.h
	$(CC) $(CFLAGS) -c worker.c

model.o: model.c model.h
	$(CC) $(CFLAGS) -c model.c

protocol.o: protocol.c protocol.h
	$(CC) $(CFLAGS) -c protocol.c

clean:
	rm -f *.o server worker distributed_ml

.PHONY: all clean