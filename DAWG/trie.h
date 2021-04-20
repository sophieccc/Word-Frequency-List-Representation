#if !defined(TRIE_H)
#define TRIE_H

#include <string>
#include <vector>
#include <unordered_set>
#include "node.h"

using namespace std;

class Trie
{

public:
    vector<string> getLexicon();
    void addLexicon(ifstream &file);
    void processWord(string word, int prevFreq, int currFreq);
    int getCommonPrefix(string current, string previous);
    Node *traversePrefix(string prefix, int freq);
    void addSuffix(string word, int freq, Node *current);
    void minimise(Node *curr, int index, int prevFreq, int currFreq);
    void addNode(Node* child, int index, int prevFreq);
    bool checkEquivalence(Node *one, Node *two);
    bool doesWordExist(string word);
    void calculateCounts();
    void addFrequencies(Node *n, Node *n2);
    int getWordFrequency(string word);
    int getTotal(Node *n);
    Trie();

    Node *rootNode;
    int nodeCount;
    int branchCount;
    unordered_multiset<Node *, Node::MyHashFunction, Node::MyEqualFunction> minSet;

protected:
    map<int, Node *> finalNodes;
    string lastWord;
    vector<Node *> registered;
    int latestId = 0;
};

#endif // TRIE_H