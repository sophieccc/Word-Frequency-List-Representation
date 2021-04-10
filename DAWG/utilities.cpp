#include <iostream>
#include <fstream>
#include <ctype.h>
#include "trie.h"
#include "compact_trie.h"

using namespace std;

void storeInfo(CompactTrie compactTrie, string filename)
{
    int maxWordLength = 0;
    int alphabetSize = compactTrie.alphabet.size();
    int numWords = 0;
    int maxFreq = 0;
    ifstream file2;
    file2.open(filename, ios_base::in);
    string line;
    while (getline(file2, line))
    {
        numWords++;
        int split = line.find(" ");
        string word = line.substr(0, split);
        if(word.size() > maxWordLength)
        {
            maxWordLength = word.size();
        }
        int freq = stoi(line.substr(split));
        if(freq > maxFreq)
        {
            maxFreq = freq;
        }
    }
    ofstream output_file("./info.txt", std::ios_base::app);
    output_file << filename << " words=" << numWords << " longest_word=" << maxWordLength;
    output_file << " max_freq=" << maxFreq << " alphabet=" << alphabetSize << endl;
}

void storeLexiconSubset(int n, string filename)
{
    ifstream file2;
    file2.open(filename, ios_base::in);
    string line;
    int count = 1;
    ofstream output_file("./subset.txt");
    while (getline(file2, line))
    {
        if(count % n == 0)
        {
            output_file << line << endl;
        }
        count++;
    }
}

void cleanFile(string filename)
{
    ifstream file2;
    file2.open(filename, ios_base::in);
    string line;
    int count = 1;
    ofstream output_file("./cleaned.txt");
    while (getline(file2, line))
    {
        int split = line.find(" ");
        string word = line.substr(0, split);
        bool allowed = true;
        for(int i = 0; i < word.size() && allowed; ++i) {
            if(!(isalpha(word[i]) || word[i]=='\'' || word[i]=='-'))
            {
                allowed = false;
            }
        }
        if(allowed)
        {
            output_file << line << endl;
        }
    }
}

int main(int argc, char *argv[])
{
    string filename = argv[1];
    CompactTrie compactTrie = CompactTrie(filename, false);
    int n = 1988;
    //storeInfo(compactTrie, filename);
    //storeLexiconSubset(n, filename);
    //cleanFile(filename);
}