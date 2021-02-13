#include <iostream>
#include <fstream>
#include <string>


#include "trie.h"
#include "node.h"

void Trie::addLexicon(std::ifstream &file)
{
    std::string line;
    while (getline(file, line))
    {
        int split = line.find(" ");
        std::string word = line.substr(0, split);
        int freq = stoi(line.substr(split));
        addWord(word, freq);
    }
    rootNode->printNode(0);
}
void Trie::addWord(std::string word, int freq)
{
    Node *current = rootNode;
    for (int i = 0; i < word.length(); i++)
    {
        current = current->addLetter(word[i], freq);
    }
}

bool Trie::doesWordExist(std::string word)
{
    Node *lastNode = rootNode->contains(word);
    if (lastNode && lastNode->getBranches().size() == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
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
    rootNode = new Node();
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
    std::cout << trie.doesWordExist("hello") << std::endl;
    std::cout << trie.doesWordExist("e") << std::endl;
    std::cout << trie.doesWordExist("ho") << std::endl;
    std::cout << trie.doesWordExist("h") << std::endl;
    std::cout << trie.doesWordExist("hellooooo") << std::endl;
}