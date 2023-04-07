/*
 * main.c - Contains the main() function and other general functions for the project
 * Author: Tristan Thomas
 * Date: 6-4-2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void process_args(int argc, char **argv, char *scheduler, char *mem_strategy, int *quantum, FILE **f);

int main(int argc, char *argv[]) {
    int quantum;
    char *scheduler = NULL, *mem_strategy = NULL;
    FILE *input_file = NULL;

    // process command line arguments
    process_args(argc, argv, scheduler, mem_strategy, &quantum, &input_file);

    free(scheduler);
    free(mem_strategy);
    fclose(input_file);

    return 0;
}

void process_args(int argc, char **argv, char *scheduler, char *mem_strategy, int *quantum, FILE **f) {
    scheduler = (char *) malloc(strlen(argv[4]));
    assert(scheduler);
    mem_strategy = (char *) malloc(strlen(argv[6]));
    assert(mem_strategy);
    *f = fopen(argv[2], "w");
    assert(f);

    strcpy(scheduler, argv[4]);
    strcpy(mem_strategy, argv[6]);
    *quantum = atoi(argv[8]);

}
