#include <stdio.h>
#include <fcntl.h>  // open() - O_RDWR - O_CREAT - O_TRUNC
#include <unistd.h> // ftruncate() - close()
#include <sys/mman.h>   // mmap() - munmap() - msync() - PROT_READ - PROT_WRITE - MAP_SHARED - MS_SYNC - MAP_FAILED
#include "filesystem.h"

// static -> VALIDE PER TUTTA L'ESECUZIONE DEL PROGRAMMA
static int descrittore_fs = -1;
static unsigned char* memoria_fs = NULL;
static long dimensione_fs = 0;

int comando_format(const char* file, long dim) {

    if (memoria_fs != NULL) {
        printf("ERRORE: chiudere il FS precedente prima di fare di nuovo format\n");
        return -1;
    }

    if (dim <= 0) {
        printf("ERRORE: devo avere dim > 0\n");
        return -1;
    }

    descrittore_fs = open(file, O_RDWR | O_CREAT | O_TRUNC, 0644);

    if (descrittore_fs == -1) {
        perror("Errore durante la creazione del file");
        return -1;
    }

    if (ftruncate(descrittore_fs, dim) == -1) {
        perror("Errore durante il dimensionamento");
        close(descrittore_fs);
        descrittore_fs = -1;
        return -1;
    }

    memoria_fs = mmap(NULL, dim, PROT_READ | PROT_WRITE, MAP_SHARED, descrittore_fs, 0);    // puntatore al primo byte della regione mappata

    if (memoria_fs == MAP_FAILED) {
        perror("Errore durante la mappatura del FS");
        memoria_fs = NULL;
        close(descrittore_fs);
        descrittore_fs = -1;
        return -1;
    }

    dimensione_fs = dim;
    printf("Filesystem formattato: %s (%ld byte)\n", file, dim);

    return 0;
}

int comando_close(void) {
    if (memoria_fs == NULL) {
        return 0;
    }

    if (msync(memoria_fs, dimensione_fs, MS_SYNC) == -1) {  // sincronizzo regione mappata con il file effettivo
        perror("Errore con la sincronizzazione tra file e memoria mappata");
    }

    if (munmap(memoria_fs, dimensione_fs) == -1) {
        perror("Errore durante la rimozione della mappatura");
        return -1;
    }

    memoria_fs = NULL;
    dimensione_fs = 0;

    if (close(descrittore_fs) == -1) {
        perror("Errore durante la chiusura del FS");
        return -1;
    }

    descrittore_fs = -1;

    return 0;
}