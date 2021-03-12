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
    bool operator==(const Node &t) const;
    Node *hasWord(string word);
    Node *hasLetter(char letter);
    Node *addLetter(char letter, int freq, bool terminal, int id);
    void getWords(vector<string> *words, string word);
    void printNode(int offset);
    Node(bool terminality, int idInput);

    int id;
    bool terminal;
    mutable bool registered;
    map<char, Node *> branches;
    map<char, int> branchFreqs;

    struct MyHashFunction
    {
        size_t operator()(const Node *n) const
        {
            string hashStr;
            if (n->terminal == true)
            {
                hashStr += "1";
            }
            else
            {
                hashStr += '0';
            }
            for (auto it = n->branches.begin(); it != n->branches.end(); ++it)
            {
                hashStr += it->first;
                hashStr += to_string(it->second->id);
            }
            hashStr += n->branches.size();

            return hash<string>()(hashStr);
        }
    };

    struct MyEqualFunction
    {
        bool operator()(const Node *lhs, const Node *rhs) const
        {
            bool eqTerminal = lhs->terminal == rhs->terminal;
            bool eqBranches = equal(lhs->branches.begin(), lhs->branches.end(), rhs->branches.begin());
            return eqTerminal && eqBranches;
        }
    };
};

#endif // NODE_H