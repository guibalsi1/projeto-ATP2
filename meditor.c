//
// Created by Guilherme on 10/10/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Definições de estruturas de dados
// Fiz isso para facilitar a implementação do undo/redo
struct Operation;
static void clear_stack(struct Operation **stack);
static void undo(void);
static void redo(void);
static struct Operation *undo_stack = NULL;
static struct Operation *redo_stack = NULL;

// Estrutura para uma linha do documento
typedef struct Line{
    char *content;
    // Estou a usar size_t para representar o tamanho da linha. Para ter a possibilidade de linhas maiores.
    size_t size;
    // Implementação de lista duplamente ligada
    struct Line *next, *prev;
} Line;

// Estrutura para o documento
typedef struct {
    Line *lines_head, *lines_tail, *lines;
    // Mesma coisa do size na linha, para contar tamanho do documento maior.
    size_t line_count;
} Document;

// Variável global para o documento, para simplificar a implementação
static Document *doc = NULL;

// funcao para criar o documento se não existir, iniciada apenas uma vez por execução caso necessário
static void create_document(void) {
    if (doc == NULL) {
        doc = (Document *)malloc(sizeof(Document));
        doc->lines = NULL;
        doc->lines_head = NULL;
        doc->lines_tail = NULL;
        doc->line_count = 0;
    }
}

// Função de adicionar uma linha no documento
void add_line(const char *text, int position) {
    // Inicializa o documento se necessário
    create_document();
    position--; // Converter para índice baseado em 0
    // Se o texto for NULL, não faz nada
    if (text == NULL) {
        return;
    }
    // Se a posição for válida, substitui o conteúdo da linha existente, como implementado no enunciado
    if (doc->line_count > 0 && position >= 0 && (size_t)position < doc->line_count) {
        Line *current = doc->lines_head;
        // Percorre até a linha na posição desejada
        for (int i = 0; i < position && current != NULL; i++) {
            current = current->next;
        }
        // Substitui o conteúdo da linha existente caso a linha seja encontrada
        if (current != NULL) {
            free(current->content);
            current->size = strlen(text);
            current->content = (char *)malloc(current->size + 1);
            // Copia o novo texto para a linha
            if (current->content != NULL) {
                strcpy(current->content, text);
            }
        }
        return;
    }
    // Cria uma linha
    Line *new_line = (Line *)malloc(sizeof(Line));
    // Verifica se a alocação foi bem-sucedida
    if (new_line == NULL) {
        return;
    }
    // Inicializa a nova linha
    new_line->size = strlen(text);
    new_line->content = (char *)malloc(new_line->size + 1);
    // Verifica se a alocação foi bem-sucedida
    if (new_line->content == NULL) {
        free(new_line);
        return;
    }
    strcpy(new_line->content, text);
    new_line->next = NULL;
    new_line->prev = NULL;

    // Operações de inserção na lista duplamente ligada
    // Verifica a posição de inserção
    if (doc->line_count == 0) { // Inserir na lista vazia
        doc->lines_head = new_line;
        doc->lines_tail = new_line;
        doc->lines = new_line;
    } else if (position <= 0) { // Inserir no início
        new_line->next = doc->lines_head;
        if (doc->lines_head != NULL) {
            doc->lines_head->prev = new_line;
        }
        doc->lines_head = new_line;
    } else if ((size_t)position >= doc->line_count) { // Inserir no final
        new_line->prev = doc->lines_tail;
        if (doc->lines_tail != NULL) {
            doc->lines_tail->next = new_line;
        }
        doc->lines_tail = new_line;
    } else {
        Line *current = doc->lines_head;
        for (int i = 0; i < position && current != NULL; i++) {
            current = current->next;
        }
        if (current != NULL) { // Inserir no meio
            new_line->next = current;
            new_line->prev = current->prev;
            if (current->prev != NULL) {
                current->prev->next = new_line;
            }
            current->prev = new_line;
        }
    }
    // Incrementa a contagem de linhas do documento
    doc->line_count++;
}

// Função de adicionar texto ao final de uma linha existente
void append_text(int line_number, const char *text) {
    create_document();
    line_number--; // Converte para índice baseado em 0
    // Verifica se a linha existe, caso contrário, não faz nada
    if (text == NULL || line_number < 0 || (size_t)line_number >= doc->line_count) {
        return;
    }

    // Percorre até a linha desejada
    Line *line = doc->lines_head;
    for (int i = 0; i < line_number && line != NULL; i++) {
        line = line->next;
    }

    // Se a linha não for encontrada, não faz nada
    if (line == NULL) {
        return;
    }

    // Realoca a memória para acomodar o novo texto
    size_t add_len = strlen(text);
    size_t new_size = line->size + add_len;

    char *new_content = (char *)realloc(line->content, new_size + 1);
    // Verifica se a realocação foi bem-sucedida
    if (new_content == NULL) {
        return;
    }
    // Adiciona o novo texto ao final da linha
    memcpy(new_content + line->size, text, add_len + 1);
    line->content = new_content;
    line->size = new_size;
}

// Função de remover uma linha do documento
void remove_line(int position) {
    create_document();
    position--; // Converter para índice baseado em 0
    // Verifica se a posição é válida
    if (doc->line_count == 0 || position < 0 || (size_t)position >= doc->line_count) {
        return;
    }
    // Percorre até a linha na posição desejada
    Line *current = doc->lines_head;
    for (int i = 0; i < position && current != NULL; i++) {
        current = current->next;
    }
    // Operações para remover a linha da lista duplamente ligada
    // Remove a linha encontrada
    if (current != NULL) {
        if (current->prev != NULL) {
            current->prev->next = current->next;
        } else {
            doc->lines_head = current->next;
        }
        if (current->next != NULL) {
            current->next->prev = current->prev;
        } else {
            doc->lines_tail = current->prev;
        }
        free(current->content);
        free(current);
        doc->line_count--;
    }
}

void split_line(int line_number, int split_position) {
    create_document();
    line_number--; // Converte para índice baseado em 0
    if (line_number < 0 || (size_t)line_number >= doc->line_count) {
        return;
    }

    // Percorre até a linha desejada
    Line *line = doc->lines_head;
    for (int i = 0; i < line_number && line != NULL; i++) {
        line = line->next;
    }

    // Verifica se a linha existe e se a posição de divisão é válida
    if (line == NULL || split_position < 0 || (size_t)split_position > line->size) {
        return;
    }

    // Cria as duas novas partes da linha
    char *first_part = (char *)malloc(split_position + 1);
    char *second_part = (char *)malloc(line->size - split_position + 1);
    // Verifica se as alocações foram bem-sucedidas
    if (first_part == NULL || second_part == NULL) {
        free(first_part);
        free(second_part);
        return;
    }

    // Copia o conteúdo para as duas partes
    memcpy(first_part, line->content, (size_t)split_position);
    first_part[split_position] = '\0';
    strcpy(second_part, line->content + split_position);

    // Atualiza a linha original com a primeira parte
    free(line->content);
    line->content = first_part;
    line->size = (size_t)split_position;

    // Cria um novo nó de linha para a segunda parte e insere após a linha atual
    Line *new_line = (Line *)malloc(sizeof(Line));
    if (!new_line) { free(second_part); return; }
    new_line->size = strlen(second_part);
    new_line->content = (char *)malloc(new_line->size + 1);
    if (!new_line->content) { free(new_line); free(second_part); return; }
    strcpy(new_line->content, second_part);
    free(second_part);

    new_line->next = line->next;
    new_line->prev = line;
    if (line->next) {
        line->next->prev = new_line;
    } else {
        doc->lines_tail = new_line;
    }
    line->next = new_line;

    doc->line_count++;
}

// Função de juntar duas linhas consecutivas
void join_lines(int line_number) {
    create_document();
    line_number--; // Converte para índice baseado em 0
    // Verifica se a linha e a próxima linha existem
    if (line_number < 0 || (size_t)line_number >= doc->line_count - 1) {
        return;
    }

    // Percorre até a linha desejada
    Line *first_line = doc->lines_head;
    for (int i = 0; i < line_number && first_line != NULL; i++) {
        first_line = first_line->next;
    }

    // Verifica se a linha e a próxima linha existem. Neste caso só por segurança
    if (first_line == NULL || first_line->next == NULL) {
        return;
    }

    // Junta o conteúdo das duas linhas
    Line *second_line = first_line->next;
    size_t new_size = first_line->size + second_line->size;

    // Realoca a memória para acomodar o novo conteúdo
    char *new_content = (char *)realloc(first_line->content, new_size + 1);
    // Verifica se a realocação foi bem-sucedida
    if (new_content == NULL) {
        return;
    }

    // Adiciona o conteúdo da segunda linha ao final da primeira linha
    memcpy(new_content + first_line->size, second_line->content, second_line->size + 1);
    first_line->content = new_content;
    first_line->size = new_size;

    // Remove a segunda linha da lista duplamente ligada
    if (second_line->next != NULL) {
        second_line->next->prev = first_line;
    } else {
        doc->lines_tail = first_line;
    }
    first_line->next = second_line->next;

    free(second_line->content);
    free(second_line);
    doc->line_count--;
}

// Função para exibir o conteúdo do documento
void display_document(void) {
    // Verifica se o documento existe
    Line *current = doc ? doc->lines_head : NULL;
    size_t line_number = 1; // Contador de linhas
    while (current != NULL) {
        // Exibe o número da linha e o conteúdo. No caso como foi usado size_t, usei %zu para imprimir
        // E o 4 para alinhar à direita com 4 espaços
        printf("%4zu| %s\n", line_number++, current->content);
        current = current->next;
    }
}

// Função para liberar a memória alocada para o documento
void free_document(void) {
    // Verifica se o documento existe
    if (doc != NULL) {
        // Percorre todas as linhas e libera a memória
        Line *current = doc->lines_head;
        while (current != NULL) {
            Line *next = current->next;
            free(current->content);
            free(current);
            current = next;
        }
        // Libera a memória do documento
        free(doc);
        doc = NULL;
        // Limpa as pilhas de undo/redo
        clear_stack(&undo_stack);
        clear_stack(&redo_stack);
    }
}

// Função para salvar o documento em um arquivo
void save_document(const char *filename) {
    create_document();
    // Abre o arquivo para escrita
    FILE *file = fopen(filename, "w");
    // Verifica se o arquivo foi aberto com sucesso
    if (file == NULL) {
        perror("Failed to open file for writing");
        return;
    }
    // Percorre todas as linhas e escreve no arquivo
    Line *current = doc->lines_head;
    while (current != NULL) {
        fprintf(file, "%s\n", current->content);
        current = current->next;
    }
    fclose(file);
}

// Função para carregar o documento de um arquivo
void load_document(const char *filename) {
    // Libera o documento atual antes de carregar um novo
    free_document();
    create_document();
    // Abre o arquivo para leitura
    FILE *file = fopen(filename, "r");
    // Verifica se o arquivo foi aberto com sucesso
    if (file == NULL) {
        perror("Failed to open file for reading");
        return;
    }
    // Estou a usar um 'buffer' fixo para ler as linhas do arquivo
    char buffer[1024];
    // Lê cada linha do arquivo e adiciona ao documento
    while (fgets(buffer, sizeof(buffer), file)) {
        // Remove o caracter de nova linha, se presente
        buffer[strcspn(buffer, "\n")] = 0;
        // Adiciona a linha ao documento
        add_line(buffer, (int)doc->line_count + 1);
    }
    fclose(file);
}

// -------Implementação do Undo/Redo-------
// Estou a usar um enum para representar os tipos de comandos. Para facilitar a implementação.
typedef enum {
    CMD_ADD_LINE,
    CMD_APPEND_TEXT,
    CMD_REMOVE_LINE,
    CMD_SPLIT_LINE,
    CMD_JOIN_LINES
} CommandType;

// Estrutura para representar uma operação implementada na pilha de undo/redo
typedef struct Operation {
    CommandType type;
    int line_number;
    int existed_before; // 1 se a linha existia antes da operação
    char *old_text; // Para undo
    char *new_text; // Para redo
    int split_position; // usado para split/join quando necessário
    struct Operation *next;
} Operation;

// Função pare empilhar uma operação na pilha
void push_operation(Operation **stack, Operation *op) {
    op->next = *stack;
    *stack = op;
}

// Função para desempilhar uma operação da pilha
Operation *pop_operation(Operation **stack) {
    if (!*stack) return NULL;
    Operation *op = *stack;
    *stack = op->next;
    op->next = NULL;
    return op;
}

// Função para limpar a pilha de operações
static void clear_stack(Operation **stack) {
    while (*stack) {
        Operation *op = pop_operation(stack);
        if (op->old_text) free(op->old_text);
        if (op->new_text) free(op->new_text);
        free(op);
    }
}

// Função para obter o texto de uma linha específica
static char *get_line_text(int line_number) {
    // Verifica se o documento existe e se a linha é válida
    if (!doc || line_number < 1 || (size_t)line_number > doc->line_count) return NULL;
    // Percorre até a linha desejada
    Line *current = doc->lines_head;
    for (int i = 1; i < line_number && current; i++) current = current->next;
    // Se não encontrar a linha, retorna NULL
    if (!current) return NULL;
    // Retorna o conteúdo da linha
    return current->content;
}

// Helper: insere uma linha SEM substituir (1-based position)
static void insert_line_at(const char *text, int position) {
    if (!text) return;
    create_document();
    if (position < 1) position = 1;
    // Cria nó
    Line *new_line = (Line *)malloc(sizeof(Line));
    if (!new_line) return;
    new_line->size = strlen(text);
    new_line->content = (char *)malloc(new_line->size + 1);
    if (!new_line->content) { free(new_line); return; }
    strcpy(new_line->content, text);
    new_line->next = NULL;
    new_line->prev = NULL;

    if (doc->line_count == 0) {
        doc->lines_head = doc->lines_tail = new_line;
        doc->line_count = 1;
        return;
    }
    if (position == 1) {
        new_line->next = doc->lines_head;
        doc->lines_head->prev = new_line;
        doc->lines_head = new_line;
        doc->line_count++;
        return;
    }
    // Inserir no meio ou final: caminhar até posição-1 e inserir após
    Line *cur = doc->lines_head;
    int idx = 1;
    while (cur->next && idx < position - 1) { cur = cur->next; idx++; }
    // Inserir após cur
    new_line->next = cur->next;
    new_line->prev = cur;
    if (cur->next) cur->next->prev = new_line; else doc->lines_tail = new_line;
    cur->next = new_line;
    doc->line_count++;
}

// Implementação da função undo
static void undo(void) {
    // Desempilha a última operação da pilha de undo
    // Como estou a usar de forma estática, a pilha é global
    Operation *op = pop_operation(&undo_stack);
    // Se não houver operação para desfazer, retorna
    if (!op) { printf("Nada para desfazer.\n"); return; }
    // switch para cada tipo de operação
    switch (op->type) {
        // Caso de adicionar linha
        case CMD_ADD_LINE:
            // Se a linha existia antes, restaura o texto antigo
            if (op->existed_before) {
                add_line(op->old_text, op->line_number);
            } else { // Caso contrário, remove a linha
                remove_line(op->line_number);
            }
            break;
        // Caso de append texto
        case CMD_APPEND_TEXT:
            // Restaura o texto antigo
            add_line(op->old_text, op->line_number);
            break;
        // Caso de remover linha
        case CMD_REMOVE_LINE:
            // Restaura a linha removida (inserção verdadeira)
            insert_line_at(op->old_text, op->line_number);
            break;
        case CMD_SPLIT_LINE:
            // Desfaz o split juntando novamente as duas linhas
            join_lines(op->line_number);
            break;
        case CMD_JOIN_LINES:
            // Desfaz o join dividindo novamente no ponto original
            split_line(op->line_number, op->split_position);
            break;
        default:
            break;
    }
    // Empilha a operação na pilha de redo
    push_operation(&redo_stack, op);
}

// Implementação da função redo
static void redo(void) {
    // Desempilha a última operação da pilha de redo
    // Como estou a usar de forma estática, a pilha é global
    Operation *op = pop_operation(&redo_stack);
    // Se não houver operação para refazer, retorna
    if (!op) { printf("Nada para refazer.\n"); return; }
    // switch para cada tipo de operação
    switch (op->type) {
        // Caso de adicionar linha
        case CMD_ADD_LINE:
            // Refaz a adição da linha
            add_line(op->new_text, op->line_number);
            break;
        // Caso de append texto
        case CMD_APPEND_TEXT:
            // Refaz o append do texto
            add_line(op->new_text, op->line_number);
            break;
        // Caso de remover linha
        case CMD_REMOVE_LINE:
            // Refaz a remoção da linha
            remove_line(op->line_number);
            break;
        case CMD_SPLIT_LINE:
            // Refaz o split usando a posição original
            split_line(op->line_number, op->split_position);
            break;
        case CMD_JOIN_LINES:
            // Refaz o join
            join_lines(op->line_number);
            break;
        default:
            break;
    }
    // Empilha a operação na pilha de undo
    push_operation(&undo_stack, op);
}

// --- Funções expostas do editor de texto ---
// Essas funções são chamadas pelo main.c para realizar as operações no editor de texto
// Elas também gerenciam as pilhas de undo/redo

// Função para inserir uma linha
void editor_insert_line(int line_number, const char *text) {
    // Cria uma operação
    Operation *op = (Operation *)malloc(sizeof(Operation));
    // Verifica se a alocação foi bem-sucedida
    if (!op) return;
    // Preenche os dados da operação
    op->type = CMD_ADD_LINE;
    op->line_number = line_number;
    op->split_position = 0;
    char *old = get_line_text(line_number);
    op->existed_before = old != NULL;
    // Salva o texto antigo e o novo texto. Usa 'strdup' para duplicar as strings
    op->old_text = old ? strdup(old) : strdup("");
    op->new_text = text ? strdup(text) : strdup("");
    op->next = NULL;
    // Realiza a operação no documento
    add_line(text, line_number);
    // Empilha a operação na pilha de undo e limpa a pilha de redo
    push_operation(&undo_stack, op);
    clear_stack(&redo_stack);
}

// Função para adicionar texto ao final de uma linha
void editor_append_text(int line_number, const char *text) {
    // Obtém o texto antigo da linha
    char *old = get_line_text(line_number);
    // Se a linha não existir, exibe uma mensagem de erro e retorna
    if (!old) { printf("Linha %d inexistente.\n", line_number); return; }
    // Cria uma nova operação
    Operation *op = (Operation *)malloc(sizeof(Operation));
    // Verifica se a alocação foi bem-sucedida
    if (!op) return;
    // Preenche os dados da operação
    op->type = CMD_APPEND_TEXT;
    op->line_number = line_number;
    op->split_position = 0;
    op->existed_before = 1;
    op->old_text = strdup(old);
    size_t old_len = strlen(old);
    size_t add_len = text ? strlen(text) : 0;
    // Cria o novo texto concatenando o antigo com o novo
    char *newstr = (char *)malloc(old_len + add_len + 1);
    // Verifica se a alocação foi bem-sucedida
    if (!newstr) { free(op); return; }
    // Copia os textos para a nova string
    memcpy(newstr, old, old_len);
    // Adiciona o texto novo se houver
    if (add_len) memcpy(newstr + old_len, text, add_len);
    // Adiciona o terminador nulo
    newstr[old_len + add_len] = '\0';
    op->new_text = newstr;
    op->next = NULL;
    // Realiza a operação no documento
    append_text(line_number, text);
    // Empilha a operação na pilha de undo e limpa a pilha de redo
    push_operation(&undo_stack, op);
    clear_stack(&redo_stack);
}

// Função para remover uma linha
void editor_remove_line(int line_number) {
    // Obtém o texto antigo da linha
    char *old = get_line_text(line_number);
    // Se a linha não existir, exibe uma mensagem de erro e retorna
    if (!old) { printf("Linha %d inexistente.\n", line_number); return; }
    // Cria uma operação
    Operation *op = (Operation *)malloc(sizeof(Operation));
    // Verifica se a alocação foi bem-sucedida
    if (!op) return;
    // Preenche os dados da operação
    op->type = CMD_REMOVE_LINE;
    op->line_number = line_number;
    op->split_position = 0;
    op->existed_before = 1;
    op->old_text = strdup(old);
    op->new_text = strdup("");
    op->next = NULL;
    // Realiza a operação no documento
    remove_line(line_number);
    // Empilha a operação na pilha de undo e limpa a pilha de redo
    push_operation(&undo_stack, op);
    clear_stack(&redo_stack);
}

// Nova função: dividir uma linha em duas partes
void editor_split_line(int line_number, int split_position) {
    // Valida a linha
    char *old = get_line_text(line_number);
    if (!old) { printf("Linha %d inexistente.\n", line_number); return; }
    // Valida posição
    size_t len = strlen(old);
    if (split_position < 0 || (size_t)split_position > len) { printf("Posição de split inválida.\n"); return; }
    // Cria operação
    Operation *op = (Operation *)malloc(sizeof(Operation));
    if (!op) return;
    op->type = CMD_SPLIT_LINE;
    op->line_number = line_number;
    op->existed_before = 1;
    op->old_text = strdup(old);
    op->new_text = NULL;
    op->split_position = split_position;
    op->next = NULL;
    // Executa
    split_line(line_number, split_position);
    // Pilhas
    push_operation(&undo_stack, op);
    clear_stack(&redo_stack);
}

// Função para juntar duas linhas consecutivas
void editor_join_lines(int line_number) {
    // Valida a primeira linha e a próxima
    char *first = get_line_text(line_number);
    char *second = get_line_text(line_number + 1);
    if (!first || !second) { printf("Não é possível juntar: verifique as linhas %d e %d.\n", line_number, line_number + 1); return; }
    size_t first_len = strlen(first);
    // Cria operação
    Operation *op = (Operation *)malloc(sizeof(Operation));
    if (!op) return;
    op->type = CMD_JOIN_LINES;
    op->line_number = line_number;
    op->existed_before = 1;
    // Para desfazer precisamos saber onde dividir novamente
    op->split_position = (int)first_len;
    op->old_text = NULL;
    op->new_text = NULL;
    op->next = NULL;
    // Executa
    join_lines(line_number);
    // Pilhas
    push_operation(&undo_stack, op);
    clear_stack(&redo_stack);
}

// Funções para undo, redo e contar linhas
void editor_undo(void) { undo(); }
void editor_redo(void) { redo(); }
size_t editor_line_count(void) { return doc ? doc->line_count : 0; }
