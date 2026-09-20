CC = gcc

myfs: src/main.c src/comandi.c src/comandi.h
	$(CC) src/main.c src/comandi.c -o myfs

clean:
	rm -f myfs
