CC = gcc

myfs: src/main.c
	$(CC) src/main.c -o myfs

clean:
	rm -f myfs
