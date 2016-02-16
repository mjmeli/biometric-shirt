CC = gcc
CFLAGS = -Wall -std=c99

SRCS = $(wildcard *.c)
TARGET = $(patsubst %.c,%,$(SRCS))

freqReg: freqReg.c
	gcc -Wall -std=c99 freqReg.c -o freqReg -lm

clean:
	$(RM) $(TARGET)
