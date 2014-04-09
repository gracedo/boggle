Boggle
======

C++

Implements the game of Boggle with a human player playing against a computer player (using two different recursive algorithms to find possible words on the board).

The Boggle game board is a square grid onto which you randomly distribute a set of letter cubes. The goal is to find words on the board by tracing a path through adjoining letters. Two letters adjoin if they are next to each other horizontally, vertically, or diagonally. There are up to eight letters adjoining a cube. Each cube can be used at most once in a word. In the original version, all players work concurrently listing the words they find. When time is called, duplicates are removed from the lists and the players receive points for their remaining words.

###Human Player's turn

Checks for each word:
- It is at least four letters long.
- It is contained in the English lexicon.
- It has not already been included in the player’s word list (even if there is an alternate path on the board to form the same word, the word is counted at most once).
- It can be formed on the board (i.e., it is composed of adjoining letters and each cube is used at most once).

If any of these conditions fail, the word is rejected. If all is good, the word is added to the player’s word list and score. In addition, the word’s path is graphically shown by temporarily highlighting its cubes on the board. The length of the word determines the score: one point for a 4letter word, two points for 5-letters, and so on. The player enters a blank line when done finding words, which signals the end of the human’s turn.

###Computer’s turn

The computer then searches the entire board to find the remaining words missed by the human player. 
The computer earns points for each word found that meets the requirements (minimum length, contained in English lexicon, not already found, and can be formed on board).

Basically, the computer is unbeatable (unless you're just THAT good!).
