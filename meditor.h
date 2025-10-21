//
// Created by Guilherme on 10/10/25.
//

#ifndef PROJETO_1_MEDITOR_H
#define PROJETO_1_MEDITOR_H

void add_line(const char *text, int position);
void append_text(int line_number, const char *text);
void remove_line(int position);
void display_document();
void free_document();
void save_document(const char *filename);
void load_document(const char *filename);

#endif //PROJETO_1_MEDITOR_H