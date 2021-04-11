#if !defined(COMPACT_TRIE_H)
#define COMPACT_TRIE_H

#include "trie.h"
#include <queue>
#include <set>

using namespace std;

class CompactTrie
{

public:
    CompactTrie(string fileName, bool compacted);
    void processTrie(Trie t);
    void processNode(Node *n, bool lastBranch);
    void createCode();
    bool doesWordExist(string word);
    void displayLists();
    void writeLexicon();
    void getWords(vector<string> *words, string word, int index);
    int getWordFrequency(string word);
    int getTotal(int index);
    int findLetter(int index, char letter);
    double getWordProbability(string word);
    void writeWordProbas(string inputFile, string outputFile);

    void writeToFile(string fileName, bool useLogs);
    void writeAlphabet(ofstream *outfile);
    pair<int, int> writeTrieInfo(ofstream *outfile, bool useLogs);
    int getMaxFrequency();
    void setMinLogBase(int maxFreq);
    int getIntegerMode(int listSize);
    void writeInteger(unsigned int index, ofstream *outfile, int queueMode, bool logVals);
    void anyByteWrite(unsigned int index, ofstream *outfile);
    void oneOrTwoBytesWrite(unsigned int index, ofstream *outfile);
    void twoOrThreeBytesWrite(unsigned int index, ofstream *outfile);
    void readFromFile(string fileName);
    void readArrays(int listSize, int queueMode, int freqMode, ifstream *infile, bool useLogs);
    pair<pair<char, int>, bool> constructBranch
    (unsigned char curr, int freqMode, ifstream *infile, bool useLogs, vector<bool> *terminality);
    int getIntegerVal(ifstream *infile, unsigned char firstChar, int mode, bool logVals);
    int anyByteRead(ifstream *infile, unsigned char firstChar);
    int oneOrTwoBytesRead(ifstream *infile, unsigned char firstChar);
    int twoOrThreeBytesRead(ifstream *infile, unsigned char firstChar);

    set<char> alphabet;

protected:
    int counter;
    vector<pair<pair<char, int>, bool>> branchList;
    vector<pair<int, bool>> nodeList;
    queue<Node *> nodes;
    map<int, char> numToChar;
    map<char, int> charToNum;
    int multiplier = 1000;
    int logBase = 2;
};

#endif // COMPACT_TRIE_H