#include <stdio.h>
#include <string.h>
#include "meditor.h"

#define MAX_INPUT 1024

int main(void) {
    char input[MAX_INPUT];
    char cmd[64];
    int line;

    printf("mtext> ");
    // Loop principal do interpretador de comandos
    while (fgets(input, sizeof(input), stdin)) {
        // Remove o caractere de nova linha. Troca por '\0'
        input[strcspn(input, "\n")] = '\0';
        // Ignora linhas vazias
        if (input[0] == '\0') { printf("mtext> "); continue; }
        // Extrai o comando com ate 63 caracteres
        if (sscanf(input, "%63s", cmd) != 1) { printf("mtext> "); continue; }
        // Se o comando for 'i'
        if (strcmp(cmd, "i") == 0) {
            // i <pos> <texto> - inserir antes da pos; se pos==1 substitui
            // Extrai a posição e o texto
            char *rest = input + strlen(cmd);
            // Pula espaços
            while (*rest == ' ') rest++;
            int consumed = 0;
            // Lê a linha e o número de caracteres consumidos
            if (sscanf(rest, "%d %n", &line, &consumed) >= 1) {
                // Move o ponteiro rest para o início do texto
                rest += consumed;
                // Pula espaços
                while (*rest == ' ') rest++;
                // Chama a função de inserção
                editor_insert_line(line, *rest ? rest : "");
            } else {
                // Caso de erro na leitura
                printf("Uso: i <pos> <texto>\n");
            }
        } else if (strcmp(cmd, "d") == 0) { // Se o comando for 'd'
            // d <pos> - deletar linha na posição
            // Extrai a posição
            if (sscanf(input, "d %d", &line) == 1) {
                // Chama a função de remoção
                editor_remove_line(line);
            } else {
                // Caso de erro na leitura
                printf("Uso: d <pos>\n");
            }
        } else if (strcmp(cmd, "r") == 0) { // Se o comando for 'r'
            // r <pos> <texto> - substituir linha na posição
            // Extrai a posição e o texto
            char *rest = input + strlen(cmd);
            // Pula espaços
            while (*rest == ' ') rest++;
            int consumed = 0;
            // Lê a linha e o número de caracteres consumidos
            if (sscanf(rest, "%d %n", &line, &consumed) >= 1) {
                // Move o ponteiro rest para o início do texto
                rest += consumed;
                // Pula espaços
                while (*rest == ' ') rest++;
                if (!*rest) { printf("Uso: r <pos> <texto>\n"); } // Caso de texto vazio
                else editor_replace_line(line, rest); // Chama a função de substituição
            } else {
                // Caso de erro na leitura
                printf("Uso: r <pos> <texto>\n");
            }
        } else if (strcmp(cmd, "a") == 0) { // Se o comando for 'a'
            // a <pos> <sufixo> - concatena sufixo
            // Extrai a posição e o sufixo
            char *rest = input + strlen(cmd);
            // Pula espaços
            while (*rest == ' ') rest++;
            int consumed = 0;
            // Lê a linha e o número de caracteres consumidos
            if (sscanf(rest, "%d %n", &line, &consumed) >= 1) {
                rest += consumed;
                while (*rest == ' ') rest++;
                // Chama a função de append
                editor_append_text(line, *rest ? rest : "");
            } else {
                // Caso de erro na leitura
                printf("Uso: a <pos> <sufixo>\n");
            }
        } else if (strcmp(cmd, "split") == 0) { // Se o comando for 'split'
            // split <pos> <idx>
            int idx;
            // Extrai a posição e o índice
            if (sscanf(input, "split %d %d", &line, &idx) == 2) {
                // Chama a função de split
                editor_split_line(line, idx);
            } else {
                // Caso de erro na leitura
                printf("Uso: split <pos> <idx>\n");
            }
        } else if (strcmp(cmd, "join") == 0) { // Se o comando for 'join'
            // join <pos>
            // Extrai a posição
            if (sscanf(input, "join %d", &line) == 1) {
                // Chama a função de join
                editor_join_lines(line);
            } else {
                // Caso de erro na leitura
                printf("Uso: join <pos>\n");
            }
        } else if (strcmp(cmd, "undo") == 0) { // Se o comando for 'undo'
            editor_undo(); // Chama a função de undo
        } else if (strcmp(cmd, "redo") == 0) { // Se o comando for 'redo'
            editor_redo(); // Chama a função de redo
        } else if (strcmp(cmd, "print") == 0) { // Se o comando for 'print'
            display_document(); // Chama a função de exibição
        } else if (strcmp(cmd, "save") == 0) { // Se o comando for 'save'
            char filename[1024];
            // Extrai o nome do arquivo
            if (sscanf(input, "save %1023s", filename) == 1) save_document(filename);
            else printf("Uso: save <filename>\n"); // Caso de erro na leitura
        } else if (strcmp(cmd, "open") == 0) { // Se o comando for 'open'
            char filename[1024];
            // Extrai o nome do arquivo
            if (sscanf(input, "open %1023s", filename) == 1) load_document(filename);
            else printf("Uso: open <filename>\n"); // Caso de erro na leitura
        } else if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "exit") == 0) { // Se o comando for 'quit' ou 'exit'
            break; // Sai do loop
        } else { // Comando desconhecido
            printf("Comando desconhecido\n");
        }

        printf("mtext> ");
    }
    // Libera a memória alocada para o documento antes de sair
    free_document();
    return 0;
}