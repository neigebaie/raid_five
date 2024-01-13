#include "draw.h"

// int print_test_blocks() {
//     FILE *drive;
//     char drive_name[128];
//     uint8_t buffer[BLOCK_SIZE];
//     int strip_n = 0;
//     size_t bytes_read = 1;
//     size_t offset = 0;

//     while (bytes_read) {
//         for (int n = 0; n < DRIVE_NB; n++) {
//             snprintf(drive_name, sizeof(drive_name), "drive_%d.dat", n);
//             drive = fopen(drive_name, "rb");

//             if (fseek(drive, offset, SEEK_SET) != 0) {
//                 fprintf(stderr, "Error seeking to offset");
//                 fclose(drive);
//                 return EXIT_FAILURE;
//             }

//             bytes_read = fread(buffer, 1, BLOCK_SIZE, drive);
            
//             if (bytes_read) {
//                 if (!n)
//                     printf("   -- STRIP N°%d --\n", strip_n);
//                 printf("  - %s: ", drive_name);

//                 for (int b = 0; b < BLOCK_SIZE; b++) {
//                     // printf("%02d ", buffer[b]);
//                     print_color_block(buffer[b]);
//                 }
//                 printf("\n");
//             }

//             fclose(drive);
//         }

//         offset += BLOCK_SIZE;
//         strip_n++;
//         printf("\n");

//     }

//     return EXIT_SUCCESS;

// }

void print_file_blocks(char *file_path, int x, int y, int grid_width, int grid_height) {
    
    FILE *file;
    file = fopen(file_path, "rb");

    if (file == NULL) {
        perror("Error opening file");
    }

    // Seek to the end of the file and get the current position (which is the file size)
    fseek(file, 0, SEEK_END);
    long size = ftell(file);

    fclose(file);

    file = fopen(file_path, "rb");
    // fseek(file, 0, SEEK_SET);

    if (size == -1) {
        perror("Error getting file size");
    }

    // printf("size=%d \n", size);
    // printf("grid_width=%d \n", grid_width);
    // printf("grid_height=%d \n", grid_height);

    int buffer_size = (int)(size / ((grid_width / 3) * (grid_height / 2)));
    if (!buffer_size) buffer_size = 1;
    uint8_t *buffer = (uint8_t *)malloc(buffer_size * sizeof(uint8_t));
    
    // printf("buffer_size=%d \n", buffer_size);
    
    int r_x = x;
    int r_y = y;

    int block_n = 0;
    int bytes_read = 1;

    while (!feof(file) && bytes_read) {
        bytes_read = fread(buffer, 1, buffer_size, file);

        long block_sum = 0;
        for (int i = 0; i < bytes_read; i++) {
            block_sum += buffer[i];
        }
        // printf("read=%d \n", bytes_read);

        if (bytes_read) {
            uint8_t block_mean = (uint8_t)(block_sum / bytes_read);
            put_cursor(x + (block_n % (grid_width / 3))*3, y + (block_n / (grid_width / 3)) * 2);
            // printf("%s", COLORS[block_mean%7]);
            block_n++;
            set_hex_color("#282828", 1);
            set_hex_color(COLORS[block_mean%7], 0);
            printf("%02x", block_mean);
            reset_color();
            printf(" ");
        }
    }

    put_cursor(x, y + grid_height - 1);
    set_hex_color(BOX_COL, 1);
    printf("\"%s\" : %db", file_path, size);
    
    fclose(file);
}

// ONLY WORKS FOR TRUE COLOR TERMINALS
void set_hex_color(char *hex, int is_fg) {
    if (strlen(hex) == 7) { // the hashtag + 3 bytes 
        char hex_byte[3];
        snprintf(hex_byte, sizeof(hex_byte), "%c%c", hex[1], hex[2]);
        int red   = strtol(hex_byte, NULL, 16);
        snprintf(hex_byte, sizeof(hex_byte), "%c%c", hex[3], hex[4]);
        int green = strtol(hex_byte, NULL, 16);
        snprintf(hex_byte, sizeof(hex_byte), "%c%c", hex[5], hex[6]);
        int blue  = strtol(hex_byte, NULL, 16);

        // \033[38;2;<r>;<g>;<b>m     #Select RGB foreground color
        // \033[48;2;<r>;<g>;<b>m     #Select RGB background color
        printf("\033[%d;2;%d;%d;%dm", 48 - 10 * (is_fg > 0), red, green, blue);
    } else {
        fprintf(stderr, "hex_to_color only accepts 3 bytes values. #RRGGBB, got: %s", hex);
    }
}

void bold_on() {
    printf("\033[1m");
}

void bold_off() {
    printf("\033[21m");
}

void reset_color() {
    printf("\033[39m\033[49m");
}

void put_cursor(int x, int y) {
    printf("\033[%d;%df", y + 1, x + 1);
}

void clear_term() {
    printf("\033[2J");
}

void term_size(int *x, int *y) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    *x = w.ws_row;
    *y = w.ws_col;
}


void draw_box(int x, int y, int width, int height, char *title) {

    set_hex_color(BOX_COL, 1);

    // draw horizontal lines (two for loops to avoid setting cursor pos too often)
    put_cursor(x + 1, y);
    for (int i = x + 1; i < x + width - 1; i++) {
        printf("%s", H_LINE);
    }
    put_cursor(x + 1, y + height - 1);
    for (int i = x + 1; i < x + width - 1; i++) {
        printf("%s", H_LINE);
    }

    // draw vertical lines
    for (int j = y + 1; j < y + height - 1; j++) {
        put_cursor(x, j);
        printf("%s", V_LINE);
        put_cursor(x + width - 1, j);
        printf("%s", V_LINE);
    }

    // draw corners
    put_cursor(x, y);
    printf("%s", RD_COR);
    put_cursor(x + width - 1, y);
    printf("%s", LD_COR);
    put_cursor(x, y + height - 1);
    printf("%s", RU_COR);
    put_cursor(x + width - 1, y + height - 1);
    printf("%s", LU_COR);

    // draw title
    bold_on();
    set_hex_color(TITLE_COL, 1);
    put_cursor(x + 2, y);
    printf("%s", title);
    reset_color();
}


void init_ui(int drives, char *input_file) {
    int w_width, w_height;
    term_size(&w_height, &w_width);

    // Init screen matrix
    // char ***term = (char ***)malloc(w_height * sizeof(char **));
    // for (int i = 0; i < w_height; i++) {
    //     term[i] = (char **)malloc(w_width * sizeof(char *));
    //     for (int j = 0; j < w_width; j++) {
    //         term[i][j] = (char *)malloc(CHAR_SIZE); // Allocate space for UTF-8 character (up to 4 bytes) + null terminator
    //     }
    // }

    // // Initialize the array with values
    // for (int i = 0; i < w_height; i++) {
    //     for (int j = 0; j < w_width; j++) {
    //         snprintf(term[i][j], CHAR_SIZE, " ");
    //     }
    // }

    printf("\e[?25l");

    clear_term();

    // Draw top box for file blocks info
    char file_name[20];
    snprintf(file_name, sizeof(file_name), " INPUT FILE ");
    draw_box(0, 0, w_width - w_width / 4, w_height / 2, file_name);
    draw_box(w_width - w_width / 4, 0, w_width / 4, w_height / 2, " CONTROLS ");
    // print_file_blocks("test_blocks_10", 2, 2, w_width - 4, w_height / 2 - 4);
    print_file_blocks("test_blocks_10", 2, 2, 120, w_height / 2 - 3);


    // Draw bottom boxes for drive blocks info
    char drive_name[64];
    char drive_path[64];
    for (int i = 0; i < drives; i++) {
        snprintf(drive_name, sizeof(drive_name), " DRIVE %d ", i);
        snprintf(drive_path, sizeof(drive_path), "drive_%d.dat", i);
        draw_box(i * (w_width / 5), w_height / 2 + 1, w_width / 5, w_height / 2, drive_name);
        print_file_blocks(drive_path, i * (w_width / 5) + 2, w_height / 2 + 1 + 2, w_width / 5 - 4, w_height / 2 - 4);
    }

    char author[] = " RAID 5 Demo by neigebaie ";
    put_cursor(w_width / 2 - strlen(author) / 2, 0);
    set_hex_color(BOX_COL, 1);
    printf("%s", author);

    fflush(stdout);

    // draw_term(term, w_width, w_height);

    // Free the allocated memory
    // for (int i = 0; i < w_height; i++) {
    //     for (int j = 0; j < w_width; j++) {
    //         free(term[i][j]);
    //     }
    //     free(term[i]);
    // }
    // free(term);
}


// Signal handler function for Ctrl+C
void exit_program(int signum) {
    clear_term();
    printf("\e[?25h");
    put_cursor(0, 0);
    exit(signum);
}


void main() {

    signal(SIGINT, exit_program);

    init_ui(5, "test.dat");

    usleep(100000000); // sleeps 100s

    exit_program(0);
}