CC = gcc
CCFLAGS = -Wall -Wextra -g -ggdb3

SOURCES = funcs.c main.c
EXECUTABLE = terminal

all: $(EXECUTABLE)

$(EXECUTABLE):
	@$(CC) $(CCFLAGS) $(SOURCES) -o $@

clean: 
	@rm $(EXECUTABLE)
