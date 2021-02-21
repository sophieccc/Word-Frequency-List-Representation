#include <iostream>
#include <string>
#include <tuple>
#include "node.h"

Node * Node::addLetter(char letter, int freq, bool terminal)
{
    std::map<char, Node *>::iterator it = branches.find(letter);
    if (it == branches.end())
    {
        Node *newNode = new Node(terminal, freq);
        branches.insert(std::pair<char, Node *>(letter, newNode));
        return newNode;
    }
    else
    {
        it->second->frequency += freq;
        return it->second;
    }
}

Node *Node::contains(std::string word, bool onlyRegistered)
{
    Node *current = this;
    for (int i = 0; i < word.length() && current; i++)
    {
        current = current->hasLetter(word[i]);
    }
    return current;
}

Node *Node::hasLetter(char letter, bool onlyRegistered)
{
    Node *nextNode = NULL;
    std::map<char, Node *>::iterator it = branches.find(letter);
    if (it != branches.end() && (!onlyRegistered || it->second->registered == true))
    {
        nextNode = it->second;
    }
    return nextNode;
}

void Node::getWords(std::vector<std::string> *words, std::string word)
{
    std::map<char, Node *>::iterator it;
    for (it = branches.begin(); it != branches.end(); ++it)
    {
        if(it->second->registered == true) {
            std::string tempWord = word + it->first;
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
}

void Node::printNode(int offset)
{
    std::string tabs = "";
    for (int i = 0; i < offset; i++)
    {
        tabs += "   ";
    }
    std::string symb = " ";
    if (terminal)
    {
        symb = "!";
    }
    std::cout << '*' << symb << tabs << std::endl;

    std::map<char, Node *>::iterator it;
    for (it = branches.begin(); it != branches.end(); ++it)
    {
        std::cout << tabs << "--" << it->first << "|" << it->second->frequency << "-->";
        it->second->printNode(offset + 2);
    }
}

Node::Node(const Node &aNode)
{
#ifdef MAP
    cout << "Calling copy constructor for <Node>" << endl;
#endif
}

Node::Node(bool terminality, int freq)
{
    terminal = terminality;
    frequency = freq;
#ifdef MAP
    cout << "Calling <Node> constructor" << endl;
#endif
}

Node::~Node()
{
#ifdef MAP
    cout << "Calling destructor for <Node>" << endl;
#endif
}