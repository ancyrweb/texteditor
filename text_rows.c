#include <stdlib.h>
#include <string.h>
#include "text_rows.h"
#include "app.h"

void tr_reallocate(unsigned int next_rows_count) {
  app_state->text_rows = realloc(app_state->text_rows, sizeof(text_row) * (next_rows_count));

  for (int i = app_state->nb_text_rows + 1; i <= next_rows_count; i++) {
    app_state->text_rows[i - 1].data = malloc(sizeof(char) * TR_LINE_BUFF_SIZE);
    app_state->text_rows[i - 1].buff_len = TR_LINE_BUFF_SIZE;
    app_state->text_rows[i - 1].data_len = 0;
  }
  app_state->nb_text_rows = next_rows_count;
}

void tr_insert_character(char c) {
  if (app_state->cy >= app_state->nb_text_rows) {
    tr_reallocate(app_state->cy + 1);
  }

  // TODO check buffer size for line
  if (app_state->text_rows[app_state->cy].data[app_state->cx] == '\0') {
    app_state->text_rows[app_state->cy].data[app_state->cx] = c;
    app_state->text_rows[app_state->cy].data[app_state->cx + 1] = '\0';
    app_state->text_rows[app_state->cy].data_len++;
    app_state->cx++;
  } else {
    char *copy = malloc(sizeof(char) * app_state->text_rows[app_state->cy].data_len);
    memcpy(copy, app_state->text_rows[app_state->cy].data, app_state->text_rows[app_state->cy].data_len);

    for (int i = 0; i < app_state->text_rows[app_state->cy].data_len + 1; i++) {
      char next_char;
      if (i == app_state->cx) {
        next_char = c;
      } else if (i > app_state->cx) {
        next_char = copy[i - 1];
      } else {
        next_char = copy[i];
      }

      app_state->text_rows[app_state->cy].data[i] = next_char;
    }

    app_state->text_rows[app_state->cy].data_len++;
    app_state->cx++;
    free(copy);
  }
}

void tr_remove_character() {
  if (app_state->cx == 0) {
    if (app_state->cy == 0) {
      // Can't go back more
      return;
    }

    // Must move the current line into the previous one.
    memcpy(
        app_state->text_rows[app_state->cy - 1].data + app_state->text_rows[app_state->cy - 1].data_len,
        app_state->text_rows[app_state->cy].data,
        app_state->text_rows[app_state->cy].data_len
    );

    app_state->cx = app_state->text_rows[app_state->cy - 1].data_len;
    app_state->text_rows[app_state->cy - 1].data_len += app_state->text_rows[app_state->cy].data_len;
    app_state->cy--;

    // Delete the line
    tr_del_row(app_state->cy + 1);
  } else {
    char *copy = malloc(sizeof(char) * app_state->text_rows[app_state->cy].data_len);
    memcpy(copy, app_state->text_rows[app_state->cy].data, app_state->text_rows[app_state->cy].data_len);

    int index_to_delete = app_state->cx - 1;
    for (int i = 0, j = 0; i <= app_state->text_rows[app_state->cy].data_len; i++) {
      if (i != index_to_delete) {
        app_state->text_rows[app_state->cy].data[j] = copy[i];
        j++;
      }
    }

    app_state->text_rows[app_state->cy].data_len--;
    if (app_state->text_rows[app_state->cy].data_len > 0) {
      app_state->text_rows[app_state->cy].data[app_state->text_rows[app_state->cy].data_len] = '\0';
    }
    app_state->cx--;
    free(copy);
  }
}

void tr_breakline() {
  tr_reallocate(app_state->nb_text_rows + 1);
  for (unsigned int i = app_state->nb_text_rows - 1; i > 0; i--) {
    if (i - 1 == app_state->cy) {
      // First, we copy the remaining text into the second line
      unsigned int buflen = app_state->text_rows[i - 1].data_len - app_state->cx + 1; // +1 for \0
      memcpy(app_state->text_rows[i].data, app_state->text_rows[i - 1].data + app_state->cx, buflen);
      app_state->text_rows[i].data_len = buflen - 1;

      // Second, we clear the broke text from the first line
      app_state->text_rows[i - 1].data[app_state->cx] = '\0';
      app_state->text_rows[i - 1].data_len = app_state->cx;
    } else if (i - 1 > app_state->cy) {
      app_state->text_rows[i].data_len = app_state->text_rows[i - 1].data_len;
      memcpy(app_state->text_rows[i].data, app_state->text_rows[i - 1].data, app_state->text_rows[i - 1].data_len + 1);
    }
  }

  app_state->cy++;
  app_state->cx = 0;
}

void tr_del_row(unsigned int row_nb) {
  text_row *next_rows = malloc(sizeof(text_row) * app_state->nb_text_rows - 1);
  for (int i = 0, j = 0; i < app_state->nb_text_rows; i++) {
    if (i != row_nb) {
      memcpy(&next_rows[j], &app_state->text_rows[i], sizeof(text_row));
      j++;
    }
  }

  app_state->nb_text_rows--;
  free(app_state->text_rows);
  app_state->text_rows = next_rows;
}