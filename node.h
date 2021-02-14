#if !defined (NODE_H)
#define NODE_H

#include <string>
#include <vector>
#include <tuple>

class Node
{

    struct Branch
    {
        char letter;
        int freq;
        Node *node;

        Branch(char letter1, int freq1, Node *node1)
        {
            letter = letter1;
            freq = freq1;
            node = node1;
        }
    };

public:
    void printNode(int offset);
    std::tuple<Node*, int, int> addLetter(char letter, int freq);
    Node *contains(std::string word);
    Node *hasLetter(char letter);
    std::vector<Branch> getBranches();
    void getWords(std::vector<std::string> *words, std::string word);
    Node(const Node &aNode);
    Node();
    virtual ~Node();

protected:
    std::vector<Branch> branches;
};

#endif // NODE_H