#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdint.h>


#define H_LINE "─"
#define V_LINE "│"
#define RD_COR "┌"
#define LD_COR "┐"
#define RU_COR "└"
#define LU_COR "┘"

#define CHAR_SIZE 5

#define BOX_COL     "#aa7c6a"
#define TITLE_COL   "#f6f1ef"

#define GRUV_RED    "#cc241d"
#define GRUV_GREEN  "#98971a"
#define GRUV_YELLOW "#d79921"
#define GRUV_BLUE   "#458588"
#define GRUV_PURPLE "#b16286"
#define GRUV_AQUA   "#689d6a"
#define GRUV_ORANGE "#d65d0e"

char COLORS[7][8] = {
    GRUV_RED, GRUV_GREEN, GRUV_YELLOW, GRUV_BLUE, GRUV_PURPLE, GRUV_AQUA, GRUV_ORANGE
};

/*
 * BASE LAYOUT : BOXES
 *   ↓
 * GENERATE CONTENT
 *   ↓
 * COMPOSE FINAL DISPLAY
 *   ↓
 * PRINT
 */ 

void init_ui();

void put_cursor(int x, int y);

void draw_box(int x, int y, int width, int height, char *title);

void print_file_blocks(char *file_path, int x, int y, int grid_width, int grid_height);

void set_hex_color(char *hex, int is_fg);

void display_ui();