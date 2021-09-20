// Include files go here
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <numeric>
#include <ctime>
using namespace std;

class WordSum // Class WordSum goes here
{
    // Private member functions and data used to solve problem
    int attempts = 0, n = 0;
    clock_t time_taken = 0;

    unordered_map<char, int> characters; // char = unique characters, int = assigned digit for given character
    vector<string> LHS, LR;
    string RHS;

    vector<bool> use = vector<bool>(10, false);
    vector<char> letters, leadingletters; // leading letter

public:
    WordSum() {}                         // Default Constructor
    WordSum(vector<string> l, string r); // Constructor (vector<string>, string) => creates data structures and solves problems

    bool permutation(int n);
    bool eval();

    friend ostream &operator<<(ostream &os, WordSum &obj);
};

WordSum::WordSum(vector<string> l, string r) : LHS(l), RHS(r)
{
    LR = LHS;
    LR.push_back(RHS);
    unordered_set<char> unique_letters;
    for (auto s : LR)
    {
        char ch = s[0];
        leadingletters.push_back(ch); // push back leading letters of each string
        for (auto c : s)
            unique_letters.insert(c);
    }
    for (auto c : unique_letters)
    {
        characters.insert({c, 0});
        letters.push_back(c);
    }
    clock_t time_taken = clock();
    permutation(n);
}

bool WordSum::permutation(int n)
{
    if (n == letters.size()) // base case
    {
        for (int i = leadingletters.size() - 1; i >= 0; i--) // check for leading digits
        {
            auto it = characters.find(leadingletters[i]);
            if (it->second == 0) // if any leading digit has a 0, try again
                return false;
        }
        if (eval()) // evaluate
            return true;
        return false;
    }
    for (int i = 0; i < 10; i++) // values from 0-9 can be allocated
    {
        if (use[i] == false) // if ith integer is not used yet
        {
            characters[letters[n]] = i; // assign char at index n to integer i
            use[i] = true;                 // set use[i] to true state
            if (permutation(n + 1))     // call recursive function with incremented n
                return true;
            use[i] = false; // backtrack for the other possible solutions
        }
    }
    return false;
}

bool WordSum::eval()
{
    attempts++;
    int RHSVal = 0, LHSVal = 0;

    int m = 1, i = 0, j = 0;
    for (i = RHS.length() - 1; i >= 0; i--) // index backwards to make place value calcs easier
    {
        auto it = characters.find(RHS[i]);
        RHSVal += m * it->second; // current m value * number assigned to character in map
        m *= 10;                  // make m go up by one place value by multiplying by 10
    }

    m = 1, i = 0, j = 0; // reinitialise variables
    vector<int> LeftEvals;
    for (auto s : LHS) // for every string in LHS vector
    {
        for (i = s.length() - 1; i >= 0; i--) // repeat process for each string on LHS
        {
            auto it = characters.find(s[i]);
            LHSVal += m * it->second;
            m *= 10;
        }
        LeftEvals.push_back(LHSVal); // save LHS values in a vector
        m = 1, LHSVal = 0;           // reinitialise variables
    }

    // add all values in LeftVal vector using accumulate function.
    LHSVal = accumulate(LeftEvals.begin(), LeftEvals.end(), 0);
    if (LHSVal == RHSVal && attempts > 0) // if left = right, a solution was found
        return true;

    return false;
}

ostream &operator<<(ostream &os, WordSum &obj) // Overloaded operator<<
{
    cout << "Problem: ";
    for (auto s : obj.LHS)
        cout << s << " ";
    cout << "= " << obj.RHS << ", CPU = " << ((float)clock() - obj.time_taken) / CLOCKS_PER_SEC << endl;
    for (auto w : obj.characters)
        cout << w.first << " = " << w.second << ", ";
    cout << "Attempts = " << obj.attempts << endl;
    return os;
}

int main()
{
    WordSum ws1({"SEND", "MORE"}, "MONEY");
    cout << ws1 << endl;
    WordSum ws2({"EARTH", "AIR", "FIRE", "WATER"}, "NATURE");
    cout << ws2 << endl;
    WordSum ws3({"SATURN", "URANUS", "NEPTUNE", "PLUTO"}, "PLANETS");
    cout << ws3 << endl;
    return 0;
}