#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "terminal.h"
#include "app.h"

// --- Private
void ter_clear() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &app_state->original_term);
  ter_clear_screen();
}

// --- Public
void ter_init() {
  tcgetattr(STDIN_FILENO, &app_state->original_term);
  tcgetattr(STDIN_FILENO, &app_state->term);

  app_state->term.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
  app_state->term.c_iflag &= ~(IXON | ICRNL | ISTRIP | INPCK | BRKINT);
  app_state->term.c_oflag &= ~(OPOST);
  app_state->term.c_cflag |= CS8;
  app_state->term.c_cc[VMIN] = 0;
  app_state->term.c_cc[VTIME] = 1;

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &app_state->term);
  atexit(ter_clear);
}

void ter_clear_screen() {
  CLEAR_SCREEN()
  REPOS_CURSOR()
}

int ter_load_window_size(unsigned int *rows, unsigned int *cols) {
  // Position the cursor to the bottom right
  write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12);
  // Ask for cursor position
  write(STDOUT_FILENO, "\x1b[6n", 4);

  char buf[LITTLE_BUFF_LENGTH];
  int i = 0;
  while (i < LITTLE_BUFF_LENGTH - 1) {
    if ((read(STDIN_FILENO, &buf[i], 1)) != 1) break;
    if (buf[i] == 'R') break;
    i++;
  }
  buf[i] = '\0';

  if (buf[0] != '\x1b' || buf[1] != '[') return -1;
  if (sscanf(&buf[2], "%d;%d", rows, cols) == -1) return -1;
  return 0;
}

void ter_move_cursor(cursor_direction d) {
  switch (d) {
    case TOP:
      if (app_state->cy == 0)
        break;

      app_state->cy--;
      write(STDOUT_FILENO, "\x1b[1A", 4);

      if (app_state->text_rows[app_state->cy].data_len <= app_state->cx) {
        app_state->cx = app_state->text_rows[app_state->cy].data_len;
        printf("\x1b[999D\x1b[%dC",app_state->cx);
        fflush(stdout);
      }
      break;
    case BOTTOM:
      if ((app_state->cy + 1) == app_state->nb_text_rows) {
        // We are at the bottomest line. Moving bottom moves the cursor to the end of the string.
        int delta = app_state->text_rows[app_state->cy].data_len - app_state->cx;
        if (delta == 0)
          break;

        app_state->cx = app_state->text_rows[app_state->cy].data_len;
        printf("\x1b[%dC", delta);
        fflush(stdout);
        break;
      }

      app_state->cy++;
      write(STDOUT_FILENO, "\x1b[1B", 4);

      if (app_state->text_rows[app_state->cy].data_len <= app_state->cx) {
        // There is text on the next line and the previous line is larger
        // We move the cursor to the end of the line

        printf("\x1b[999D");
        app_state->cx = app_state->text_rows[app_state->cy].data_len;
        if (app_state->cx > 0) {
          printf("\x1b[%dC",app_state->cx);
        }
        fflush(stdout);
      }

      break;
    case RIGHT:
      if (app_state->cx == app_state->cols)
        break;

      if (app_state->nb_text_rows < (app_state->cy + 1))
        break;

      if (app_state->text_rows[app_state->cy].data_len <= app_state->cx)
        break;

      app_state->cx++;
      write(STDOUT_FILENO, "\x1b[1C", 4);
      break;
    case LEFT:
      if (app_state->cx == 0) {
        if (app_state->cy == 0) {
          break;
        }

        app_state->cy--;
        app_state->cx = app_state->text_rows[app_state->cy].data_len;
        printf("\x1b[1A\x1b[999D");
        if (app_state->cx > 0) {
          printf("\x1b[%dC",app_state->cx);
        }
        fflush(stdout);
        break;
      }

      app_state->cx--;
      write(STDOUT_FILENO, "\x1b[1D", 4);
      break;
    default:
      break;
  }
}

void ter_refresh() {
  CLEAR_SCREEN()
  REPOS_CURSOR();
  ter_print();
  // CSI H is 1-based, therefore top left is at 1;1.
  printf("\x1b[%d;%dH", app_state->cy + 1, app_state->cx + 1);
  fflush(stdout);
}

void ter_print() {
  char buf[1000];
  text_row *r;

  for (int i = 0; i < app_state->nb_text_rows; i++) {
    r = app_state->text_rows + i;
    if (r->data_len == 0) {
      // No data in this row, just print a line break
      write(STDOUT_FILENO, "\r\n", 2);
      continue;
    }

    sprintf(buf, "%s\r\n", r->data);
    write(STDOUT_FILENO, buf, strlen(r->data) + 2);
  }
}