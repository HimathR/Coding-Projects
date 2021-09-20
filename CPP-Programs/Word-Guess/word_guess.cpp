// Include files go here
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <map>
#include <functional>
using namespace std;

class WordGuess // Class WordGuess goes here
{
  // Private member functions and data
  int length = 0, num_guesses = 0;
  vector<string> possible_words; // store words in a vector
  map<string, int> categories;   // division into categories (word, category size)
  string used = "", answer = string(length, '-');

public:
  // Default Constructor
  WordGuess() {}

  // Constructor (string, int, int) => reads file and creates data structures
  WordGuess(string filename, int l, int n); // initialize length + num_guesses

  // bool again() => returns true if guesses remaining and problem not solved
  bool again() { return (num_guesses > 0 && !successful()); }

  // used to check if same letter hasn't occured twice
  bool compare(char letter, string word) { return (word.find(letter) != string::npos); }

  // int guesses_left() => returns the current number of guesses remaining
  int guesses_left() { return num_guesses; }

  // bool already_used(char) => returns true if letter already used
  bool already_used(char letter);

  // used to remove values not corresponding to largest category chosen
  void cut_down(vector<string> &possible_words, function<bool(string)> check);

  // void add_letter(char) => adds letter to solution and updates data structures
  void add_letter(char letter);

  // used to divide the string by index into categories
  string categorise(string word, string answer, char letter);

  // string current_word() => returns the current user solution
  string current_word() { return answer; }

  // int words_left() => returns the current number of possible solutions
  int words_left() { return possible_words.size(); }

  // bool successful() => if no values of the answer have _, then the word was found and returns true
  bool successful() { return (answer.find('-') == string::npos); }

  // returns the answer word
  string get_answer() { return possible_words[0]; }
};

WordGuess::WordGuess(string filename, int l, int n) : length(l), num_guesses(n)
{
  ifstream in(filename); // should open the file if its there
  if (!in.is_open())
  {
    cout << "File Not Detected!" << filename << endl;
    exit(0);
  }
  string word;
  while (in >> word) // read file into string
  {
    if (word.length() == length)
    {
      possible_words.push_back(word);
    }
  }
  in.close();
}

string WordGuess::categorise(string word, string answer, char letter)
{
  string category = answer;
  for (int i = 0; i < word.length(); i++)
  {
    if (word[i] == letter)
      category[i] = letter;
  }
  return category;
}

bool WordGuess::already_used(char letter)
{
  if (compare(letter, used))
  {
    used += letter;
    return true;
  }
  else
  {
    used += letter;
    return false;
  }
}

void WordGuess::cut_down(vector<string> &possible_words, function<bool(string)> check)
{
  possible_words.erase(remove_if(possible_words.begin(), possible_words.end(), check), possible_words.end());
}

void WordGuess::add_letter(char letter)
{
  for (string word : possible_words)
  {
    string category = categorise(word, answer, letter);
    categories[category]++; // in category size
  }

  // lambda function to find largest value in categories
  auto max_el = max_element(
      begin(categories),end(categories),
      [](const auto &p1, const auto &p2) {
        return p1.second < p2.second;
      });
  answer = max_el->first; 
  
  // Remove all words from possible_size that aren't included in the chosen family with lambda function
  cut_down(possible_words, [=](string word) 
           {
             for (int i = 0; i < word.length(); i++)
             {
               // see if the indexes of answer match indexes in word e.g. word _l_a_ = current answer of _l_a_
               if (answer[i] == '-' && word[i] == letter)
                 return true;
               if (answer[i] != '-' && answer[i] != word[i])
                 return true;
             }
             return false;
           });

  num_guesses--;      // finally, decrement # of guesses
  categories.clear(); // reset the categories
}

int main(int argc, char *argv[])
{
  char letter;

  if (argc < 3)
  {
    cout << "Must enter word length and number of guesses on command line" << endl;
    exit(0);
  }

  WordGuess wg("dictionary.txt", atoi(argv[1]), atoi(argv[2])); // enter word length, maximum allowed guesses
  while (wg.again())
  {
    cout << "Enter a letter: ";
    cin >> letter;
    if (!islower(letter))
      exit(0);
    if (wg.already_used(letter))
    {
      cout << "You have already tried " << letter << endl;
      continue;
    }
    wg.add_letter(letter);
    cout << wg.current_word() << ", Guesses left = " << wg.guesses_left()
         << ", Words left = " << wg.words_left() << endl;
  }
  if (wg.successful())
    cout << "Good job!" << endl;
  else
    cout << "Sorry, the word was " << wg.get_answer() << endl;
  return 0;
}