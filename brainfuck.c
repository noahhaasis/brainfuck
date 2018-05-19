#include <stdio.h>
#include "stack.h"

#define TAPE_LENGTH 1024

typedef struct {
    FILE *code_stream;
    stack_t *positions;
    int tape[TAPE_LENGTH];
    unsigned int tape_pos;
} program_t;

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: ./brainfuck <program>\n");
        return 1;
    }

    program_t program = {
        fopen(argv[1], "r"), 
        stack_create()
    };
    if (!program.code_stream) {
        fprintf(stderr, "Unable to open file %s\n", argv[1]);
        return 1;
    }

    char c, temp;
    fpos_t *curr_pos;
    while ((c = fgetc(program.code_stream)) != EOF) {
        switch (c) {
        case '[':
            curr_pos = malloc(1);
            fgetpos(program.code_stream, curr_pos);
            stack_push(program.positions, curr_pos);
            break;
        case ']':
            curr_pos = (fpos_t *)stack_peek(program.positions);
            if (!curr_pos) {
                fprintf(stderr, "Error: unmatched ']'!\n");
                stack_destroy_with_elements(program.positions);
                return 1;
            }
            if (program.tape[program.tape_pos]) {
                fsetpos(program.code_stream, curr_pos);
            } else {
                free(stack_pop(program.positions));
            }
            break;
        case '+':
            program.tape[program.tape_pos]++;
            break;
        case '-':
            program.tape[program.tape_pos]--;
            break;
        case '<':
            if (program.tape_pos == 0) {
                fprintf(stderr, "Error: Cannot move beyond beginning of tape!\n");
                stack_destroy_with_elements(program.positions);
                return 1;
            }
            program.tape_pos--;
            break;
        case '>':
            if (program.tape_pos == TAPE_LENGTH-1) {
                fprintf(stderr, "Error: Run out of tape space!\n");
                stack_destroy_with_elements(program.positions);
                return 1;
            }
            program.tape_pos++;
            break;
        case '.':
            putchar(program.tape[program.tape_pos]);
            break;
        case ',':
            if ((temp = getchar()) != EOF) {
                program.tape[program.tape_pos] = temp;
            }
            break;
        default:
            break;
        }
    }
    stack_destroy_with_elements(program.positions);

    return 0;
}
