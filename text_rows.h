#ifndef TEXTEDITOR_TEXT_ROWS_H
#define TEXTEDITOR_TEXT_ROWS_H

#define TR_LINE_BUFF_SIZE 512

// Text_Rows
// ---
// Manage the rows of text displayed on the screen

typedef struct text_row {
  char *data;
  unsigned long buff_len; // size of the reserved memory
  unsigned long data_len; // size of the string itself
} text_row;

void tr_reallocate(unsigned int next_rows);
void tr_insert_character(char c);
void tr_remove_character();
void tr_breakline();
void tr_del_row(unsigned int row_nb);


#endif //TEXTEDITOR_TEXT_ROWS_H
