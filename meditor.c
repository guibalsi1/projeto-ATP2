//
// Created by Guilherme on 10/10/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Line{
    char *content;
    size_t size;
    struct Line *next, *prev;
} Line;

typedef struct {
    Line *lines_head, *lines_tail, *lines;
    size_t line_count;
} Document;

static Document *doc = NULL;

static void create_document() {
    if (doc == NULL) {
        doc = (Document *)malloc(sizeof(Document));
        doc->lines = NULL;
        doc->lines_head = NULL;
        doc->lines_tail = NULL;
        doc->line_count = 0;
    }
}

void add_line(const char *text, int position) {
    create_document();
    position--; // Convert to 0-based index
    // Check for null inputs
    if (text == NULL) {
        return;
    }

    // Check if we should replace an existing line
    if (doc->line_count > 0 && position >= 0 && position < doc->line_count) {
        // Find the line at the specified position
        Line *current = doc->lines_head;
        for (int i = 0; i < position && current != NULL; i++) {
            current = current->next;
        }

        if (current != NULL) {
            // Replace the content of the existing line
            free(current->content);
            current->size = strlen(text);
            current->content = (char *)malloc(current->size + 1);
            if (current->content != NULL) {
                strcpy(current->content, text);
            }
        }
        return;
    }

    // Original insertion logic for new lines
    Line *new_line = (Line *)malloc(sizeof(Line));
    if (new_line == NULL) {
        return; // Memory allocation failed
    }

    new_line->size = strlen(text);
    new_line->content = (char *)malloc(new_line->size + 1);
    if (new_line->content == NULL) {
        free(new_line);
        return; // Memory allocation failed
    }
    strcpy(new_line->content, text);
    new_line->next = NULL;
    new_line->prev = NULL;

    if (doc->line_count == 0) {
        doc->lines_head = new_line;
        doc->lines_tail = new_line;
        doc->lines = new_line;
    } else if (position <= 0) {
        new_line->next = doc->lines_head;
        if (doc->lines_head != NULL) {
            doc->lines_head->prev = new_line;
        }
        doc->lines_head = new_line;
    } else if (position >= doc->line_count) {
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
        if (current != NULL) {
            new_line->next = current;
            new_line->prev = current->prev;
            if (current->prev != NULL) {
                current->prev->next = new_line;
            }
            current->prev = new_line;
        }
    }
    doc->line_count++;
}

void append_text(int line_number, const char *text) {
    create_document();
    line_number--; // Convert to 0-based index
    if (text == NULL || line_number < 0 || line_number >= doc->line_count) {
        return; // Invalid input or line number out of bounds
    }

    // Find the line at the specified position
    Line *line = doc->lines_head;
    for (int i = 0; i < line_number && line != NULL; i++) {
        line = line->next;
    }

    if (line == NULL) {
        return; // Line not found
    }

    size_t new_text_length = strlen(text);
    size_t new_size = line->size + new_text_length;

    char *new_content = (char *)realloc(line->content, new_size + 1);
    if (new_content == NULL) {
        return; // Memory allocation failed
    }

    strcpy(new_content + line->size, text);
    line->content = new_content;
    line->size = new_size;
}

void remove_line(int position) {
    create_document();
    position--; // Convert to 0-based index
    if (doc->line_count == 0 || position < 0 || position >= doc->line_count) {
        return; // Invalid position or empty document
    }

    Line *current = doc->lines_head;
    for (int i = 0; i < position && current != NULL; i++) {
        current = current->next;
    }

    if (current != NULL) {
        if (current->prev != NULL) {
            current->prev->next = current->next;
        } else {
            doc->lines_head = current->next; // Update head if needed
        }
        if (current->next != NULL) {
            current->next->prev = current->prev;
        } else {
            doc->lines_tail = current->prev; // Update tail if needed
        }
        free(current->content);
        free(current);
        doc->line_count--;
    }
}

void display_document() {
    Line *current = doc->lines;
    size_t line_number = 1;
    while (current != NULL) {
        printf("%4zu| %s\n", line_number++, current->content);
        current = current->next;
    }
}

void free_document() {
    if (doc != NULL) {
        Line *current = doc->lines;
        while (current != NULL) {
            Line *next = current->next;
            free(current->content);
            free(current);
            current = next;
        }
        free(doc);
    }
}

void save_document(const char *filename) {
    create_document();
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to open file for writing");
        return;
    }
    Line *current = doc->lines;
    while (current != NULL) {
        fprintf(file, "%s\n", current->content);
        current = current->next;
    }
    fclose(file);
}

void load_document(const char *filename) {
    create_document();
    FILE *file = fopen(filename, "r");
    int aux = 0;
    if (file == NULL) {
        perror("Failed to open file for reading");
        return;
    }
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), file)) {
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline character
        add_line(buffer, aux++);
    }
    fclose(file);
}
