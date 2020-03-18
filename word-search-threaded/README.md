# WordSearch
College Project - Data Structures class - Uses parallel processing to find word matches in a word search puzzle.

This project actually only consists of 4 files, the rest are part of Cheryl-Engine-v1

So check out the following files:
WordSearch.h
WordSearch.cpp
WordBox.h
WordBox.cpp

The cool algorithm is in the WordSearch.cpp. I wanted to make it a threaded algorithm so what I did was make a lambda function to run comparisons between two lists of words. The lists are Search_Words and Search_Areas. The search areas would consist of diagonals, horizontals, and verticals taken straight from the WordBox (ie. the puzzle). The search words would consist of words to search for inside the search areas.

So this lambda would compare entries of one list against all the entries of another list. There were a lot of lists, so having the lambda running on multiple threads seemed like the best option. Of course it did work out, but maybe it is due to how trivial a word search really is. In any case the algorithm compiled lists of strings and would sort them by size. The lambda would be run for every Search_Word list against every Search_Area list of equal or greater length to its word lengths. So there would be 15~60 searches for every search word. They would not even necessarily be searches within the same thread, though obviously some would be.

Neat huh?
