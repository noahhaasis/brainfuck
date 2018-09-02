/*
 * A simple interpreter for the brainfuck language.
 *
 * TODO:
 *  - Remove possible integer overflow bugs
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stack.h"

#define INITIAL_TAPE_LENGTH 512

typedef struct {
    FILE *code_stream;
    stack_t *positions;
    int *tape;
    unsigned long tape_len;
    unsigned long tape_pos;
} program_t;

void skip_loop(program_t *program);

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: ./brainfuck <program>\n");
        return 1;
    }

    program_t program = {
        fopen(argv[1], "r"),
        stack_create(),
        calloc(INITIAL_TAPE_LENGTH, 1),
        INITIAL_TAPE_LENGTH
    };
    if (!program.code_stream) {
        fprintf(stderr, "Unable to open file %s\n", argv[1]);
        stack_destroy(program.positions);
        return 1;
    }
    if (!program.tape) {
        fprintf(stderr, "Unable to allocate a tape\n");
        fclose(program.code_stream);
        stack_destroy(program.positions);
        return 1;
    }

    char c, temp;
    fpos_t *curr_pos;
    while ((c = fgetc(program.code_stream)) != EOF) {
        switch (c) {
        case '[': // Jump to the matching ']' if the current cell is equal to 0
            if (!program.tape[program.tape_pos]) {
                skip_loop(&program);
            }
            curr_pos = malloc(sizeof(fpos_t));
            fgetpos(program.code_stream, curr_pos);
            stack_push(program.positions, curr_pos);
            break;
        case ']': // Jump to the lasting open bracket
            curr_pos = (fpos_t *)stack_peek(program.positions);
            if (!curr_pos) {
                fprintf(stderr, "Error: unmatched ']'!\n");
                goto error;
            }
            if (program.tape[program.tape_pos]) {
                fsetpos(program.code_stream, curr_pos);
            } else {
                free(stack_pop(program.positions));
            }
            break;
        case '+': // Increment
            program.tape[program.tape_pos]++;
            break;
        case '-': // Decrement
            program.tape[program.tape_pos]--;
            break;
        case '<': // Move left
            if (program.tape_pos < 0) {
                fprintf(stderr, "Error: Cannot move beyond beginning of tape!\n");
                goto error;
            }
            program.tape_pos--;
            break;
        case '>': // Move right
            program.tape_pos++;
            if (program.tape_pos >= program.tape_len) {
                program.tape_len *= 2;
                program.tape = realloc(program.tape, program.tape_len);
                if (!program.tape) {
                    fprintf(stderr, "Out of memory\n");
                    goto error;
                }
                // Initialize the new part of the tape to zero
                memset(program.tape + program.tape_len/2, 0, program.tape_len/2);
            }
            break;
        case '.': // Output
            putchar(program.tape[program.tape_pos]);
            break;
        case ',': // Input
            if ((temp = getchar()) != EOF) {
                program.tape[program.tape_pos] = temp;
            }
            break;
        default: // Everything which isn't a command is considered a comment
            break;
        }
    }

    fclose(program.code_stream);
    stack_destroy_with_elements(program.positions);
    free(program.tape);
    return 0;

error:
    fclose(program.code_stream);
    stack_destroy_with_elements(program.positions);
    free(program.tape);
    return 1;
}

void skip_loop(program_t *program) {
    int unclosed_scopes_count = 1;
    for (;;) {
        char c = fgetc(program->code_stream);
        if (c == '[') {
            unclosed_scopes_count++;
        } else if(c == ']') {
            unclosed_scopes_count--;
        }
        if (unclosed_scopes_count == 0) {
            break;
        }
    }
    fgetc(program->code_stream);
}
