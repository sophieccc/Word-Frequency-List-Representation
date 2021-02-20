#if !defined (TRIE_H)
#define TRIE_H

#include <string>
#include <vector>
#include "node.h"

class Trie
{

public:
    void addLexicon(std::ifstream &file);
    void addSuffix(std::string word, int freq, Node* current);
    void processWord(std::string word, int freq);
    int getCommonPrefix(std::string current, std::string previous);
    void replace_or_register(Node* curr);
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
    std::vector<Node*> allNodes;
    std::string lastWord;
};

#endif // TRIE_H