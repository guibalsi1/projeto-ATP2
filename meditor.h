//
// Created by Guilherme on 10/10/25.
//

#ifndef PROJETO_1_MEDITOR_H
#define PROJETO_1_MEDITOR_H

void display_document(void);
void free_document(void);
void save_document(const char *filename);
void load_document(const char *filename);
void editor_insert_line(int line_number, const char *text);
void editor_append_text(int line_number, const char *text);
void editor_remove_line(int line_number);
void editor_split_line(int line_number, int split_position);
void editor_join_lines(int line_number);
void editor_undo(void);
void editor_redo(void);
void editor_replace_line(int line_number, const char *text);
const char *editor_get_line(int line_number);

#endif //PROJETO_1_MEDITOR_H