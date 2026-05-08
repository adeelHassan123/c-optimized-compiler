# Module 4: FIRST & FOLLOW Sets / LL(1) Table Makefile

CC = gcc
CFLAGS = -Wall -g

TARGET = first_follow

.PHONY: all clean run

all: $(TARGET)

$(TARGET): first_follow.c
	$(CC) $(CFLAGS) -o $(TARGET) first_follow.c

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
