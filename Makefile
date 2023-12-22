CC = gcc
CCFLAGS = -Wall -Wextra -g -ggdb3

SOURCES = monte-carlo.c
EXECUTABLE = monte-carlo

all: $(EXECUTABLE)

$(EXECUTABLE):
	@$(CC) $(CCFLAGS) $(SOURCES) -o $@

clean: 
	@rm $(EXECUTABLE)