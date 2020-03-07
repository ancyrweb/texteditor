#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "app.h"
#include "terminal.h"
#include "file.h"

// --- Private
char wait_input() {
  char c;
  int readed = read(STDIN_FILENO, &c, 1);
  if (readed == 0)
    return -1;

  return c;
}

// --- Public
void app_init(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "You must provide a filename.\r\n");
    exit(1);
  }

  app_state = malloc(sizeof(struct appstate));

  // Initialize terminal
  ter_clear_screen();
  ter_init();
  ter_load_window_size(&app_state->rows, &app_state->cols);
  app_state->cx = 0;
  app_state->cy = 0;

  // Initialize rows
  app_state->filename = strdup(argv[1]);
  file_load();

  ter_clear_screen();
  ter_refresh();
}

int app_process_input() {
  char key = wait_input();
  if (key == CTRLKEY('q') || key == CTRLKEY('c')) {
    return -1;
  } else if (key == CTRLKEY('s')) {
    file_save();
  } else if (key == -1 || iscntrl(key) || key == 91) {
    if (key == 127) {
      tr_remove_character();
      ter_refresh();
    } else if (key == 13) {
      tr_breakline();
      ter_refresh();
    }

    return 0;
  } else if (key == 65) {
    ter_move_cursor(TOP);
  } else if (key == 66) {
    ter_move_cursor(BOTTOM);
  } else if (key == 67) {
    ter_move_cursor(RIGHT);
  } else if (key == 68) {
    ter_move_cursor(LEFT);
  } else {
    tr_insert_character(key);
    ter_refresh();
  }
  return 0;
}