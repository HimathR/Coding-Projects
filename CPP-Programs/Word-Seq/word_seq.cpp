#include <iostream>
#include <ctime>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
using namespace std;

class WordSeq
{
    unordered_map<string, int> twocounts, threecounts, fourcounts, fivecounts;
    vector<unordered_map<string, int>> counts; // vector of unordered maps - one map each for 2 to 5

public:
    // Default Constructor
    WordSeq() {}
    // Constructor (string, int, int) => reads file and creates data structures
    WordSeq(string filename);
    // Identify and generate subsequences
    void subsequence(string word);
};

WordSeq::WordSeq(string filename)
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
        subsequence(word);
    }

    counts.emplace_back(twocounts);
    counts.emplace_back(threecounts);
    counts.emplace_back(fourcounts);
    counts.emplace_back(fivecounts);

    // iterate through vector of unordered maps
    int currentlen = 2;
    for (auto currentmap : counts)
    {
        vector<pair<string, int>> sorted(10); // put the top ten occurences into a vector of pairs
        partial_sort_copy(currentmap.begin(), currentmap.end(), sorted.begin(), sorted.end(),
                          [](pair<string, int> const &left,
                             pair<string, int> const &right) {
                              return left.second > right.second;
                          });
        cout << "Found " << currentmap.size() << " subsequences of length " << currentlen << endl;
        for (auto it = sorted.rbegin(); it != sorted.rend(); it++)
            cout << it->first << " " << it->second << endl; // print top 10 occurences
        currentlen++;
        cout << endl;
    }
    in.close();
}

void WordSeq::subsequence(string word)
{
    for (int i = 0; i < word.size(); i++)
    {
        for (int len = 2; len <= 5; len++)
        {
            string subseq = word.substr(i, len);
            if (len == 2 && subseq.size() == 2)
                twocounts[subseq]++;
            else if (len == 3 && subseq.size() == 3)
                threecounts[subseq]++;
            else if (len == 4 && subseq.size() == 4)
                fourcounts[subseq]++;
            else if (len == 5 && subseq.size() == 5)
                fivecounts[subseq]++;
        }
    }
}

int main()
{
    clock_t time_taken = clock();
    WordSeq ws("dictionary.txt");
    cout << "CPU Time: " << ((float)clock() - time_taken) / CLOCKS_PER_SEC << endl;
    return 0;
}