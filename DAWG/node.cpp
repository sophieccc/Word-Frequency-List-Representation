#include <iostream>
#include <string>
#include <tuple>
#include "node.h"

std::tuple<Node*, int, int> Node::addLetter(char letter, int freq, bool terminal)
{
    std::map<char,Node*>::iterator it = branches.find(letter);
    if ( it == branches.end() ) {
        Node *newNode = new Node(terminal, freq);
        branches.insert(std::pair<char,Node*>(letter,newNode));
        return std::make_tuple(newNode,1,1);
    } else {
            it->second->frequency += freq;
            return std::make_tuple(it->second,0,0);
    }
}

Node *Node::contains(std::string word)
{
    Node *current = this;
    Node *final = NULL;
    int i;
    for (i = 0; i < word.length() && current; i++)
    {
        current = current->hasLetter(word[i]);
    }
    return current;
}

Node *Node::hasLetter(char letter)
{
    Node *nextNode = NULL;
    std::map<char,Node*>::iterator it = branches.find(letter);
    if ( it != branches.end() ) {
        nextNode = it->second;
    } 
    return nextNode;
}


void Node::getWords(std::vector<std::string> *words, std::string word)
{
    std::map<char,Node*>::iterator it;
    for (it=branches.begin(); it!=branches.end(); ++it) {
       std::string tempWord = word + it->first;
        if (it->second->branches.size() == 0)
        {
            words->push_back(tempWord);
        }
        else
        {
            if(it->second->terminal == true) {
                words->push_back(tempWord);
            }
            it->second->getWords(words, tempWord);
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
    if(terminal) {
        symb = "!";     
    }
    std::cout << '*' << symb << tabs << std::endl;

    std::map<char,Node*>::iterator it;
    for (it=branches.begin(); it!=branches.end(); ++it) {
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