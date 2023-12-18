RECEIVER_TARGET 	= receiver 
SENDLER_TARGET 		= sendler
RECEIVER_SOURCES 	= receiver.c
SENDLER_SOURCES 	= sendler.c
FLAGS = -Wall -Wextra -Wpedantic

.PHONY: receiver sendler

rebuild: clean build

build: ${RECEIVER_TARGET} ${SENDLER_TARGET}

${RECEIVER_TARGET}: ${RECEIVER_SOURCES}
	gcc ${RECEIVER_SOURCES} -o ${RECEIVER_TARGET} ${FLAGS}

${SENDLER_TARGET}: ${SENDLER_SOURCES}
	gcc ${SENDLER_SOURCES} -o ${SENDLER_TARGET} ${FLAGS}

run:
	./${SENDLER_TARGET} input.txt
	./${RECEIVER_TARGET} output.txt

clean:
	rm ${SENDLER_TARGET}
	rm ${RECEIVER_TARGET}
	rm pid
