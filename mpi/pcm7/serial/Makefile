CC = gcc
CFLAGS = -Wall -Wextra -std=c99
INPUT ?= main       # default input file is main.c
TARGET = main 
OBJ = $(INPUT).o utils_serial.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(INPUT).o: $(INPUT).c utils_serial.h
	$(CC) $(CFLAGS) -c $<

utils_serial.o: utils_serial.c utils_serial.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(TARGET)
