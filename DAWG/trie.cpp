#include <iostream>
#include <fstream>
#include <iterator>

#include "trie.h"
#include "node.h"

using namespace std;

void Trie::addLexicon(ifstream &file)
{
    string line;
    int lineCount = 0;
    int prevFreq = 0;
    int currFreq = 0;
    while (getline(file, line))
    {
        prevFreq = currFreq;
        int split = line.find(" ");
        string word = line.substr(0, split);
        currFreq = stoi(line.substr(split));
        processWord(word, prevFreq, currFreq);
        lineCount++;
    }
    replace_or_register(rootNode, 0, currFreq, 0);
}

void Trie::processWord(string word, int prevFreq, int currFreq)
{
    int lastIndex = getCommonPrefix(word, lastWord);
    string prefix = word.substr(0, lastIndex);
    string suffix = word.substr(lastIndex);
    Node *lastPrefixState = traversePrefix(prefix, currFreq);
    if (lastPrefixState != NULL && lastPrefixState->branches.size() != 0)
    {
        replace_or_register(lastPrefixState, lastIndex, prevFreq, currFreq);
    }
    addSuffix(suffix, currFreq, lastPrefixState);
    lastWord = word;
}

Node *Trie::traversePrefix(string prefix, int freq)
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
            if (equal(one->branches.begin(),
                           one->branches.end(), two->branches.begin()))
            {
                return true;
            }
        }
    }
    return false;
}

void Trie::addSuffix(string word, int freq, Node *current = NULL)
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
        current = current->addLetter(word[i], freq, terminal, latestId);
        if(current->id == latestId) {
            latestId++;
        }
    }
}

int Trie::getCommonPrefix(string current, string previous)
{
    int i = 0;
    while (i < previous.length() && previous[i] == current[i])
    {
        i++;
    }
    return i;
}

bool Trie::doesWordExist(string word)
{
    Node *lastNode = rootNode->hasWord(word);
    if (lastNode && lastNode->terminal == true)
    {
        return true;
    }
    else
    {
        return false;
    }
}
void Trie::calculateCounts()
{
    if(finalNodes.size() ==0) {
        finalNodes[0] = rootNode;
        findChildren(rootNode);
    }
}

void Trie::findChildren(Node* curr) 
{
    map<char, Node *>::iterator it;
    vector<Node*> children;
    for (it = curr->branches.begin(); it != curr->branches.end(); ++it)
    {
        branchCount++;
        if(finalNodes.find(it->second->id)== finalNodes.end()) {
            nodeCount++;
            finalNodes[it->second->id] = it->second;
            findChildren(it->second);
        }
    }
}

vector<string> Trie::getLexicon()
{
    vector<string> words = vector<string>();
    string word = "";
    rootNode->getWords(&words, word);
    return words;
}

Trie::Trie()
{
    rootNode = new Node(false, 0, latestId);
    latestId++;
    lastWord = "";
    // Depends whether you want to include 
    // the root node and its branches in the counts
    //registered.push_back(rootNode);
    rootNode->registered = true;
}

int main(int argc, char *argv[])
{

    ifstream file;
    file.open(argv[1], ios_base::in);
    Trie trie = Trie();
    cout << "Adding Lexicon." << endl;
    trie.addLexicon(file);
    cout << "Lexicon Added." << endl;

    char c = 0;
    bool exit = false;
    do
    {
        cin >> c;
        switch (c)
        {
        case 'a':
        {
            vector<string> words;
            ofstream output_file("./results.txt");
            cout << "Writing lexicon to file..." << endl;
            words = trie.getLexicon();
            ostream_iterator<string> output_iterator(output_file, "\n");
            copy(words.begin(), words.end(), output_iterator);
            cout << "Wrote lexicon to file." << endl;
            break;
        }
        case 'b':
        {
            cout << "Check ? exists:" << endl;
            string input;
            cin >> input;
            cout << "Checking if " << input << " exists" << endl;
            cout << trie.doesWordExist(input) << endl;
            break;
        }
        case 'c':
        {
            cout << "Graph of ?: (if you want the full graph type * " << endl;
            string input;
            cin >> input;
            Node* one;
            if(input == "*") {
                one = trie.rootNode;
            }
            else {
                one = trie.rootNode->hasWord(input);
            }
            if (one != NULL)
            {
                cout << "Graph of " << input << ":" << endl;
                one->printNode(0);
            }
            else
            {
                cout << "Does not exist in trie." << endl;
            }
            break;
        }
        case 'd':
        {
            trie.calculateCounts();
            cout << "Branch count:" << trie.branchCount << endl;
            cout << "Node count:" << trie.nodeCount << endl;
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