#if !defined(COMPACT_TRIE_H)
#define COMPACT_TRIE_H

#include "trie.h"
#include <queue>

using namespace std;

class CompactTrie
{

public:
    CompactTrie(string fileName, bool compacted);
    bool doesWordExist(string word);
    void processTrie(Trie t);
    void processNode(Node *n, bool lastBranch);
    void writeToFile(string fileName);
    void writeInteger(unsigned int index, ofstream *outfile);
    int getIntegerVal(ifstream *infile, unsigned char curr);
    void readFromFile(string fileName);
    void createCode();
    void displayLists();

protected:
    int counter;
    vector<pair<pair<char, int>, bool>> branchList;
    vector<pair<int, bool>> nodeList;
    queue<Node *> nodes;
    map<int, char> numToChar;
    map<char, int> charToNum;
};

#endif // COMPACT_TRIE_H