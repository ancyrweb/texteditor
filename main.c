#include "app.h"

int main(int argc, char** argv) {
  app_init(argc, argv);

  while (1) {
    int result = app_process_input();
    if (result == -1)
      break;
  }
  return 0;
}