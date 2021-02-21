#include <iostream>
#include <fstream>
#include <string>

#include "trie.h"
#include "node.h"
#include <tuple>

void Trie::addLexicon(std::ifstream &file)
{
    std::string line;
    int lineCount = 0;
    int prevFreq = 0;
    int currFreq = 0;
    while (getline(file, line) && lineCount < 100000)
    {
        prevFreq = currFreq;
        int split = line.find(" ");
        std::string word = line.substr(0, split);
        currFreq = stoi(line.substr(split));
        processWord(word, prevFreq, currFreq);
        lineCount++;
    }
    replace_or_register(rootNode, 0, currFreq, 0);
    rootNode->printNode(0);
}

void Trie::processWord(std::string word, int prevFreq, int currFreq)
{
    int lastIndex = getCommonPrefix(word, lastWord);
    std::string prefix = word.substr(0, lastIndex);
    std::string suffix = word.substr(lastIndex);
    Node *tempNode = rootNode;
    for (int i = 0; i <= prefix.length(); i++)
    {
        tempNode->frequency += currFreq;
        tempNode = tempNode->hasLetter(prefix[i]);
    }
    Node *lastPrefixState = rootNode->contains(prefix);
    if (lastPrefixState != NULL && lastPrefixState->branches.size() != 0)
    {
        replace_or_register(lastPrefixState, lastIndex, prevFreq, currFreq);
    }
    addSuffix(suffix, currFreq, lastPrefixState);
    lastWord = word;
}

void Trie::replace_or_register(Node *curr, int index, int prevFreq, int currFreq)
{
    Node *child = curr->hasLetter(lastWord[index]);
    if (child->branches.size() != 0)
    {
        replace_or_register(child, index + 1, prevFreq, currFreq);
    }
    bool found = false;
    for (int i = 0; i < registered.size() && !found; i++)
    {
        found = checkEquivalence(registered[i], child);
        if (found)
        {
            std::cout << registered[i] << std::endl;
            std::cout << registered[i]->frequency << std::endl;
            registered[i]->frequency += prevFreq;
            curr->branches.find(lastWord[index])->second = registered[i];
            std::cout << curr->branches.find(lastWord[index])->second->frequency << std::endl;
            delete child;
            branchCount--;
        }
    }
    if (!found)
    {
        registered.push_back(child);
    }
}

bool Trie::checkEquivalence(Node *one, Node *two)
{
    if (one->terminal == two->terminal)
    {
        if (one->branches.size() == two->branches.size())
        {
            if (std::equal(one->branches.begin(),
                           one->branches.end(), two->branches.begin()))
            {
                return true;
            }
        }
    }
    return false;
}

void Trie::addSuffix(std::string word, int freq, Node *current = NULL)
{
    if (current == NULL)
    {
        current = rootNode;
    }
    std::pair<Node *, int> results;
    bool terminal = false;
    for (int i = 0; i < word.length(); i++)
    {
        if (i == word.length() - 1)
        {
            terminal = true;
        }
        results = current->addLetter(word[i], freq, terminal);
        current = std::get<0>(results);
        branchCount += std::get<1>(results);
    }
}

int Trie::getCommonPrefix(std::string current, std::string previous)
{
    int i;
    for (i = 0; i < previous.length(); i++)
    {
        if (previous[i] != current[i])
        {
            return i;
        }
    }
    return i;
}

bool Trie::doesWordExist(std::string word)
{
    Node *lastNode = rootNode->contains(word);
    if (lastNode && lastNode->branches.size() == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int Trie::getBranchCount()
{
    return branchCount;
}

int Trie::getNodeCount()
{
    return registered.size();
}

std::vector<std::string> Trie::getLexicon()
{
    std::vector<std::string> words = std::vector<std::string>();
    std::string word = "";
    rootNode->getWords(&words, word);
    return words;
}

Trie::Trie(const Trie &aTrie)
{
#ifdef MAP
    cout << "Calling copy constructor for <Trie>" << endl;
#endif
}

Trie::Trie()
{
#ifdef MAP
    cout << "Calling <Trie> constructor" << endl;
#endif
    rootNode = new Node(false, 0);
    branchCount = 0;
    lastWord = "";
    registered.push_back(rootNode);
}

Trie::~Trie()
{
#ifdef MAP
    cout << "Calling destructor for <Trie>" << endl;
#endif
}

int main(int argc, char *argv[])
{

    std::ifstream file;
    file.open(argv[1], std::ios_base::in);
    Trie trie = Trie();
    trie.addLexicon(file);

    std::cout << trie.getBranchCount() << std::endl;
    std::cout << trie.getNodeCount() << std::endl;
    // std::cout << trie.doesWordExist("hello") << std::endl;
    // std::cout << trie.doesWordExist("e") << std::endl;
    // std::cout << trie.doesWordExist("ho") << std::endl;
    // std::cout << trie.doesWordExist("h") << std::endl;
    // std::cout << trie.doesWordExist("hellooooo") << std::endl;
}