#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#define MAGIC_FS "MY_FS_OS"
#define MAX_INODE 128
#define LUNGHEZZA_NOME 64

#define TIPO_FILE 1
#define TIPO_DIRECTORY 2

typedef struct {
    char n_magico[9];
    long dimensione_totale;
    int max_numero_inode;
    long offset_tabella_inode;
    long offset_area_dati;
    int root_inode; // in quale inode ho piazzato '/'
    long inizio_spazio_libero;  // primo byte disponibile dell'area dati
} Superblocco;  // Contiene le informazioni generali del FS

typedef struct {
    int usato;
    int tipo;
    int padre;
    char nome[LUNGHEZZA_NOME];
    long dimensione_dati;
    long offset_dati;
} Inode;    // Singolo elemento del FS

int comando_format(const char* file, long dim);
int comando_close(void);
int comando_list(void);
int comando_make_directory(const char* nome);
int comando_change_directory(const char* nome);
void print_prompt(void);
int comando_touch(const char* nome);

#endif