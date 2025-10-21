#include <stdio.h>
#include "meditor.h"

int main(void) {
    load_document("example.txt");
    printf("Documento original:\n");
    display_document();

    add_line("First line of the document.", 1);
    add_line("Second line of the document.", 2);
    add_line("Third line of the document.", 3);

    printf("\nApós adicionar linhas:\n");
    display_document();

    // Agora fazemos o append na linha 2 (que existe)
    append_text(2, " - texto adicionado!");

    printf("\nApós append na linha 2:\n");
    display_document();

    save_document("output.txt");
    free_document();
    return 0;
}