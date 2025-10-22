#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "meditor.h"

#define MAX_INPUT 1024

int main(void) {
    char input[MAX_INPUT];
    char cmd;
    int line;
    char text[MAX_INPUT];

    printf("mtext> ");
    while (fgets(input, sizeof(input), stdin)) {
        if (input[0] == '\n') { printf("mtext> "); continue; }
        text[0] = '\0';
        if (sscanf(input, "%c %d %[^\n]", &cmd, &line, text) >= 2) {
            switch (cmd) {
                case 'i': // inserir/substituir linha
                    editor_insert_line(line, text);
                    break;
                case 'a': // append texto na linha
                    editor_append_text(line, text);
                    break;
                case 'r': // remover linha
                    editor_remove_line(line);
                    break;
                case 's': { // split linha: s <linha> <pos>
                    int pos;
                    if (sscanf(input, "s %d %d", &line, &pos) == 2) {
                        editor_split_line(line, pos);
                    } else {
                        printf("Uso: s <linha> <posicao>\n");
                    }
                    break;
                }
                case 'j': // join linhas: j <linha>
                    editor_join_lines(line);
                    break;
                default:
                    printf("Comando desconhecido\n");
            }
        } else if (strncmp(input, "undo", 4) == 0) {
            editor_undo();
        } else if (strncmp(input, "redo", 4) == 0) {
            editor_redo();
        } else if (strncmp(input, "print", 5) == 0) {
            display_document();
        } else if (sscanf(input, "save %1023s", text) == 1) {
            save_document(text);
        } else if (sscanf(input, "load %1023s", text) == 1) {
            load_document(text);
        } else if (strncmp(input, "quit", 4) == 0) {
            break;
        } else {
            printf("Comando invalido\n");
        }
        printf("mtext> ");
    }
    free_document();
    return 0;
}