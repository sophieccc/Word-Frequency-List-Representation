#include <iostream>
#include <fstream>
#include <iterator>
#include "trie.h"

using namespace std;

// Adds all words in the given file to the trie.
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
    minimise(rootNode, 0, currFreq, 0);
}

// Adds the current word to the trie and minimises 
// the non-shared suffix of the previous word. 
// For an explanation of the process please see the write-up.
void Trie::processWord(string word, int prevFreq, int currFreq)
{
    int lastIndex = getCommonPrefix(word, lastWord);
    string prefix = word.substr(0, lastIndex);
    string suffix = word.substr(lastIndex);
    Node *lastPrefixState = traversePrefix(prefix, currFreq);
    if (lastPrefixState != NULL && lastPrefixState->branches.size() != 0)
    {
        minimise(lastPrefixState, lastIndex, prevFreq, currFreq);
    }
    addSuffix(suffix, currFreq, lastPrefixState);
    lastWord = word;
}

// Adds the current word's frequency to each traversed branch.
Node *Trie::traversePrefix(string prefix, int freq)
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

// Makes it so node-sharing occures for suffixes -- what makes the trie a DAWG.
void Trie::minimise(Node *curr, int index, int prevFreq, int currFreq)
{
    shared = false;
    Node *child = curr->hasLetter(lastWord[index]);
    if (child != NULL && child->registered != true)
    {
        if (child->branches.size() != 0)
        {
            minimise(child, index + 1, prevFreq, currFreq);
        }
        // Gets all nodes in the minimised set that could be equivalent
        // to the current child node, i.e. have the same hash.
        auto res = minSet.equal_range(child);
        if (res.first == minSet.end())
        {
            child->registered = true;
            minSet.insert(child);
            // If previous nodes traversed were shared. Once sharing is no
            // longer possible, the word's frequency is added to all successor nodes.
            if (shared == true && index < lastWord.size() - 1)
            {
                addFrequencies(child->branches.find(lastWord[index + 1])->second, prevFreq);
            }
            shared = false;
        }
        else
        {
            bool found = false;
            for (auto it = res.first; it != res.second && !found; ++it)
            {
                Node *n = *it;
                if (checkEquivalence(n, child) == true)
                {
                    shared = true;
                    found = true;
                    curr->branches.find(lastWord[index])->second = n;
                    delete child;
                }
            }
        }
    }
}

// Adds frequencies to all suffix nodes when shared. 
void Trie::addFrequencies(Node *n, int freq)
{
    if (n->terminal == false)
    {
        for (auto it = n->branches.begin(); it != n->branches.end(); ++it)
        {
            n->branchFreqs.find(it->first)->second += freq;
            addFrequencies(it->second, freq);
        }
    }
}

// Checks if two nodes are equivalent.
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

// Adds new nodes to the trie for a word's currently non-shared suffix.
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
        if (current->id == latestId)
        {
            latestId++;
        }
    }
}

// Gets the original word frequency of a word from the trie. 
// This function does not always work due to fundamental DAWG aspects 
// that mean some information is lost. It should only be used for debugging.
int Trie::getWordFrequency(string word)
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
int Trie::getTotal(Node *n)
{
    int total = 0;
    for (auto it = n->branchFreqs.begin(); it != n->branchFreqs.end(); ++it)
    {
        total += it->second;
    }
    return total;
}

// Gets the common prefix of two words.
int Trie::getCommonPrefix(string current, string previous)
{
    int i = 0;
    while (i < previous.length() && previous[i] == current[i])
    {
        i++;
    }
    return i;
}

// Checks if a given word exists in the trie.
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

// Calculates how many nodes and branches are in the trie.
void Trie::calculateCounts()
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

// Gets every possible word in the trie.
vector<string> Trie::getLexicon()
{
    vector<string> words = vector<string>();
    string word = "";
    rootNode->getWords(&words, word);
    return words;
}

Trie::Trie()
{
    rootNode = new Node(false, latestId);
    latestId++;
    lastWord = "";
    minSet.insert(rootNode);
    rootNode->registered = true;
    nodeCount = 0;
    branchCount = 0;
}

/* int main(int argc, char *argv[])
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
            Node *one;
            if (input == "*")
            {
                one = trie.rootNode;
            }
            else
            {
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
        case 'e':
        {
            cout << "Check ? frequency:" << endl;
            string input;
            cin >> input;
            cout << "Checking frequency of " << input << endl;
            cout << trie.getWordFrequency(input) << endl;
            break;
        }
        default:
        {
            exit = true;
            break;
        }
        }
    } while (!exit);
} */