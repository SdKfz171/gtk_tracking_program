CC = gcc

OBJS = main.o
OBJS1 = list.o element.o
CFLAGS = $(shell pkg-config --cflags gtk+-3.0 json-c)
LDFLAGS = $(shell pkg-config --libs gtk+-3.0 json-c) -lcurl -lsqlite3

main : $(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS) $(LDFLAGS)

element : $(OBJS1)
	$(CC) -o $@ $(OBJS1) $(CFLAGS) $(LDFLAGS)

clean :
	rm *.o main element