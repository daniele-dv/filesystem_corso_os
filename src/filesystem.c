#include <stdio.h>
#include <fcntl.h>  // open() - O_RDWR - O_CREAT - O_TRUNC
#include <unistd.h> // ftruncate() - close()
#include "filesystem.h"

int comando_format(const char* file, long dim) {

    int fd;

    if (dim <= 0) {
        printf("ERRORE: devo avere dim > 0\n");
        return -1;
    }

    fd = open(file, O_RDWR | O_CREAT | O_TRUNC, 0644);

    if (fd == -1) {
        perror("Errore durante la creazione del file");
        return -1;
    }

    if (ftruncate(fd, dim) == -1) {
        perror("Errore durante il dimensionamento");
        close(fd);
        return -1;
    }

    if (close(fd) == -1) {
        perror("Errore durante la chiusura del file");
        return -1;
    }

    printf("Filesystem formattato: %s (%ld byte)\n", file, dim);

    return 0;
}