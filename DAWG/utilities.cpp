#include <iostream>
#include <fstream>
#include "trie.h"
#include "compact_trie.h"

using namespace std;

int main(int argc, char *argv[])
{
    CompactTrie compactTrie = CompactTrie(argv[1], false);
    int maxWordLength = 0;
    int alphabetSize = compactTrie.alphabet.size();
    int numWords = 0;
    int maxFreq = 0;

    ifstream file2;
    file2.open(argv[1], ios_base::in);
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
    output_file << argv[1] << " words=" << numWords << " longest_word=" << maxWordLength;
    output_file << " max_freq=" << maxFreq << " alphabet=" << alphabetSize << endl;
}