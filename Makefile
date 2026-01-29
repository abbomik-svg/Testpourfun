CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
LDFLAGS = -lm

SRCS = main.c simu_urgences.c
OBJS = $(SRCS:.c=.o)
TARGET = simu_urgences

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all run clean
