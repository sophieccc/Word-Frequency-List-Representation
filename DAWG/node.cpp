#include <iostream>
#include "node.h"

using namespace std;

bool Node::operator==(const Node& t) const
{ 
    bool eqTerminal = this->terminal == t.terminal;
    bool eqBranches = equal(this->branches.begin(), this->branches.end(), t.branches.begin());
    return eqTerminal && eqBranches;
}
Node *Node::hasWord(string word)
{
    Node *current = this;
    for (int i = 0; i < word.length() && current; i++)
    {
        current = current->hasLetter(word[i]);
    }
    return current;
}

Node *Node::hasLetter(char letter)
{
    Node *nextNode = NULL;
    map<char, Node *>::iterator it = branches.find(letter);
    if (it != branches.end())
    {
        nextNode = it->second;
    }
    return nextNode;
}

Node * Node::addLetter(char letter, int freq, bool terminal, int id)
{
    map<char, Node *>::iterator it = branches.find(letter);
    if (it == branches.end())
    {
        Node *newNode = new Node(terminal, id);
        branchFreqs.insert(pair<char, int>(letter, freq));
        branches.insert(pair<char, Node *>(letter, newNode));
        return newNode;
    }
    else
    {
        branchFreqs.find(letter)->second +=freq;
        return it->second;
    }
}

void Node::getWords(vector<string> *words, string word)
{
    for (auto it = branches.begin(); it != branches.end(); ++it)
    {
        string tempWord = word + it->first;
        if (it->second->branches.size() == 0)
        {
            words->push_back(tempWord);
        }
        else
        {
            if (it->second->terminal == true)
            {
                words->push_back(tempWord);
            }
            it->second->getWords(words, tempWord);
        }
    }
}

void Node::printNode(int offset)
{
    string tabs = "";
    for (int i = 0; i < offset; i++)
    {
        tabs += "      ";
    }
    string symb = " ";
    if (terminal)
    {
        symb = "!";
    }
    cout << "[" << id << "]" << symb << tabs << endl;
    for (auto it = branches.begin(); it != branches.end(); ++it)
    {
        cout << tabs << "--" << it->first << "-" << branchFreqs.find(it->first)->second << "-->";
        it->second->printNode(offset + 2);
    }
}

Node::Node(bool terminality, int idInput)
{
    terminal = terminality;
    id = idInput;
    registered = false;
}