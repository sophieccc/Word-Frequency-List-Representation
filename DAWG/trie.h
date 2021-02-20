#if !defined (TRIE_H)
#define TRIE_H

#include <string>
#include "node.h"

class Trie
{

public:
    void addLexicon(std::ifstream &file);
    void addWord(std::string word, int freq);
    bool doesWordExist(std::string word);
    int getNodeCount();
    int getBranchCount();
    std::vector<std::string> getLexicon();
    Trie(const Trie &aTrie);
    Trie();
    virtual ~Trie();

    int nodeCount;
    int branchCount;

protected:
    Node *rootNode;
};

#endif // TRIE_H