all:
	@gcc mq_cli.c -lrt -o client
	@gcc mq_ser.c -lrt -o server
	@dd if=/dev/urandom of=input.txt bs=1024 count=4
	@echo " >>>  Launch client in second terminal"
	@./server