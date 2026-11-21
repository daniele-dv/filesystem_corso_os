#include <stdio.h>
#include <string.h>
#include <stdlib.h> // strtol()
#include "filesystem.h"
#include "comandi.h"

int interpreta_comando(char* input) {
    
    char* command = strtok(input, " \t"); // divido il comando sugli spazi e su eventuali caratteri 'TAB'

    if (command == NULL) {
        return 0;
    }

    char* arg = strtok(NULL, " \t");  // prendo l'argomento solo se il comando non e' nullo

    if (strcmp(command, "format") == 0) {
        char* dimensione_str = strtok(NULL, " \t");
        if (arg == NULL || dimensione_str == NULL || strtok(NULL, " \t") != NULL) { // se non ho argomenti - ho troppi argomenti ...
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
        if (arg != NULL) {
            printf("Riprova: <close>\n");
            return 0;   // uso 0 e non 1 così posso proseguire l'esecuzione
        }
        int risultato = comando_close();
        if (risultato == -1) {
            return 0;   // ho un errore con msync()-munmap() -> mantengo attiva la shell
        }
        if (risultato == -2) {
            return 2;   // errore nella close() -> termino il programma segnalando un errore
        }
        printf("Addio...\n");
        return 1;
    }
    else if (strcmp(command, "ls") == 0) {
        if (arg != NULL) {
            printf( "Riprova: <ls>\n");
            return 0;   // 0 così posso continuare l'esecuzione
        }
        comando_list();
    }
    else if (strcmp(command, "mkdir") == 0) {
        if (arg == NULL || strtok(NULL, " \t") != NULL) {   // se non ho argomenti - ho troppi argomenti...
            printf("Riprova: mkdir <nome_directory>\n");
            return 0;
        }
        comando_make_directory(arg);
    }
    else if (strcmp(command, "cd") == 0) {
        if (arg == NULL || strtok(NULL, " \t") != NULL) {   // se non ho argomenti - ho troppi argomenti...
            printf("Riprova: cd <nome_directory>\n");
            return 0;
        }
        comando_change_directory(arg);
    }
    else if (strcmp(command, "touch") == 0) {
        if (arg == NULL || strtok(NULL, " \t") != NULL) {   // se non ho argomenti - ho troppi argomenti...
            printf("Riprova: touch <nome_file>\n");
            return 0;
        }
        comando_touch(arg);
    }
    else if (strcmp(command, "append") == 0) {
        char* testo = strtok(NULL, "\n");
        
        if (arg == NULL || testo == NULL) {
            printf("Riprova: append <nome_file> <testo>\n");
            return 0;
        }
        
        comando_append(arg, testo);
    }
    else if (strcmp(command, "cat") == 0) {
        if (arg == NULL || strtok(NULL, " \t") != NULL) {   // se non ho argomenti - ho troppi argomenti...
            printf("Riprova: cat <nome_file>\n");
            return 0;
        }
        comando_cat(arg);
    }
    else if (strcmp(command, "rm") == 0) {
        if (arg == NULL || strtok(NULL, " \t") != NULL) {   // se non ho argomenti - ho troppi argomenti...
            printf("Riprova: rm <dir/file>\n");
            return 0;
        }
        comando_remove(arg);
    }
    else {
        printf("Che comando e' <%s> ?\n", input);
    }

    return 0;
}