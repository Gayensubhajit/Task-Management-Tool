CC = gcc
CFLAGS = -Wall -Wextra -std=c99
EXECUTABLE = taskman

all: $(EXECUTABLE)

$(EXECUTABLE): task_manager.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(EXECUTABLE)
	rm -f *.o

.PHONY: all clean