#if !defined(TRIE_H)
#define TRIE_H

#include <string>
#include <vector>
#include "node.h"

class Trie
{

public:
    void addLexicon(std::ifstream &file);
    void addSuffix(std::string word, int freq, Node *current);
    void processWord(std::string word, int prevFreq, int currFreq);
    int getCommonPrefix(std::string current, std::string previous);
    Node* traversePrefix(std::string prefix, int freq);
    void replace_or_register(Node *curr, int index, int prevFreq, int currFreq);
    bool checkEquivalence(Node *one, Node *two);
    bool doesWordExist(std::string word, bool onlyRegistered=false);
    int getNodeCount();
    int getBranchCount();
    std::vector<std::string> getLexicon();
    Trie(const Trie &aTrie);
    Trie();
    
    Node *rootNode;
    std::vector<Node *> registered;

protected:
    std::string lastWord;
};

#endif // TRIE_H