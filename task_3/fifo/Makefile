all:
	@gcc fifo_reader.c -o reader
	@gcc fifo_writer.c -o writer
	@dd if=/dev/urandom of=input.txt bs=1024 count=4
	@echo " >>>  Launch reader in second terminal"
	@./writer input.txt