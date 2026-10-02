#include <stdio.h>
#include <string.h>
#include  "comandi.h"
#include "filesystem.h"

#define MAX_COMMAND 1024

int main(void) {

    char input[MAX_COMMAND];
    
    while (1) {
        printf("myfs> ");
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        size_t pos_newline = strcspn(input, "\n");  // cerco la prima occorrenza di \n (quello prodotto da 'invio')
        input[pos_newline] = '\0';  // metto \0 al posto di \n
        
        if (interpreta_comando(input) == 1) {
            break;
        }
    }

    comando_close();    // mi assicuro di chiudere correttamente il FS anche se sono uscito senza close (es. ctrl+D)

    return 0;
}