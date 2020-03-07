#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "app.h"

void file_load() {
  FILE *f = fopen(app_state->filename, "r");
  if (f == NULL) {
    fclose(f);
    return;
  }

  unsigned int lines = 1;
  size_t longest_line_len = TR_LINE_BUFF_SIZE;
  unsigned int line_len = 0;

  char c;
  while ((c = fgetc(f)) != EOF) {
    line_len++;
    if (c == '\n') {
      lines++;
      if (line_len > longest_line_len) {
        longest_line_len = line_len;
      }
      line_len = 0;
    }
  }

  app_state->nb_text_rows = lines;
  app_state->text_rows = malloc(sizeof(text_row) * lines);
  fseek(f, 0, SEEK_SET);

  char *buf = malloc(sizeof(char) * longest_line_len);
  for (int i = 0; i < lines; i++) {
    int raw_size = getline(&buf, &longest_line_len, f);
    int size = raw_size - 2;

    app_state->text_rows[i].data_len = size;
    app_state->text_rows[i].data = malloc(sizeof(char) * TR_LINE_BUFF_SIZE);
    app_state->text_rows[i].buff_len = TR_LINE_BUFF_SIZE;
    memcpy(app_state->text_rows[i].data, buf, size);
    app_state->text_rows[i].data[size] = '\0';
  }

  free(buf);
  fclose(f);
}
void file_save() {
  FILE *f = fopen(app_state->filename, "w");
  if (f == NULL) {
    fprintf(stderr, "Could not open file.");
    fclose(f);
    return;
  }

  int buflen = 256;
  for (int i = 0; i < app_state->nb_text_rows; i++) {
    if (buflen < app_state->text_rows[i].buff_len) {
      buflen = app_state->text_rows[i].buff_len + 2;
    }
  }

  char *buf = malloc(sizeof(char) * buflen);
  for (int i = 0; i < app_state->nb_text_rows; i++) {
    int datalen = app_state->text_rows[i].data_len;
    memcpy(buf, app_state->text_rows[i].data, datalen);
    buf[datalen] = '\r';
    buf[datalen + 1] = '\n';
    buf[datalen + 2] = '\0';
    fputs(buf, f);
  }

  free(buf);
  fclose(f);
}