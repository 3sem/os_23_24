all:
	@gcc shm_cli.c -lrt -pthread -o client
	@gcc shm_ser.c -lrt -pthread -o server
	@dd if=/dev/urandom of=input.txt bs=1024 count=4
	@echo " >>>  Launch server in second terminal"
	@./client 