#if !defined(TRIE_H)
#define TRIE_H

#include <string>
#include <vector>
#include "node.h"

class Trie
{

public:
    void addLexicon(std::ifstream &file);
    void processWord(std::string word, int prevFreq, int currFreq);
    int getCommonPrefix(std::string current, std::string previous);
    Node* traversePrefix(std::string prefix, int freq);
    void addSuffix(std::string word, int freq, Node *current);
    void replace_or_register(Node *curr, int index, int prevFreq, int currFreq);
    bool checkEquivalence(Node *one, Node *two);
    bool doesWordExist(std::string word);
    void calculateCounts();
    void findChildren(Node* curr);
    std::vector<std::string> getLexicon();
    Trie(const Trie &aTrie);
    Trie();
    
    Node *rootNode;
    int nodeCount;
    int branchCount;

protected:
    std::map<int, Node *> finalNodes;
    std::string lastWord;
    std::vector<Node *> registered;
    int latestId = 0;
};

#endif // TRIE_H