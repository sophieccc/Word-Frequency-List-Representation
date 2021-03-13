#if !defined(COMPACT_TRIE_H)
#define COMPACT_TRIE_H

#include "trie.h"
#include <queue>

using namespace std;

class CompactTrie
{

public:
    CompactTrie(ifstream &file, bool compacted);
    bool doesWordExist(string word);
    void processTrie(Trie t);
    void processNode(Node* n, bool lastBranch);

protected:
    int counter;
    vector<pair<pair<char,int>,bool>> branchList;
    vector<pair<int,bool>> nodeList;
    queue<Node*> nodes;
};

#endif // COMPACT_TRIE_H