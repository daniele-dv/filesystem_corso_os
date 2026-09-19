#include <stdio.h>
#include <string.h>

#define MAX_COMMAND 1024

int main(void) {
    char input[MAX_COMMAND];

    printf("Filesystem\n");
    
    while (1) {
        printf("myfs> ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        int pos_newline = strcspn(input, "\n");  // cerco la prima occorrenza di \n (quello prodotto da 'invio')
        input[pos_newline] = '\0';  // metto \0 al posto di \n

        if (strcmp(input, "close") == 0) {
            printf("Addio...\n");
            break;
        }

        printf("Comando inserito: %s\n", input);
    }

    return 0;
}