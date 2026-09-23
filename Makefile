CC = gcc

myfs: src/main.c src/comandi.c src/filesystem.c src/comandi.h src/filesystem.h
	$(CC) src/main.c src/comandi.c src/filesystem.c -o myfs

clean:
	rm -f myfs
