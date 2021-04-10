#include <iostream>
#include <fstream>
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
int main(int argc, char *argv[])
{
    string filename = argv[1];
    CompactTrie compactTrie = CompactTrie(filename, false);
    storeInfo(compactTrie, filename);
}