/*
 * RAID 5 Demo : Redundant Array of Independant Disks
 * @author neigebaie
 */

#include <getopt.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "draw.h"

#define BLOCK_SIZE 10
#define DRIVE_NB 5


struct {
    char name[128];
    long offset; // block offsets
    size_t size;
} typedef file;

static int helpme(char *argv_0);

char *o_in  = NULL;

// MAPPING.dat
// DRIVE_0.dat
// DRIVE_1.dat
// DRIVE_2.dat
// DRIVE_3.dat
// DRIVE_4.dat

// 0 1 2 3 P
// 4 5 6 P 7

int store_file(char *input_path, int strip_n) {

    FILE *input_f = fopen(input_path, "rb");
    if (input_f == NULL) {
        fprintf(stderr, "Error opening input file");
        return EXIT_FAILURE;
    }

    while (!feof(input_f)) {
        // READ STRIPE

        printf("READING STIPE N°%d\n", strip_n);
        int block_n = 0;
        size_t bytes_read = 0;
        uint8_t buffer[DRIVE_NB - 1][BLOCK_SIZE];
        uint8_t parity[BLOCK_SIZE];

        for (int i = 0; i < BLOCK_SIZE; i++) {
            parity[i] = 0;
        }

        for (block_n = 0; block_n < DRIVE_NB - 1; block_n++) {
            if (feof(input_f))
                break;
            
            bytes_read += fread(buffer[block_n], 1, BLOCK_SIZE, input_f);

            for (int i = 0; i < BLOCK_SIZE; i++) {
                parity[i] ^= buffer[block_n][i];
            }
        }

        printf("bytes read = %ld\n", bytes_read);

        if (bytes_read) {
            // WRITE TO DISKS
            block_n = 0;
            FILE *drive;
            char drive_name[128];

            for (int n = 0; n < DRIVE_NB; n++) {
                snprintf(drive_name, sizeof(drive_name), "drive_%d.dat", n);
                drive = fopen(drive_name, "ab");

                if (strip_n % DRIVE_NB == DRIVE_NB - 1 - n) {
                    fwrite(parity, 1, BLOCK_SIZE, drive);
                } else {
                    fwrite(buffer[block_n], 1, BLOCK_SIZE, drive);
                    block_n++;
                }
                
                fclose(drive);
            }

            strip_n++;
        }
        
    }
    
    // Close the input file
    fclose(input_f);

    return 0;
}

int main(int argc, char *argv[]) {
    int opt;

    while ((opt = getopt(argc, argv, "hiv:")) != EOF) {
        switch (opt) {
            case 'i': /* input file */
                o_in = optarg;
                break;
            case 'h': /* help */
                helpme(argv[0]);
                return EXIT_SUCCESS;
            case 'v': /* verbose/debug */
                print_test_blocks();
                return EXIT_SUCCESS;
            default:
                fprintf(stderr, "Usage: %s [-e] [-d] -i input -o output -k key\n", argv[0]);
                return EXIT_FAILURE;
        }
    }

    if (argc == 1) {
        helpme(argv[0]);
        return EXIT_SUCCESS;
    }

    if (o_in)
        store_file(o_in, 0);

    return EXIT_SUCCESS;
}

static int helpme(char *argv_0)
{
    printf("RAID 5 Demo https://github.com/neigebaie/raid_five\n\n\
store a file:	%s -e -i input -o output -k key\n\n\
options:", argv_0);
    printf("\n\
-h      this help page\n\
-i file input file\n");

    return 0;
}