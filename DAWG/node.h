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
    bool operator==(const Node& t) const;
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

    struct MyHashFunction
    {
        size_t operator()(const Node& n) const
        {
            string hashStr;
            if(n.terminal==true) {
                hashStr += "1";
            }
            else {
                hashStr +='0';
            }
            for (auto it = n.branches.begin(); it != n.branches.end(); ++it)
            {
                hashStr+=it->first;
                hashStr+=to_string(it->second->id);
            }

            return hash<string>()(hashStr); 
        }
    };
    
};

#endif // NODE_H