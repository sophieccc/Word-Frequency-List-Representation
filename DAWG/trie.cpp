#include <iostream>
#include <fstream>
#include <iterator>
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
    while (getline(file, line))
    {
        prevFreq = currFreq;
        int split = line.find(" ");
        std::string word = line.substr(0, split);
        currFreq = stoi(line.substr(split));
        processWord(word, prevFreq, currFreq);
        lineCount++;
    }
    replace_or_register(rootNode, 0, currFreq, 0);
    //rootNode->printNode(0);
}

void Trie::processWord(std::string word, int prevFreq, int currFreq)
{
    int lastIndex = getCommonPrefix(word, lastWord);
    std::string prefix = word.substr(0, lastIndex);
    std::string suffix = word.substr(lastIndex);
    Node *lastPrefixState = traversePrefix(prefix, currFreq);
    if (lastPrefixState != NULL && lastPrefixState->branches.size() != 0)
    {
        replace_or_register(lastPrefixState, lastIndex, prevFreq, currFreq);
    }
    addSuffix(suffix, currFreq, lastPrefixState);
    lastWord = word;
}

Node *Trie::traversePrefix(std::string prefix, int freq)
{
    Node *tempNode = rootNode;
    for (int i = 0; i < prefix.length(); i++)
    {
        tempNode->frequency += freq;
        tempNode = tempNode->hasLetter(prefix[i]);
    }
    tempNode->frequency += freq;
    return tempNode;
}
void Trie::replace_or_register(Node *curr, int index, int prevFreq, int currFreq)
{
    Node *child = curr->hasLetter(lastWord[index]);
    if (child != NULL && child->registered != true)
    {
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
                registered[i]->frequency += prevFreq;
                curr->branches.find(lastWord[index])->second = registered[i];
                delete child;
            }
        }
        if (!found)
        {
            registered.push_back(child);
            child->registered = true;
        }
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
    bool terminal = false;
    for (int i = 0; i < word.length(); i++)
    {
        if (i == word.length() - 1)
        {
            terminal = true;
        }
        current = current->addLetter(word[i], freq, terminal);
    }
}

int Trie::getCommonPrefix(std::string current, std::string previous)
{
    int i = 0;
    while (i < previous.length() && previous[i] == current[i])
    {
        i++;
    }
    return i;
}

bool Trie::doesWordExist(std::string word, bool onlyRegistered)
{
    Node *lastNode = rootNode->contains(word, onlyRegistered);
    if (lastNode && lastNode->terminal == true)
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
    int numBranches = 0;
    for (int i = 0; i < registered.size(); i++)
    {
        numBranches += registered[i]->branches.size();
    }
    return numBranches;
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
    lastWord = "";
    // Depends whether you want to include 
    // the root node and its branches in the counts
    //registered.push_back(rootNode);
    rootNode->registered = true;
}

int main(int argc, char *argv[])
{

    std::ifstream file;
    file.open(argv[1], std::ios_base::in);
    Trie trie = Trie();
    std::cout << "Adding Lexicon." << std::endl;
    trie.addLexicon(file);
    std::cout << "Lexicon Added." << std::endl;

    char c = 0;
    bool exit = false;
    do
    {
        std::cin >> c;
        switch (c)
        {
        case 'a':
        {
            std::vector<std::string> words;
            std::ofstream output_file("./results.txt");
            std::cout << "Writing lexicon to file..." << std::endl;
            words = trie.getLexicon();
            std::ostream_iterator<std::string> output_iterator(output_file, "\n");
            std::copy(words.begin(), words.end(), output_iterator);
            std::cout << "Wrote lexicon to file." << std::endl;
            break;
        }
        case 'b':
        {
            std::cout << "Check ? exists:" << std::endl;
            std::string input;
            std::cin >> input;
            std::cout << "Checking if " << input << " exists" << std::endl;
            std::cout << trie.doesWordExist(input, true) << std::endl;
            break;
        }
        case 'c':
        {
            std::cout << "Graph of ?:" << std::endl;
            std::string input;
            std::cin >> input;
            Node *one = trie.rootNode->contains(input, true);
            if (one != NULL)
            {
                std::cout << "Graph of " << input << ":" << std::endl;
                one->printNode(0);
            }
            else
            {
                std::cout << "Does not exist in trie." << std::endl;
            }
            break;
        }
        case 'd':
        {
            std::cout << "Branch count:" << trie.getBranchCount() << std::endl;
            break;
        }
        case 'e':
        {
            std::cout << "Node count:" << trie.getNodeCount() << std::endl;
            break;
        }
        default:
        {
            exit = true;
            break;
        }
        }
    } while (!exit);
}