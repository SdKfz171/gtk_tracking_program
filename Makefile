CC = gcc

OBJS = main.o
CFLAGS = $(shell pkg-config --cflags gtk+-3.0 json-c)
LDFLAGS = $(shell pkg-config --libs gtk+-3.0 json-c) -lcurl -lsqlite3

# SRCS = $(OBJS:.o=.c)

main : $(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS) $(LDFLAGS)

clean :
	rm *.o main