CC = gcc
CCFLAGS = -Wall -Wextra -g -ggdb3

SOURCES = backup_daemon.c
EXECUTABLE = backup_daemon

all: $(EXECUTABLE)

$(EXECUTABLE):
	@$(CC) $(CCFLAGS) $(SOURCES) -o $@

.PHONY: clean
clean: 
	@rm $(EXECUTABLE)

test:
	@$(CC) $(CCFLAGS) test.c -o $@
