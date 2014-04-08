/**
* File: boggle.cpp
* ----------------
* Implements the game of Boggle.
*/
 
#include <cctype>
#include <iostream>
#include <cmath>
#include "simpio.h"
#include "gwindow.h"
#include "gboggle.h"
#include "grid.h"
#include "lexicon.h"
#include "random.h"
#include "strlib.h"
using namespace std;
 
/* Constants */
const int kBoggleWindowWidth = 650;
const int kBoggleWindowHeight = 350;
const int kStandardNumOfCubes = 16;
const int kBigBoggleNumOfCubes = 25;
const int kPauseTime = 50;
const string kEnglishLexicon = "EnglishWords.dat";
 
const string kStandardCubes[16] = {
  "AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS",
  "AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
  "DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
  "EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"
};
 
const string kBigBoggleCubes[25] = {
  "AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM",
  "AEEGMU", "AEGMNN", "AFIRSY", "BJKQXZ", "CCNSTW",
  "CEIILT", "CEILPT", "CEIPST", "DDLNOR", "DDHNOT",
  "DHHLOR", "DHLNOR", "EIIITT", "EMOTTT", "ENSSSU",
  "FIPRSY", "GORRVW", "HIPRRY", "NOOTUW", "OOOTTU"
};
 
/* Data Structures */
enum Direction {
  Northwest, North, Northeast, East, Southeast, South, Southwest, West
};
 
struct Cube {
  bool flag; //if true, has been used
  char letter;
  int row;
  int col;
};
 
struct Game {
  Grid<Cube> board;
  Lexicon english;
  Lexicon alreadyPlayed;
};
 
/* Function Prototypes */
static void welcome();
static bool responseIsAffirmative(const string& prompt);
static void giveInstructions();
void chooseBoard(Game &g);
void customBoardConfig(Game &g, int numOfCubes);
void autoBoardConfig(Game &g, int numOfCubes);
Grid<Cube> shuffleCubes(int numOfCubes);
void humanTurn(Game &g);
bool isLegal(Game &g, string input);
bool canBeFormed(Game &g, Cube c, string input);
Cube adjacentCube(Game &g, Direction &dir, int row, int col);
void highlightCubes(Game &g);
void clearAllHighlights(Game &g);
void computerTurn(Game &g);
void compCanBeFormed(Game &g, Cube c, Player player, string word);
bool compLegalWord(Game &g, string word);
bool inSameLocation(Cube c1, Cube c2);
 
/* Main Program */
int main() {
  GWindow gw(kBoggleWindowWidth, kBoggleWindowHeight);
  initGBoggle(gw);
  Game g;
  g.english = Lexicon(kEnglishLexicon);
  welcome();
 
  do {
    chooseBoard(g);
    humanTurn(g);
    computerTurn(g);
  } while(responseIsAffirmative("Would you like to play another round? "));
 
  cout << "Thanks for playing! Hit return to exit.";
  return 0;
}
 
/* Prints the welcoming text */
static void welcome() {
  cout << "Welcome!  You're about to play an intense game ";
  cout << "of mind-numbing Boggle.  The good news is that ";
  cout << "you might improve your vocabulary a bit.  The ";
  cout << "bad news is that you're probably going to lose ";
  cout << "miserably to this little dictionary-toting hunk ";
  cout << "of silicon.  If only YOU had a gig of RAM..." << endl << endl;
 
  if(responseIsAffirmative("Do you need instructions? ")) giveInstructions();
}
 
/* Gets response from user and checks if yes or no */
static bool responseIsAffirmative(const string &prompt) {
  while(true) {
    string answer = getLine(prompt);
    if(!answer.empty()) {
      switch(toupper(answer[0])) {
        case 'Y': return true;
        case 'N': return false;
      }
    }
    cout << "Please answer yes or no." << endl;
  }
}
 
/* Prints the instructions to the game */
static void giveInstructions() {
  cout << endl;
  cout << "The boggle board is a grid onto which I ";
  cout << "I will randomly distribute cubes. These ";
  cout << "6-sided cubes have letters rather than ";
  cout << "numbers on the faces, creating a grid of ";
  cout << "letters on which you try to form words. ";
  cout << "You go first, entering all the words you can ";
  cout << "find that are formed by tracing adjoining ";
  cout << "letters. Two letters adjoin if they are next ";
  cout << "to each other horizontally, vertically, or ";
  cout << "diagonally. A letter can only be used once ";
  cout << "in each word. Words must be at least four ";
  cout << "letters long and can be counted only once. ";
  cout << "You score points based on word length: a ";
  cout << "4-letter word is worth 1 point, 5-letters ";
  cout << "earn 2 points, and so on. After your puny ";
  cout << "brain is exhausted, I, the supercomputer, ";
  cout << "will find all the remaining words and double ";
  cout << "or triple your paltry score." << endl << endl;
  cout << "Hit return when you're ready...";
  getLine();
}
 
/* Asks the user what type of Boggle board they'd like to play (standard or Big Boggle),
* and if they would like to force the board configuration */
void chooseBoard(Game &g) {
  cout << "You can choose standard Boggle (4x4 grid) ";
  cout << "or Big Boggle (5x5)." << endl;
  string answer;
  int numOfCubes;
 
  if(responseIsAffirmative("Would you like Big Boggle? ")) numOfCubes = kBigBoggleNumOfCubes;
  else numOfCubes = kStandardNumOfCubes;
  drawBoard(sqrt((double)numOfCubes), sqrt((double)numOfCubes));
 
  cout << endl;
  cout << "I'll give you a chance to set up the board to ";
  cout << "your specification, which makes it easier to confirm ";
  cout << "your boggle program is working." << endl;
 
  if(responseIsAffirmative("Do you want to force the board configuration? ")) customBoardConfig(g, numOfCubes);
  else autoBoardConfig(g, numOfCubes);
}
 
/* Rearranges the board configuration based on user input */
void customBoardConfig(Game &g, int numOfCubes) {
  cout << endl;
  cout << "Enter a " << numOfCubes << "-character string ";
  cout << "identify which letters you want on the cubes." << endl;
  cout << "The first 4 letters are the cubes on the top row ";
  cout << "from left to right, the next 4 letters are the second ";
  cout << "row, and so on." << endl;
  cout << "Enter the string: ";
  string userInputLetters;
 
  while(true) {
    userInputLetters = getLine();
    if(userInputLetters.length() >= numOfCubes) break;
    cout << "String must include " << numOfCubes << " characters!";
    cout << " Try again: ";
  }
 
  int numRowsCols = sqrt((double)numOfCubes);
  Grid<Cube> cubes(numRowsCols,numRowsCols);
  int numSide = 0;
 
  for(int row = 0; row < numRowsCols; row++) {
    for(int col = 0; col < numRowsCols; col++) {
      char letter = userInputLetters[numSide];
      labelCube(row, col, letter);
      Cube cube = {false,letter,row,col};
      cubes[row][col] = cube;
      numSide++;
    }
  }
  g.board = cubes;
}
 
/* Rearranges the board configuration automatically */
void autoBoardConfig(Game &g, int numOfCubes) {
  Grid<Cube> cubes = shuffleCubes(numOfCubes);
  g.board = cubes;
  for(int row = 0; row < cubes.numRows(); row++) {
    for(int col = 0; col < cubes.numCols(); col++) {
      char letter = cubes[row][col].letter;
      labelCube(row, col, letter);
    }
  }
}
 
/* Randomly shuffles the cubes */
Grid<Cube> shuffleCubes(int numOfCubes) {
  int numRowsCols = sqrt((double)numOfCubes);
  Vector<string> vec;
 
  for(int n = 0; n < numOfCubes; n++) {
    if(numOfCubes == kBigBoggleNumOfCubes) vec.add(kBigBoggleCubes[n]);
    if(numOfCubes == kStandardNumOfCubes) vec.add(kStandardCubes[n]);
  }
  for(int i = 0; i < vec.size(); i++) {
    int r = randomInteger(i, numOfCubes-1); //choose random index r between i and last element position
    swap(vec[i], vec[r]);
  }
 
  int numSide = 0;
  Grid<string> cubeLetters(numRowsCols, numRowsCols);
  Grid<Cube> finalCubes(cubeLetters.numRows(), cubeLetters.numCols());
  for(int row = 0; row < cubeLetters.numRows(); row++) {
    for(int col = 0; col < cubeLetters.numCols(); col++) {
      cubeLetters[row][col] = vec[numSide]; //store new string positions of vector into grid format
      string side = cubeLetters[row][col];
      char letter = side[randomInteger(0,5)];
      Cube cube = {false, letter, row, col};
      finalCubes[row][col] = cube;
      numSide++;
    }
  }
  return finalCubes;
}
 
/* Loop that allows the user to enter words. Words already entered or that don't meet
* the minimum word length or that aren't in the English lexicon are rejected. */
void humanTurn(Game &g) {
  cout << endl;
  cout << "Ok, take all the time you want and find all ";
  cout << "the words you can! Signal that you're finished ";
  cout << "by entering an empty line." << endl << endl;
 
  Player player = HUMAN;
  string input;
 
  while(true) {
    cout << "Enter a word: ";
    input = getLine();
    if(input == "") break;
    input = toUpperCase(input);
    foreach(Cube c in g.board) { //reset highlight options to all false
      c.flag = false;
      highlightCube(c.row, c.col, false);
    }
    if(input.length() < 4) cout << "That word doesn't meet the minimum word length of 4 characters." << endl;
    else if(!(g.english).contains(input)) cout << "That's not a word!" << endl;
    else if((g.alreadyPlayed).contains(input)) cout << "You've already guessed that!" << endl;
    else if(!isLegal(g, input)) cout << "You can't make that word!" << endl;
    else {
      g.alreadyPlayed.add(input);
      recordWordForPlayer(input, player);
    }
  }
}
 
/* Checks if guessed word is legal and can be formed.
* Also a wrapper function for canBeFormed. */
bool isLegal(Game &g, string input) {
  string prefix = input.substr(0,2);
  if((g.english).containsPrefix(prefix)) {
    foreach(Cube c in g.board) {
      if(canBeFormed(g, c, input)) return true;
    }
  }
  return false;
}
 
/* Employs recursive backtracking to verify that the word can be formed on the board */
bool canBeFormed(Game &g, Cube c, string input) {
  if(input[0] != c.letter) return false;
  (g.board[c.row][c.col]).flag = true;
  string str = input.substr(1);
 
  if(str.length() == 0) { //base case; if no letters remaining, word is legal
    highlightCubes(g);
    pause(kPauseTime);
    clearAllHighlights(g);
    (g.board[c.row][c.col]).flag = false;
    return true;
  }
 
  for(Direction dir = Northwest; dir <= West; dir = Direction(dir+1)) {
    Cube nextCube = adjacentCube(g, dir, c.row, c.col);
    if(!nextCube.flag && canBeFormed(g, nextCube, str)) {
      (g.board[c.row][c.col]).flag = false;
      return true;
    }
  }
 
  (g.board[c.row][c.col]).flag = false;
  return false;
}
 
/* Uses switch statements to try out every cube adjacent to the cube
* currently being looked at. */
Cube adjacentCube(Game &g, Direction &dir, int row, int col) {
  Cube c2 = g.board[row][col];
  int maxRow = (g.board).numRows()-1;
  int maxCol = (g.board).numCols()-1;
 
  switch(dir) {
    case Northwest:
    if(row != 0 && col != 0) c2 = g.board[row-1][col-1]; break;
    case North:
    if(row != 0) c2 = g.board[row-1][col]; break;
    case Northeast:
    if(row != 0 && col != maxCol) c2 = g.board[row-1][col+1]; break;
    case East:
    if(col != maxCol) c2 = g.board[row][col+1]; break;
    case Southeast:
    if(row != maxRow && col != maxCol) c2 = g.board[row+1][col+1]; break;
    case South:
    if(row != maxRow) c2 = g.board[row+1][col]; break;
    case Southwest:
    if(row != maxRow && col != 0) c2 = g.board[row+1][col-1]; break;
    case West:
    if(col != 0) c2 = g.board[row][col-1]; break;
  }
  return c2;
}
 
/* Highlights all of the cubes flagged as "true" (used) */
void highlightCubes(Game &g) {
  foreach(Cube c in g.board) {
    if(c.flag) highlightCube(c.row, c.col, true);
  }
}
 
/* Clears all of the highlights from all cubes */
void clearAllHighlights(Game &g) {
  foreach(Cube c in g.board) {
    highlightCube(c.row, c.col, false);
  }
}
 
/* Implements the computer player, which finds all the remaining words */
void computerTurn(Game &g) {
  Player player = COMPUTER;
  string word;
 
  foreach(Cube c in g.board) {
    string word(1, c.letter);
    (g.board[c.row][c.col]).flag = true;
    highlightCube(c.row, c.col, true);
    compCanBeFormed(g, c, player, word);
    (g.board[c.row][c.col]).flag = false;
    highlightCube(c.row, c.col, false);
  }
}
 
/* Employs recursion to simulate an exhaustive search for all remaining legal words */
void compCanBeFormed(Game &g, Cube c, Player player, string word) {
  if(compLegalWord(g, word)) {
    (g.alreadyPlayed).add(word);
    recordWordForPlayer(word, player);
    highlightCube(c.row, c.col, true);
    pause(kPauseTime);
    highlightCube(c.row, c.col, false);
  }
 
  for(Direction dir = Northwest; dir <= West; dir = Direction(dir+1)) {
    Cube nextCube = adjacentCube(g, dir, c.row, c.col);
    string str = word + nextCube.letter;

    if(!nextCube.flag && (g.english).containsPrefix(str) && !(g.alreadyPlayed).contains(str) && !inSameLocation(nextCube, c)) {
      (g.board[nextCube.row][nextCube.col]).flag = true;
      highlightCube(c.row, c.col, true);
      word = str;
      compCanBeFormed(g, nextCube, player, word);
      word = word.substr(0,word.length()-1); //remove last char and try other char's at other positions

      (g.board[c.row][c.col]).flag = false;
      highlightCube(c.row, c.col, false);
    }
  }
}
 
/* Checks if each word is valid and returns true if so */
bool compLegalWord(Game &g, string word) {
  return (word.length()>=4 && (g.english).contains(word) && !(g.alreadyPlayed).contains(word));
}
 
/* Checks if two cubes are in the same location */
bool inSameLocation(Cube c1, Cube c2) {
  return ((c1.row==c2.row) && (c1.col==c2.col));
}