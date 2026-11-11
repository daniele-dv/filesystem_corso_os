#include <stdio.h>
#include <string.h>
#include  "comandi.h"
#include "filesystem.h"

#define MAX_COMMAND 1024

int main(int argc, char* argv[]) {

    char input[MAX_COMMAND];
    
    if (argc > 2) {
        printf("Bisogna usare: %s [nome_file.fs]\n", argv[0]);
        return 1;
    }

    if (argc == 2) {
        if (comando_open(argv[1]) == -1) {
            return 1;
        }
    }

    while (1) {
        print_prompt();
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        size_t pos_newline = strcspn(input, "\n");  // cerco la prima occorrenza di \n (quello prodotto da 'invio')

        if (input[pos_newline] == '\0' && pos_newline == MAX_COMMAND - 1) { // se il buffer è pieno e non contiene '\n'...
            int carattere = getchar();
            if (carattere != '\n' && carattere != EOF) {
                while (carattere != '\n' && carattere != EOF) { // se non ho raggiunto fine riga o fine file...
                    carattere = getchar();  // prendo un carattere alla volta senza salvarli da nessuna parte, dunque lo scarto
                }
                printf("ERRORE: comando troppo lungo\n");
                continue;   // passo alla prossima iterazione di while(1)
            }
        }

        input[pos_newline] = '\0';  // metto \0 al posto di \n
        
        if (interpreta_comando(input) == 1) {
            break;
        }
    }

    comando_close();    // mi assicuro di chiudere correttamente il FS anche se sono uscito senza close (es. ctrl+D)

    return 0;
}