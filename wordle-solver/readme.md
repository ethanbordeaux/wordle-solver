Wordle Solver

An attempt to create an efficient Wordle solver for the viral word guessing game:

https://www.powerlanguage.co.uk/wordle/

Along with the Wordle solver methods a test harness is included.  The test harness can run against an extracted version of the Wrdle dictionary or a custom dictionary.  The dictionary should be one word per line and pure ASCII.

There are two dictionary types that can be used; a built-in one that is pulled from the Wordle source or one of your own choosing.  The dictionary doesn't need to be pruned to 5-letter words only but it should be all plain ASCII and each entry on a separate line.

You can choose your own start word for guessing or use the one built into the application based on statistical analysis ("tromp").

You can either play a single game against a specific word, a random set of games where words are pulled from the input dictionary, against the full dictionary, or a special "start word analyzer" that plays every start word against the full dictionary to find the optimal start word for that particular dictionary.
