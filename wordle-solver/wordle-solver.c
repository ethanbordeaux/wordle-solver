//
//  wordle-solver.c
//  wordle-solver
//
//  Created by Ethan Bordeaux on 1/11/22.
//

#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

#include "wordle-solver.h"

void ws_init(s_wordle_state *wordle_state,
             char           *start_guess)
{
    for(int i=0; i<WORDLE_WORD_SIZE; i++)
    {
        wordle_state->word[i].letter = '_';
        for(int j=0; j<26; j++)
        {
            wordle_state->word[i].eliminated_letters[j] = 0;
        }
        wordle_state->letters_in_word[i] = 0;
    }
    if(start_guess != NULL && strlen(start_guess) == WORDLE_WORD_SIZE)
    {
        strcpy(wordle_state->starting_word, start_guess);
    }
}

int ws_make_guess(s_wordle_state *wordle_state,
                  char           *target_word,
                  char           dictionary[MAX_DICTIONARY_SIZE][WORDLE_WORD_SIZE+1],
                  int            dictionary_entries,
                  bool           verbose)
{
    char guess[WORDLE_WORD_SIZE+1] = {0};
    bool is_word_known = true;
    int letter_count[26][WORDLE_WORD_SIZE] = {0};
    
    for(int i=0; i<WORDLE_WORD_SIZE; i++)
    {
        guess[i] = wordle_state->word[i].letter;
        if(wordle_state->word[i].letter == '_')
        {
            is_word_known = false;
        }
    }
    guess[WORDLE_WORD_SIZE] = '\0';
    
    if(is_word_known)
    {
        printf("word found!  it's '%s'\n", guess);
        return 1;
    }
    
    // on first turn if seeded with starting word use it
    if(strlen(wordle_state->starting_word) == 5)
    {
        // use seed for first word, then cancel it out;
        strcpy(guess, wordle_state->starting_word);
        memset(wordle_state->starting_word, 0, 6);
    }
    else
    {
        // recalculate probabilities based on current state of guesses
        for(int i=0; i<WORDLE_WORD_SIZE; i++)
        {
            if(guess[i] == '_')
            {
                for(int j=0; j<dictionary_entries; j++)
                {
                    if(wordle_state->word[i].eliminated_letters[dictionary[j][i]-'a'] == 0)
                    {
                        letter_count[dictionary[j][i]-'a'][i]++;
                    }
                }
            }
        }
        
        // find word in dictionary that matches these requirements and optimizes likelyhood letters are used
        // in those locations.
        int i_guess = 0;
        int max_letter_count = 0, curr_letter_count = 0;
        for(int i=0; i<dictionary_entries; i++)
        {
            bool possible_match = true;
            for(int j=0; j<WORDLE_WORD_SIZE; j++)
            {
                // if known letter doesn't match current word, skip immediately
                if(guess[j] != '_')
                {
                    if(guess[j] != dictionary[i][j])
                    {
                        possible_match = false;
                        break;
                    }
                }
                
                // if letter is already eliminated, skip immediately
                if(wordle_state->word[j].eliminated_letters[(dictionary[i][j]-'a')] == 1)
                {
                    possible_match = false;
                    break;
                }
            }
            
            if(!possible_match)
            {
                continue;
            }
            
            // make sure word contains letters we know are a part of word
            bool letter_found = true;
            if(wordle_state->letters_in_word[0] != 0)
            {
                int j = 0;
                while(wordle_state->letters_in_word[j] != 0)
                {
                    letter_found = false;
                    if(wordle_state->letters_in_word[j] != 0)
                    {
                        for(int k=0; k<WORDLE_WORD_SIZE; k++)
                        {
                            if(dictionary[i][k] == wordle_state->letters_in_word[j])
                            {
                                letter_found = true;
                            }
                        }
                        if(!letter_found)
                        {
                            break;
                        }
                    }
                    j++;
                }
            }
            if(!letter_found)
            {
                continue;
            }
            
            // word is candidate, compute how common letters are a part of all valid words
            curr_letter_count = 0;
            for(int j=0; j<WORDLE_WORD_SIZE; j++)
            {
                curr_letter_count += letter_count[dictionary[i][j]-'a'][j];
            }
            if(curr_letter_count > max_letter_count)
            {
                max_letter_count = curr_letter_count;
                i_guess = i;
            }
        }
        strcpy(guess, &dictionary[i_guess][0]);
    }

    ws_debug_print(verbose, "best guess is %s\n", guess);
    
    if(!strcmp(guess, target_word))
    {
        // found it!
        for(int j=0; j<WORDLE_WORD_SIZE; j++)
        {
            wordle_state->word[j].letter = guess[j];
        }
        return 1;
    }
    else
    {
        // update eliminated and letters in word states
        for(int j=0; j<WORDLE_WORD_SIZE; j++)
        {
            if(guess[j] == target_word[j])
            {
                wordle_state->word[j].letter = guess[j];
            }
            else
            {
                // is this letter anywhere in the word?
                bool is_letter_in_word = false;
                for(int k=0; k<WORDLE_WORD_SIZE; k++)
                {
                    if(guess[j] == target_word[k])
                    {
                        is_letter_in_word = true;
                    }
                }
                // if letter is in word, just eliminate for where the guess happened
                // otherwise elimiate for all locations
                if(is_letter_in_word)
                {
                    wordle_state->word[j].eliminated_letters[guess[j]-'a'] = 1;
                    ws_debug_print(verbose, "eliminating %c from location %d\n", guess[j], j);
                    int i_letters_in_word = 0;
                    
                    // make sure this letter isn't already maintained in list
                    bool is_letter_known = false;
                    for(int i=0; i<WORDLE_WORD_SIZE; i++)
                    {
                        if(guess[j] == wordle_state->letters_in_word[i])
                        {
                            is_letter_known = true;
                        }
                    }
                    
                    // save known but misplaced letter in state
                    if(!is_letter_known)
                    {
                        while(wordle_state->letters_in_word[i_letters_in_word] != 0)
                        {
                            i_letters_in_word++;
                        }
                        wordle_state->letters_in_word[i_letters_in_word] = guess[j];
                    }
                }
                else
                {
                    for(int i=0; i<WORDLE_WORD_SIZE; i++)
                    {
                        wordle_state->word[i].eliminated_letters[guess[j]-'a'] = 1;
                    }
                    ws_debug_print(verbose, "eliminating %c from all locations\n", guess[j]);
                }
            }
        }
    }    
    return 0;
}

void ws_debug_print(bool verbose,
                    const char *fmt, ...)
{
    if(!verbose)
    {
        return;
    }
    va_list va;
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
}
