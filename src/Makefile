CC=gcc
CFLAGS= -Wall -g -D DEBUG
LIBS= -lSDL2
SRC= main.c 
OBJS= $(SRC:%.c=%.o)
TARGET= ../FallingSand

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@ $(LIBS)

clear:
	rm $(OBJS) $(TARGET)
