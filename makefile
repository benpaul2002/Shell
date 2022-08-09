CC = gcc
CFLAGS  = -g -Wall -lm

TARGET = main

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(TARGET).c $(CFLAGS)

clean:
	$(RM) $(TARGET)
