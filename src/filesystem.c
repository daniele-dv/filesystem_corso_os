#include <stdio.h>
#include <fcntl.h>  // open() - O_RDWR - O_CREAT - O_TRUNC
#include <unistd.h> // ftruncate() - close()
#include <sys/mman.h>   // mmap() - munmap() - msync() - PROT_READ - PROT_WRITE - MAP_SHARED - MS_SYNC - MAP_FAILED
#include <string.h> // memset() - strcpy()
#include "filesystem.h"

// static -> VALIDE PER TUTTA L'ESECUZIONE DEL PROGRAMMA E ACCESSIBILI SOLO DA QUESTO FILE
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



int comando_make_directory(const char* nome) {
    if (memoria_fs == NULL) {
        printf("ERRORE: nessun FS aperto\n");
        return -1;
    }

    if (nome == NULL || strlen(nome) == 0) {
        printf("ERRORE: non possono esistere directory senza nome\n");
        return -1;
    }

    if (strlen(nome) >= LUNGHEZZA_NOME) {
        printf("ERRORE: nome della directory troppo lungo\n");
        return -1;
    }

    if (strcmp(nome, ".") == 0 || strcmp(nome, "..") == 0 || strchr(nome, '/') != NULL) {    // strchr(...) cerca se nel nome compare il carattere '/'
        printf("ERRORE: Il nome inserito non e' valido\n");
        return -1;
    }

    Superblocco* superblocco = (Superblocco*) memoria_fs;
    Inode* tabella_inode = (Inode*) (memoria_fs + superblocco->offset_tabella_inode);

    for (int i = 0; i < superblocco->max_numero_inode; i++) {
        if (tabella_inode[i].usato == 1 && tabella_inode[i].padre == inode_directory_corrente && strcmp(tabella_inode[i].nome, nome) == 0) {
            printf("ERRORE: nome gia usato da un altro elemento\n");    // puo essere sia un file che una direcctory
            return -1;
        }
    }

    int inode_libero = -1;

    for (int i = 0; i < superblocco->max_numero_inode; i++) {
        if (tabella_inode[i].usato == 0) {
            inode_libero = i;
            break;
        }
    }

    if (inode_libero == -1) {
        printf("ERRORE: tutti gli inode sono gia' occupati\n");
        return -1;
    }

    Inode* new_dir = &tabella_inode[inode_libero];
    memset(new_dir, 0, sizeof(Inode));  // azzero l'inode prima di inizializzarlo

    new_dir->usato = 1;
    new_dir->tipo = TIPO_DIRECTORY;
    new_dir->padre = inode_directory_corrente;
    strcpy(new_dir->nome, nome);
    new_dir->dimensione_dati = 0;
    new_dir->offset_dati = -1;

    return 0;
}


int comando_change_directory(const char* nome) {
    if (memoria_fs == NULL) {
        printf("ERRORE: nessun FS aperto\n");
        return -1;
    }

    if (nome == NULL || strlen(nome) == 0) {
        printf("ERRORE: inserire il nome della directory\n");
        return -1;
    }

    Superblocco* superblocco = (Superblocco*) memoria_fs;

    Inode* tabella_inode = (Inode*)(memoria_fs + superblocco->offset_tabella_inode);

    if (strcmp(nome, "/") == 0) {
        inode_directory_corrente = superblocco->root_inode;
        return 0;
    }

    if (strcmp(nome, ".") == 0) {
        return 0;
    }

    if (strcmp(nome, "..") == 0) {
        Inode* current_dir = &tabella_inode[inode_directory_corrente];  // recupero l'inode della dir in cui mi trovo
        if (current_dir->padre != -1) { // a meno che non sono nella root, posso entrare nel padre
            inode_directory_corrente = current_dir->padre;
        }
        return 0;
    }

    // Cambio dir passando ad una dir specifica
    for (int i = 0; i < superblocco->max_numero_inode; i++) {
        if (tabella_inode[i].usato == 1 && tabella_inode[i].padre == inode_directory_corrente 
                                        && strcmp(tabella_inode[i].nome, nome) == 0) {
            if (tabella_inode[i].tipo != TIPO_DIRECTORY) {
                printf("ERRORE: %s non e' una directory\n", nome);
                return -1;
            }

            inode_directory_corrente = i;
            return 0;
        }
    }

    printf("ERRORE: la directory %s non esiste\n", nome);
    return -1;
}