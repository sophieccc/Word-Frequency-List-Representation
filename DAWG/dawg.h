#if !defined(DAWG_H)
#define DAWG_H

#include <string>
#include <vector>
#include <unordered_set>
#include "node.h"

using namespace std;

class Dawg
{

public:
    vector<string> getLexicon();
    void addLexicon(ifstream &file);
    void processWord(string word, int prevFreq, int currFreq);
    int getCommonPrefix(string current, string previous);
    Node *traversePrefix(string prefix, int freq);
    void addSuffix(string word, int freq, Node *current);
    void minimise(Node *curr, int index, int prevFreq, int currFreq);
    Node* findEquivNode(Node* child);
    void addNode(Node* child, int index, int prevFreq);
    bool checkEquivalence(Node *one, Node *two);
    bool doesWordExist(string word);
    void calculateCounts();
    void addFrequencies(Node *n, Node *n2);
    int getWordFrequency(string word);
    int getTotal(Node *n);
    Dawg();

    Node *rootNode;
    int nodeCount;
    int branchCount;
    unordered_multiset<Node *, Node::MyHashFunction, Node::MyEqualFunction> minSet;

protected:
    map<int, Node *> finalNodes;
    string prevWord;
    vector<Node *> registered;
    int latestId = 0;
};

#endif // DAWG_H