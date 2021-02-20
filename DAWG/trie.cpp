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
    while (getline(file, line) && lineCount<100000)
    {
        lineCount++;
        int split = line.find(" ");
        std::string word = line.substr(0, split);
        int freq = stoi(line.substr(split));
        processWord(word, freq);
    }
    rootNode->printNode(0);
}
void Trie::addSuffix(std::string word, int freq, Node*current=NULL)
{
    if (current == NULL) {
        current = rootNode;
    }
    std::tuple <Node*, int,int> results;
    bool terminal = false;
    for (int i = 0; i < word.length(); i++)
    {
        if (i== word.length()-1) {
            terminal = true;
        }
        results = current->addLetter(word[i], freq, terminal);
        current = std::get<0>(results);
        branchCount += std::get<1>(results);
        nodeCount += std::get<2>(results);
        allNodes.push_back(current);
    }
}

void Trie::processWord(std::string word, int freq)
{
    int lastIndex = getCommonPrefix(word, lastWord);
    std::string prefix = word.substr(0,lastIndex);
    std::string suffix = word.substr(lastIndex);
    Node* lastPrefixState = rootNode->contains(prefix);
    if(lastPrefixState->branches.size()!=0) {
        replace_or_register(lastPrefixState);
    }
    addWord(suffix, freq, lastPrefixState);
}

void Trie::replace_or_register(Node* curr) 
{
}
int Trie::getCommonPrefix(std::string current, std::string previous) 
{
    int i;
    for(i=0; i < previous.length(); i++) {
        if(previous[i] != current[i]) {
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
    return nodeCount;
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
    allNodes.push_back(rootNode);
    branchCount = 0;
    nodeCount = 1;
    lastWord = "";
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