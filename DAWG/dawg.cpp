#include <iostream>
#include <fstream>
#include <iterator>
#include "dawg.h"

using namespace std;

// Adds all words in the given file to the dawg.
void Dawg::addLexicon(ifstream &file)
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
    minimise(rootNode, 0, currFreq, 0);
}

// Adds the current word to the dawg and minimises 
// the non-shared suffix of the previous word. 
// For an explanation of the process please see the write-up.
void Dawg::processWord(string currWord, int prevFreq, int currFreq)
{
    int prefixPos = getCommonPrefix(currWord, prevWord);
    string prefix = currWord.substr(0, prefixPos);
    string suffix = currWord.substr(prefixPos);
    Node *lastPrefixNode = traversePrefix(prefix, currFreq);
    if (lastPrefixNode != NULL && lastPrefixNode->branches.size() != 0)
    {
        minimise(lastPrefixNode, prefixPos, prevFreq, currFreq);
    }
    addSuffix(suffix, currFreq, lastPrefixNode);
    prevWord = currWord;
}

// Adds the current word's frequency to each traversed branch.
Node *Dawg::traversePrefix(string prefix, int freq)
{
    Node *tempNode = rootNode;
    for (int i = 0; i < prefix.length(); i++)
    {
        auto it = tempNode->branchFreqs.find(prefix[i]);
        if (it != tempNode->branchFreqs.end())
        {
            it->second += freq;
        }
        tempNode = tempNode->hasLetter(prefix[i]);
    }
    return tempNode;
}

// Makes it so node-sharing occures for suffixes -- what makes the dawg a DAWG.
void Dawg::minimise(Node *curr, int index, int prevFreq, int currFreq)
{
    Node *child = curr->hasLetter(prevWord[index]);
    if (child != NULL && child->registered != true)
    {
        if (child->branches.size() != 0)
        {
            minimise(child, index + 1, prevFreq, currFreq);
        }
        // Gets all nodes in the minimised set that could be equivalent
        // to the current child node, i.e. have the same hash.
        Node* equivNode = findEquivNode(child);
        if(equivNode == NULL)
        {
            addNode(child, index, prevFreq);
        }
        else 
        {
            addFrequencies(equivNode,child);
            curr->branches.find(prevWord[index])->second = equivNode;
            delete child;
        }
    }
}

Node* Dawg::findEquivNode(Node* child)
{
    Node* equivNode = NULL;
    auto res = minSet.equal_range(child);
    if (res.first != minSet.end())
    {
        bool found = false;
        for (auto it = res.first; it != res.second && !found; ++it)
        {
            Node *n = *it;
            if (checkEquivalence(n, child) == true)
            {
                found = true;
                equivNode = n;
            }
        }
    }
    return equivNode;
}

void Dawg::addNode(Node* child, int index, int prevFreq)
{
    child->registered = true;
    minSet.insert(child);
}

// Adds frequencies to all suffix nodes when shared. 
void Dawg::addFrequencies(Node *n, /* int freq */ Node *n2)
{
    for (auto it = n->branches.begin(), it2 = n2->branches.begin(); it != n->branches.end(); ++it, ++it2)
    {
        n->branchFreqs.find(it->first)->second += n2->branchFreqs.find(it->first)->second;
    }
}

// Checks if two nodes are equivalent.
bool Dawg::checkEquivalence(Node *one, Node *two)
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

// Adds new nodes to the dawg for a word's currently non-shared suffix.
void Dawg::addSuffix(string word, int freq, Node *current = NULL)
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
        if (current->id == latestId)
        {
            latestId++;
        }
    }
}

// Gets the original word frequency of a word from the dawg. 
// This function does not always work due to fundamental DAWG aspects 
// that mean some information is lost. It should only be used for debugging.
int Dawg::getWordFrequency(string word)
{
    if (doesWordExist(word))
    {
        Node *curr = rootNode;
        // The total frequency of the node's outgoing branches.
        int nodeFreq = getTotal(curr);
        // The total frequency of the entire dawg (every word).
        int dawgFreq = nodeFreq;
        // The current frequency result for the word.
        int currResult = 0;
        // The frequency of the branch for the current letter.
        int branchFreq = 0;
        // The frequency of all terminal nodes that have been traversed.
        int terminalFreq = 0;
        for (int i = 0; i < word.size(); i++)
        {
            branchFreq = curr->branchFreqs.find(word[i])->second;
            currResult += (branchFreq - nodeFreq);
            curr = curr->branches.find(word[i])->second;
            nodeFreq = getTotal(curr);
            // Subtracting the frequency going into the next node from the
            // total outgoing frequency of the next node gives the frequency that
            // is no longer going anywhere, i.e. the freq of the terminating word.
            if (curr->terminal && i != word.size() - 1)
            {
                terminalFreq += (nodeFreq - branchFreq);
            }
        }
        currResult += terminalFreq;
        // Need to take into account outgoing frequencies if there are
        // more branches coming out of the final node.
        if (curr->branches.size() > 0)
        {
            currResult -= nodeFreq;
        }
        currResult += dawgFreq;
        return currResult;
    }
    else
    {
        return -1;
    }
}

// Gets the total frequency of all the branches coming out of a node. 
int Dawg::getTotal(Node *n)
{
    int total = 0;
    for (auto it = n->branchFreqs.begin(); it != n->branchFreqs.end(); ++it)
    {
        total += it->second;
    }
    return total;
}

// Gets the common prefix of two words.
int Dawg::getCommonPrefix(string current, string previous)
{
    int i = 0;
    while (i < previous.length() && previous[i] == current[i])
    {
        i++;
    }
    return i;
}

// Checks if a given word exists in the dawg.
bool Dawg::doesWordExist(string word)
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

// Calculates how many nodes and branches are in the dawg.
void Dawg::calculateCounts()
{
    if (nodeCount == 0)
    {
        nodeCount = minSet.size();
        for (auto it = minSet.begin(); it != minSet.end(); ++it)
        {
            Node *curr = *it;
            for (auto iter = curr->branches.begin(); iter != curr->branches.end(); ++iter)
            {
                branchCount++;
            }
        }
    }
}

// Gets every possible word in the dawg.
vector<string> Dawg::getLexicon()
{
    vector<string> words = vector<string>();
    string word = "";
    rootNode->getWords(&words, word);
    return words;
}

Dawg::Dawg()
{
    rootNode = new Node(false, latestId);
    latestId++;
    prevWord = "";
    minSet.insert(rootNode);
    rootNode->registered = true;
    nodeCount = 0;
    branchCount = 0;
}

int main(int argc, char *argv[])
{

    ifstream file;
    file.open(argv[1], ios_base::in);
    Dawg dawg = Dawg();
    cout << "Adding Lexicon." << endl;
    dawg.addLexicon(file);
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
            words = dawg.getLexicon();
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
            cout << dawg.doesWordExist(input) << endl;
            break;
        }
        case 'c':
        {
            cout << "Graph of ?: (if you want the full graph type * " << endl;
            string input;
            cin >> input;
            Node *one;
            if (input == "*")
            {
                one = dawg.rootNode;
            }
            else
            {
                one = dawg.rootNode->hasWord(input);
            }
            if (one != NULL)
            {
                cout << "Graph of " << input << ":" << endl;
                one->printNode(0);
            }
            else
            {
                cout << "Does not exist in dawg." << endl;
            }
            break;
        }
        case 'd':
        {
            dawg.calculateCounts();
            cout << "Branch count:" << dawg.branchCount << endl;
            cout << "Node count:" << dawg.nodeCount << endl;
            break;
        }
        case 'e':
        {
            cout << "Check ? frequency:" << endl;
            string input;
            cin >> input;
            cout << "Checking frequency of " << input << endl;
            cout << dawg.getWordFrequency(input) << endl;
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