#ifndef TEXTEDITOR_TERMINAL_H
#define TEXTEDITOR_TERMINAL_H

#define LITTLE_BUFF_LENGTH 32
#define CTRLKEY(x) (x & 0x9f)
#define CLEAR_SCREEN() write(STDOUT_FILENO, "\x1b[2J", 4);
#define REPOS_CURSOR() write(STDOUT_FILENO, "\x1b[H", 3);

#include <termios.h>

typedef enum cursor_direction {
  TOP, BOTTOM, RIGHT, LEFT
} cursor_direction;

void ter_init();
void ter_clear_screen();
int ter_load_window_size(unsigned int *rows, unsigned int *cols);
void ter_move_cursor(cursor_direction d);
void ter_refresh();
void ter_print();

#endif //TEXTEDITOR_TERMINAL_H
