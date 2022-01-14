//
//  wordle-solver.h
//  wordle-solver
//
//  Created by Ethan Bordeaux on 1/11/22.
//

#ifndef wordle_solver_h
#define wordle_solver_h

#include <stdio.h>
#include <stdbool.h>

// protect against loading malformed dictionary for nefarious purposes
#define MAX_DICTIONARY_SIZE         20000

#define WORDLE_WORD_SIZE            5

typedef struct s_letter_guess
{
    char letter;
    char eliminated_letters[26];
} s_letter_guess;

typedef struct s_wordle_state
{
    char           starting_word[WORDLE_WORD_SIZE+1];
    s_letter_guess word[5];
    char           letters_in_word[WORDLE_WORD_SIZE+1];    // letters we know are in the word but have not been properly placed
} s_wordle_state;

void ws_init(s_wordle_state *wordle_state,
             char           *start_guess);

int ws_make_guess(s_wordle_state *wordle_state,
                  char           *target_word,
                  char           dictionary[MAX_DICTIONARY_SIZE][WORDLE_WORD_SIZE+1],
                  int            dictionary_entries,
                  bool           verbose);

void ws_debug_print(bool verbose,
                    const char *fmt, ...);

#endif /* wordle_solver_h */
