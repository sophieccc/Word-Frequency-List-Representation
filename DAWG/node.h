#if !defined(NODE_H)
#define NODE_H

#include <string>
#include <vector>
#include <tuple>
#include <map>

class Node
{

public:
    std::pair<Node *, int> addLetter(char letter, int freq, bool terminal);
    Node *contains(std::string word);
    Node *hasLetter(char letter);
    void getWords(std::vector<std::string> *words, std::string word);
    void printNode(int offset);
    Node(const Node &aNode);
    Node(bool terminality, int freq);
    virtual ~Node();

    bool terminal;
    int frequency;
    std::map<char, Node *> branches;
};

#endif // NODE_H