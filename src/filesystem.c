#include <stdio.h>
#include <fcntl.h>  // open() - O_RDWR - O_CREAT - O_TRUNC
#include <unistd.h> // ftruncate() - close()
#include <sys/mman.h>   // mmap() - munmap() - msync() - PROT_READ - PROT_WRITE - MAP_SHARED - MS_SYNC - MAP_FAILED
#include <string.h> // memset() - strcpy()
#include "filesystem.h"

// static -> VALIDE PER TUTTA L'ESECUZIONE DEL PROGRAMMA
static int descrittore_fs = -1;
static unsigned char* memoria_fs = NULL;
static long dimensione_fs = 0;
static int inode_directory_corrente = -1;

int comando_format(const char* file, long dim) {

    if (memoria_fs != NULL) {
        printf("ERRORE: chiudere il FS precedente prima di fare di nuovo format\n");
        return -1;
    }

    if (dim <= 0) {
        printf("ERRORE: devo avere dim > 0\n");
        return -1;
    }
    
    long spazio_metadati = (long)sizeof(Superblocco) + (long)(MAX_INODE * sizeof(Inode));

    if (dim <= spazio_metadati) {
        printf("ERRORE: dimensione insufficiente per questo tipo di FS\n");
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
    
    Superblocco* superblocco = (Superblocco*)memoria_fs;    // Interpreto i byte iniziali della memoria come un superblocco
    memset(superblocco, 0, sizeof(Superblocco));
    strcpy(superblocco->n_magico, MAGIC_FS);
    superblocco->dimensione_totale = dim;
    superblocco->max_numero_inode = MAX_INODE;
    superblocco->offset_tabella_inode = (long)sizeof(Superblocco);
    superblocco->offset_area_dati = (long)sizeof(Superblocco) + (long)(MAX_INODE * sizeof(Inode));
    superblocco->root_inode = 0;
    superblocco->inizio_spazio_libero = superblocco->offset_area_dati;


    Inode* tabella_inode = (Inode*)(memoria_fs + superblocco->offset_tabella_inode);
    memset(tabella_inode, 0, MAX_INODE * sizeof(Inode));
    Inode* root = &tabella_inode[0];
    root->usato = 1;
    root->tipo = TIPO_DIRECTORY;
    root->padre = -1;
    strcpy(root->nome, "/");
    root->dimensione_dati = 0;
    root->offset_dati = -1;

    inode_directory_corrente = 0;   // appena formatto parto dalla root

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
    inode_directory_corrente = -1; // chiuso il FS non sono piu in nessuna directory

    if (close(descrittore_fs) == -1) {
        perror("Errore durante la chiusura del FS");
        return -1;
    }

    descrittore_fs = -1;

    return 0;
}



int comando_list(void) {
    
    if (memoria_fs == NULL) {   // gestisco ls se usato prima di format
        printf("ERRORE: nessun FS aperto\n");
        return -1;
    }

    Superblocco* superblocco = (Superblocco*) memoria_fs;

    Inode* tabella_inode = (Inode*) (memoria_fs + superblocco->offset_tabella_inode);
    
    int elementi_trovati = 0;   // cosi so se la dir che sto esaminando e' vuota o no

    for (int i = 0; i < superblocco->max_numero_inode; i++) {
        if (tabella_inode[i].usato == 1 && tabella_inode[i].padre == inode_directory_corrente) {
            // scrivo diversamente se ho trovato una cartella o un file per capire cosa e' cosa
            if (tabella_inode[i].tipo == TIPO_DIRECTORY) {
                printf("%s/ \n", tabella_inode[i].nome);
            }
            else {
                printf("%s\n", tabella_inode[i].nome);
            }

            elementi_trovati++;
        }
    }

    if (elementi_trovati == 0) {
        printf("Nessun elemento trovato\n");
    }

    return 0;
}