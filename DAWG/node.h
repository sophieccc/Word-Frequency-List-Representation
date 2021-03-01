#if !defined(NODE_H)
#define NODE_H

#include <string>
#include <vector>
#include <tuple>
#include <map>

using namespace std;

class Node
{

public:
    Node *hasWord(string word);
    Node *hasLetter(char letter);
    Node *addLetter(char letter, int freq, bool terminal, int id);
    void getWords(vector<string> *words, string word);
    void printNode(int offset);
    Node(bool terminality, int freq, int idInput);

    int id;
    bool terminal;
    bool registered;
    int frequency;
    map<char, Node *> branches;
    
};

#endif // NODE_H