#include <stdio.h>
#include <string.h>
#include <stdlib.h> // strtol()
#include "filesystem.h"
#include "comandi.h"

int interpreta_comando(char* input) {
    
    char* command = strtok(input, " "); // divido il comando sugli spazi

    if (command == NULL) {
        return 0;
    }

    char* arg = strtok(NULL, " ");  // prendo l'argomento solo se il comando non e' nullo

    if (strcmp(command, "format") == 0) {
        char* dimensione_str = strtok(NULL, " ");
        if (arg == NULL || dimensione_str == NULL) {
            printf("Riprova: format <nome_file> <size>\n");
            return 0;
        }

        char* fine_conversione;
        long dim = strtol(dimensione_str, &fine_conversione, 10);   // interpreto la stringa come numero in base 10

        if (*fine_conversione != '\0' || dim <= 0) {
            printf("ERRORE: inserire una dim > 0\n");
            return 0;
        }

        comando_format(arg, dim);
    }
    else if (strcmp(command, "close") == 0) {
        printf("Addio...\n");
        return 1;
    }
    else if (strcmp(command, "ls") == 0) {
        printf("Comando inserito: ls\n");
    }
    else if (strcmp(command, "mkdir") == 0) {
        if (arg == NULL) {
            printf("Riprova: mkdir <nome_directory>\n");
            return 0;
        }
        printf("Creazione directory %s\n", arg);
    }
    else if (strcmp(command, "touch") == 0) {
        if (arg == NULL) {
            printf("Riprova: touch <nome_file>\n");
            return 0;
        }
        printf("Creazione file %s\n", arg);
    }
    else {
        printf("Che comando e' <%s> ?\n", input);
    }

    return 0;
}