cmake_minimum_required(VERSION 3.15)
project(texteditor C)

set(CMAKE_C_STANDARD 99)

add_executable(texteditor main.c terminal.c terminal.h utils.c utils.h text_rows.c text_rows.h app.c app.h file.c file.h)