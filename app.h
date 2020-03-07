#ifndef TEXTEDITOR_APP_H
#define TEXTEDITOR_APP_H

#include <termios.h>
#include "text_rows.h"

struct appstate {
  struct termios original_term;
  struct termios term;

  // window height and width
  unsigned int rows;
  unsigned int cols;

  unsigned int cx;
  unsigned int cy;

  unsigned int nb_text_rows;
  text_row *text_rows;

  char* filename;
};

struct appstate *app_state;

void app_init(int argc, char** argv);
int app_process_input();

#endif //TEXTEDITOR_APP_H
