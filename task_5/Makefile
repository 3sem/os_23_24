CC = gcc
CCFLAGS = -Wall -Wextra -Wpedantic -g -ggdb3

RECEIVER_TARGET  = receiver 
SENDLER_TARGET 	 = sendler

RECEIVER_SOURCES = receiver.c
SENDLER_SOURCES  = sendler.c

.PHONY: rebuild
rebuild: clean build

.PHONY: build
build: ${RECEIVER_TARGET} ${SENDLER_TARGET}

.PHONY: run
run:
	@./${RECEIVER_TARGET} output.txt &
	@./${SENDLER_TARGET}  input.txt

.PHONY: clean
clean:
	@rm -f ${SENDLER_TARGET} ${RECEIVER_TARGET} pid

${RECEIVER_TARGET}: ${RECEIVER_SOURCES}
	@$(CC) ${RECEIVER_SOURCES} -o ${RECEIVER_TARGET} ${CCFLAGS}

${SENDLER_TARGET}: ${SENDLER_SOURCES}
	@$(CC) ${SENDLER_SOURCES} -o ${SENDLER_TARGET} ${CCFLAGS}
