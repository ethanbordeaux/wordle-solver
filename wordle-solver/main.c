//
//  main.c
//  world-solver
//
//  Created by Ethan Bordeaux on 1/9/22.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

#include "wordle-solver.h"
#include "wordle-words.h"

#define GAME_MODE_UNSET             -1

// test input dictionary against single word
#define GAME_MODE_SINGLE            0

// test against every word in input dictionary
#define GAME_MODE_FULL_DICT         1

// play n random games against input dictionary
#define GAME_MODE_RAND              2

// play against every word in input dictionary, modifying start word
#define GAME_MODE_START_OPTIMIZE    3

// play against Wordle dictionary (target words and full dictionary are different sizes)
#define GAME_MODE_WORDLE_CHECK      4

// used in dictionary validation
#define MAX_WORD_SIZE               100

/*
 *  scan entire dictionary to find the best start word
 */
void find_optimal_word(char dictionary[MAX_DICTIONARY_SIZE][WORDLE_WORD_SIZE+1],
                       int  dictionary_entries,
                       bool verbose)
{
    int i_best_starting_word_wins, i_best_average_score;
    float best_average_score = 100.0f;
    int best_starting_word_wins = 0;

    srand((unsigned int)time(NULL));
    
    for(int i=0; i<dictionary_entries; i++)
    {
        char target_word[WORDLE_WORD_SIZE+1];
        int game_count = 0, tot_games = dictionary_entries, min_guess = 10000, max_guess = 0, total_guess = 0;
        int six_or_less = 0;
        char min_guess_word[WORDLE_WORD_SIZE+1], max_guess_word[WORDLE_WORD_SIZE+1];
        
        int guesses_per_win[20] = {0};
        
        s_wordle_state wordle_state;

        for(int j=0; j<dictionary_entries; j++)
        {
            strcpy(target_word, &dictionary[j][0]);
            
            ws_init(&wordle_state, &dictionary[i][0]);
            
            ws_debug_print(verbose, "playing with starting word %s\n", wordle_state.starting_word);

            int guess_count = 0;
            int ret;
            do
            {
                ret = ws_make_guess(&wordle_state,
                                    target_word,
                                    dictionary,
                                    dictionary_entries,
                                    verbose);
                guess_count++;
                
            } while(!ret);
            
            if(guess_count < 20)
            {
                guesses_per_win[guess_count]++;
            }
            else
            {
                ws_debug_print(verbose, "whoa something went south here\n");
            }
            
            ws_debug_print(verbose, "found word in %d tries\n", guess_count);
            game_count++;
            if(guess_count > max_guess)
            {
                max_guess = guess_count;
                strcpy(max_guess_word, target_word);
            }
            if(guess_count < min_guess)
            {
                min_guess = guess_count;
                strcpy(min_guess_word, target_word);
            }
            total_guess += guess_count;
            
            if(guess_count <= WORDLE_WORD_SIZE+1)
            {
                six_or_less++;
            }
        }
        for(int i=1; i<20; i++)
        {
            printf("%d found in %d guesses\n", guesses_per_win[i], i);
        }
        printf("maximum guesses %d for word %s\n", max_guess, max_guess_word);
        printf("minimum guesses %d for word %s\n", min_guess, min_guess_word);
        printf("average guesses %f\n", ((float)total_guess)/(float)game_count);
        printf("won the wordle %2.2f%% of the time, fear the future\n", 100.0*((float)six_or_less/(float)tot_games));
    
        if(((float)total_guess)/(float)game_count < best_average_score)
        {
            i_best_average_score = i;
            best_average_score = ((float)total_guess)/(float)game_count;
            printf("new best average score %2.2f%% with %s\n", best_average_score, &dictionary[i][0]);
        }
        if(six_or_less > best_starting_word_wins)
        {
            best_starting_word_wins = six_or_less;
            i_best_starting_word_wins = i;
            printf("new best most wins %d with word %s\n", best_starting_word_wins, &dictionary[i][0]);
        }
    }
}

void print_help(void)
{
    printf("wordle-solver\n");
    printf("\n");
    printf("    -help          print help\n");
}

int main(int argc, const char * argv[])
{
    int dictionary_entries = 0;
    char dictionary[MAX_DICTIONARY_SIZE][WORDLE_WORD_SIZE+1];
    char target_word[WORDLE_WORD_SIZE+1];
    int game_mode = GAME_MODE_UNSET;
    bool verbose = false;
    int num_games = 0;
    
    int i_argv = 1;
    while(argv[i_argv] != NULL)
    {
        if(!strncmp(argv[i_argv], "-help", strlen("-help")))
        {
            print_help();
            return 0;
        }
        else if(!strncmp(argv[i_argv], "-single=", strlen("-single=")))
        {
            if(game_mode == GAME_MODE_UNSET)
            {
                strcpy(target_word, &argv[i_argv][strlen("-single=")]);
                printf("target word is %s\n", target_word);
                game_mode = GAME_MODE_SINGLE;
            }
            else
            {
                printf("unclear set of parameters trying to start multiple simulation types; exiting...\n");
                return 1;
            }
        }
        else if(!strncmp(argv[i_argv], "-full-dictionary", strlen("-full-dictionary")))
        {
            if(game_mode == GAME_MODE_UNSET)
            {
                printf("testing across full dictionary\n");
                game_mode = GAME_MODE_FULL_DICT;
            }
            else
            {
                printf("unclear set of parameters trying to start multiple simulation types; exiting...\n");
                return 1;
            }
        }
        else if(!strncmp(argv[i_argv], "-v", strlen("-v")))
        {
            verbose = true;
        }
        else if(!strncmp(argv[i_argv], "-rand=", strlen("-rand=")))
        {
            if(game_mode == GAME_MODE_UNSET)
            {
                char s_num_games[10];
                strncpy(s_num_games, &argv[i_argv][strlen("-rand=")], 9);
                num_games = atoi(s_num_games);
                game_mode = GAME_MODE_RAND;
            }
            else
            {
                printf("unclear set of parameters trying to start multiple simulation types; exiting...\n");
                return 1;
            }
        }
        else if(!strncmp(argv[i_argv], "-file=", strlen("-file=")))
        {
            if(game_mode == GAME_MODE_WORDLE_CHECK)
            {
                printf("should not be loading dictionary when using Wordle dictionaries; exiting...\n");
                return 1;
            }
            else
            {
                printf("attempting to load dictionary %s\n", &argv[i_argv][strlen("-file=")]);
                FILE *fp;
                fp = fopen(&argv[i_argv][strlen("-file=")], "r");
                if(!fp)
                {
                    printf("couldn't load %s, exiting...\n", argv[1]);
                    return 1;
                }
                
                char word[MAX_WORD_SIZE];
                
                // scan dictionary to determine how much space to allocate
                while(!feof(fp) && dictionary_entries < MAX_DICTIONARY_SIZE)
                {
                    if(fgets(word, MAX_WORD_SIZE, fp) != NULL)
                    {
                        if(word[strlen(word)-1] == '\n')
                        {
                            word[strlen(word)-1] = 0;
                        }
                        if(strlen(word) == WORDLE_WORD_SIZE)
                        {
                            // protect against adding words with punctuation
                            int skip_word = 0;
                            for(int i=0; i<WORDLE_WORD_SIZE; i++)
                            {
                                if(!isalpha(word[i]))
                                {
                                    skip_word = 1;
                                    break;
                                }
                            }
                            strcpy(&dictionary[dictionary_entries][0], word);
                            dictionary_entries++;
                        }
                    }
                }
                
                if(dictionary_entries >= MAX_DICTIONARY_SIZE)
                {
                    printf("dictionary is too large, exiting...\n");
                    return 1;
                }
            }
        }
        i_argv++;
    }
    
    // this one's too different so it gets it's own function
    if(game_mode == GAME_MODE_START_OPTIMIZE)
    {
        find_optimal_word(dictionary, dictionary_entries, verbose);
        return 0;
    }

    if(dictionary_entries == 0)
    {
        printf("no dictionary loaded; exiting...\n");
        return 1;
    }
    
    srand((unsigned int)time(NULL));
    
    int game_count = 0, tot_games = 0, min_guess = 10000, max_guess = 0, total_guess = 0;
    int six_or_less = 0;
    char min_guess_word[WORDLE_WORD_SIZE+1], max_guess_word[WORDLE_WORD_SIZE+1];
    
    int guesses_per_win[20] = {0};
        
    switch(game_mode)
    {
        case GAME_MODE_SINGLE:
            tot_games = 1;
            break;
        
        case GAME_MODE_FULL_DICT:
            tot_games = dictionary_entries;
            break;
            
        case GAME_MODE_RAND:
            tot_games = num_games;
            break;
            
        case GAME_MODE_WORDLE_CHECK:
            tot_games = LEN_WORDLE_TARGET_WORDS;
            break;
    }
    
    while(game_count < tot_games)
    {
        switch(game_mode)
        {
            // fetch next word to process
            case GAME_MODE_SINGLE:
                break;
            
            case GAME_MODE_FULL_DICT:
                strcpy(target_word, &dictionary[game_count][0]);
                break;
                
            case GAME_MODE_RAND:
                strcpy(target_word, &dictionary[rand()%dictionary_entries][0]);
                break;
                
            case GAME_MODE_WORDLE_CHECK:
                strcpy(target_word, &wordle_target_words[game_count][0]);
                break;
        }

        s_wordle_state wordle_state;

        ws_init(&wordle_state, "tromp");
        
        printf("searching for word %s\n", target_word);

        int guess_count = 0;
        int ret;
        do
        {
            if(game_mode == GAME_MODE_WORDLE_CHECK)
            {
                ret = ws_make_guess(&wordle_state,
                                    target_word,
                                    wordle_full_dictionary,
                                    dictionary_entries,
                                    verbose);
            }
            else
            {
                ret = ws_make_guess(&wordle_state,
                                    target_word,
                                    dictionary,
                                    dictionary_entries,
                                    verbose);
            }
            guess_count++;
            
        } while(!ret);
        
        if(guess_count < 20)
        {
            guesses_per_win[guess_count]++;
        }
        else
        {
            printf("whoa something went south here\n");
        }
        
        printf("found word in %d tries\n", guess_count);
        game_count++;
        
        // capture max/min and winning stats
        if(guess_count > max_guess)
        {
            max_guess = guess_count;
            strcpy(max_guess_word, target_word);
        }
        if(guess_count < min_guess)
        {
            min_guess = guess_count;
            strcpy(min_guess_word, target_word);
        }
        total_guess += guess_count;
        
        if(guess_count <= 6)
        {
            six_or_less++;
        }
    }

    switch(game_mode)
    {
        case GAME_MODE_RAND:
        case GAME_MODE_FULL_DICT:
        case GAME_MODE_WORDLE_CHECK:
            for(int i=1; i<20; i++)
            {
                printf("%d found in %d guesses\n", guesses_per_win[i], i);
            }
            printf("maximum guesses %d for word %s\n", max_guess, max_guess_word);
            printf("minimum guesses %d for word %s\n", min_guess, min_guess_word);
            printf("average guesses %f\n", ((float)total_guess)/(float)tot_games);
            printf("won the wordle %2.2f%% of the time, fear the future\n", 100.0*((float)six_or_less/(float)tot_games));
            break;
            
        case GAME_MODE_SINGLE:
            printf("found %s in %d guesses\n", target_word, min_guess);
            break;
    }
    
    return 0;
}
