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

        char* command = strtok(input, " "); // divido il comando sugli spazi
        char* arg = strtok(NULL, " ");
        
        if (command == NULL) {
            continue;
        }


        if (strcmp(command, "close") == 0) {
            printf("Addio...\n");
            break;
        }
        else if (strcmp(command, "ls") == 0) {
            printf("Comando inserito: ls\n");
        }
        else if (strcmp(command, "mkdir") == 0) {
            printf("Comando inserito: mkdir\n");
            if (arg != NULL) {
                printf("Directory; %s\n", arg);
            }
        }
        else if (strcmp(command, "touch") == 0) {
            printf("Comando inserito: touch\n");
            if (arg != NULL) {
                printf("File: %s\n", arg);
            }
        }
        else { 
            printf("Che comando e' <%s> ?\n", input);
        }
    }

    return 0;
}