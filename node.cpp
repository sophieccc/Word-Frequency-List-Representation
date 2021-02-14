#include <iostream>
#include <string>
#include <tuple>
#include "node.h"

std::tuple<Node*, int, int> Node::addLetter(char letter, int freq)
{
    for (int i = 0; i < branches.size(); i++)
    {
        if (branches[i].letter == letter)
        {
            branches[i].freq += freq;
            return std::make_tuple(branches[i].node,0,0);
        }
    }
    Node *newNode = new Node();
    Branch *newBranch = new Branch(letter, freq, newNode);
    branches.push_back(*newBranch);
    return std::make_tuple(newNode,1,1);
}

void Node::printNode(int offset)
{
    std::string tabs = "";
    for (int i = 0; i < offset; i++)
    {
        tabs += "   ";
    }
    std::cout << '*' << tabs << std::endl;
    for (int i = 0; i < branches.size(); i++)
    {
        std::cout << tabs << "--" << branches[i].letter << "-->";
        branches[i].node->printNode(offset + 2);
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
    for (int i = 0; i < branches.size() && !nextNode; i++)
    {
        if (branches[i].letter == letter)
        {
            nextNode = branches[i].node;
        }
    }
    return nextNode;
}

std::vector<Node::Branch> Node::getBranches()
{
    return branches;
}

void Node::getWords(std::vector<std::string> *words, std::string word)
{
    for (int i = 0; i < branches.size(); i++)
    {
        std::string tempWord = word + branches[i].letter;
        if (branches[i].node->branches.size() == 0)
        {
            words->push_back(tempWord);
        }
        else
        {
            branches[i].node->getWords(words, tempWord);
        }
    }
}

Node::Node(const Node &aNode)
{
#ifdef MAP
    cout << "Calling copy constructor for <Node>" << endl;
#endif
}

Node::Node()
{
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