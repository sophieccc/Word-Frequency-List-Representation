#if !defined(NODE_H)
#define NODE_H

#include <string>
#include <vector>
#include <tuple>
#include <map>

class Node
{

public:
    Node *hasWord(std::string word);
    Node *hasLetter(char letter);
    Node *addLetter(char letter, int freq, bool terminal, int id);
    void printNode(int offset);
    void getWords(std::vector<std::string> *words, std::string word);
    Node(const Node &aNode);
    Node(bool terminality, int freq, int idInput);

    int id;
    bool terminal;
    bool registered;
    int frequency;
    std::map<char, Node *> branches;
    
};

#endif // NODE_H