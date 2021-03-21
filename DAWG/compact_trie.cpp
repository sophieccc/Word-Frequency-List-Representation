#include <iostream>
#include <fstream>
#include <iterator>
#include <cmath>
#include "compact_trie.h"

using namespace std;

// Compacted = Read pre-created array ver. of trie.
// Non-Compacted = Read word-freq list to first create pointer ver. of trie.
CompactTrie::CompactTrie(string fileName, bool compacted)
{
    createCode();
    ifstream file;
    file.open(fileName, ios_base::in);
    if (compacted)
    {
        readFromFile(fileName);
    }
    else
    {
        Trie trie = Trie();
        cout << "Adding Lexicon." << endl;
        trie.addLexicon(file);
        cout << "Lexicon Added." << endl;
        processTrie(trie);
    }
}

void CompactTrie::createCode()
{
    for (int i = 1; i <= 256; i += 64)
    {
        int index = i;
        for (char l = 'a'; l <= 'z'; ++l)
        {
            numToChar[index] = l;
            if (i == 1)
            {
                charToNum[l] = index;
            }
            index++;
        }
    }
}

void CompactTrie::processTrie(Trie t)
{
    counter = t.rootNode->branches.size();
    nodes.push(t.rootNode);
    while (!nodes.empty())
    {
        Node *curr = nodes.front();
        for (auto it = curr->branches.begin(); it != curr->branches.end(); ++it)
        {
            bool last = false;
            if (next(it) == curr->branches.end())
            {
                last = true;
            }
            int freq = curr->branchFreqs.find(it->first)->second;
            branchList.push_back(pair<pair<char, int>, bool>(pair<char, int>(it->first, freq), last));
            processNode(it->second, last);
        }
        nodes.pop();
    }
}

void CompactTrie::processNode(Node *n, bool last)
{
    if (n->counter == -1)
    {
        n->counter = counter;
        if (n->terminal && n->branches.size() == 0)
        {
            nodeList.push_back(pair<int, bool>(0, n->terminal));
        }
        else
        {
            nodeList.push_back(pair<int, bool>(counter, n->terminal));
        }
        nodes.push(n);
        counter += n->branches.size();
    }
    else
    {
        if (n->terminal && n->branches.size() == 0)
        {
            nodeList.push_back(pair<int, bool>(0, n->terminal));
        }
        else
        {
            nodeList.push_back(pair<int, bool>(n->counter, n->terminal));
        }
    }
}

bool CompactTrie::doesWordExist(string word)
{
    int index = 0;
    for (int i = 0; i < word.size(); i++)
    {
        bool foundChar = false;
        bool lastBranch = false;
        while (!foundChar && !lastBranch)
        {
            lastBranch = branchList[index].second;
            if (branchList[index].first.first == word[i])
            {
                if (i < word.size() - 1)
                {
                    index = nodeList[index].first;
                }
                foundChar = true;
            }
            else
            {
                index++;
            }
        }
        if (!foundChar)
        {
            return false;
        }
    }
    return nodeList[index].second;
}

void CompactTrie::writeToFile(string fileName)
{
    ofstream outfile;
    outfile.open(fileName, ios::out | ios::binary);
    unsigned int listSize = branchList.size();
    outfile.write(reinterpret_cast<char *>(&listSize), sizeof(int));
    for (auto it = branchList.begin(); it != branchList.end(); ++it)
    {
        unsigned char letter = charToNum.at(it->first.first);
        if (it->second)
        {
            letter += 64;
        }
        int index = it - branchList.begin();
        if (nodeList.at(index).second == true)
        {
            letter += 128;
        }
        outfile.write((char *)(&letter), sizeof(letter));
        unsigned int frequency = it->first.second;
        writeInteger(frequency, &outfile);
    }
    for (auto it = nodeList.begin(); it != nodeList.end(); ++it)
    {
        unsigned int index = it->first;
        writeInteger(index, &outfile);
    }
}

void CompactTrie::writeInteger(unsigned int index, ofstream *outfile)
{
    unsigned char curr;
    if (index < 128)
    {
        curr = index;
        outfile->write((char *)(&curr), sizeof(curr));
    }
    else
    {
        curr = ((index % 128) + 128);
        outfile->write((char *)(&curr), sizeof(curr));
        index /= 128;
        if (index < 128)
        {
            curr = index;
            outfile->write((char *)(&curr), sizeof(curr));
        }
        else
        {
            curr = ((index % 128) + 128);
            outfile->write((char *)(&curr), sizeof(curr));
            curr = (index / 128);
            outfile->write((char *)(&curr), sizeof(curr));
        }
    }
}

void CompactTrie::readFromFile(string fileName)
{
    std::ifstream infile;
    infile.open(fileName, ios::in | ios::binary);
    unsigned int listSize = 0;
    infile.read(reinterpret_cast<char *>(&listSize), sizeof(int));
    unsigned char curr;
    vector<bool> terminality;
    for (int j = 0; j < listSize; j++)
    {
        infile.read((char *)(&curr), sizeof(curr));
        bool lastBranch = false;
        unsigned char letter = numToChar[int(curr)];
        if (int(curr) > 64)
        {
            lastBranch = true;
        }
        if (int(curr) > 128)
        {
            terminality.push_back(true);
        }
        else
        {
            terminality.push_back(false);
        }
        infile.read((char *)(&curr), sizeof(char));
        unsigned int frequency = getIntegerVal(&infile, curr);
        branchList.push_back(pair<pair<char, int>, bool>(pair<char, int>(letter, frequency), lastBranch));
    }
    int i = 0;
    while (infile.read((char *)(&curr), sizeof(char)))
    {
        bool terminal = terminality[i];
        i++;
        int index = getIntegerVal(&infile, curr);
        nodeList.push_back(pair<int, bool>(index, terminal));
    }
}

int CompactTrie::getIntegerVal(ifstream *infile, unsigned char curr)
{
    int index = 0;
    if (curr <= 127)
    {
        index += curr;
    }
    else
    {
        unsigned char secondChar;
        unsigned char thirdChar;
        int remainingBits = (int)pow(2, 14);
        index += (curr - 128);
        infile->read((char *)(&secondChar), sizeof(secondChar));
        if (secondChar > 127)
        {
            index += ((secondChar - 128) * 128);
            infile->read((char *)(&thirdChar), sizeof(thirdChar));
            index += (thirdChar * remainingBits);
        }
        else
        {
            index += (secondChar * 128);
        }
    }
    return index;
}

void CompactTrie::displayLists()
{
    cout << "branchList" << endl;
    for (auto it = branchList.begin(); it != branchList.end(); ++it)
    {
        cout << "(" << it->first.first << "," << it->second << ","
             << "f=" << it->first.second << ") ";
    }
    cout << endl << "nodeList" << endl;
    for (auto it = nodeList.begin(); it != nodeList.end(); ++it)
    {
        cout << "(" << it->first << "," << it->second << ") ";
    }
}

void CompactTrie::writeLexicon()
{
    vector<string> words;
    getWords(&words, "", 0);
    ofstream output_file("./results.txt");
    for(int i=0; i < words.size(); i++)
    {
        output_file << words.at(i) << '\t' << getWordFrequency(words.at(i)) << endl;
    }
}

void CompactTrie::getWords(vector<string> *words, string word, int index)
{
    bool lastBranch = false;
    for (int ind = index; !lastBranch; ind++)
    {
        lastBranch = branchList[ind].second;
        string tempWord = word + branchList[ind].first.first;
        if (nodeList[ind].first==0)
        {
            words->push_back(tempWord);
        }
        else
        {
            if (nodeList[ind].second == true)
            {
                words->push_back(tempWord);
            }
            getWords(words, tempWord, nodeList[ind].first);
        }
    }
}

int CompactTrie::getWordFrequency(string word)
{
    int curr = 0;
    int nodeFreq = getTotal(curr);
    int dawgFreq = nodeFreq;
    int currResult = 0;
    int branchFreq = 0;
    int terminalFreq = 0;
    for (int i = 0; i < word.size(); i++)
    {
        curr = findLetter(curr, word[i]);
        branchFreq = branchList[curr].first.second;
        currResult += (branchFreq - nodeFreq);
        nodeFreq = getTotal(nodeList[curr].first);
        if (nodeList[curr].second == true && i != word.size() - 1)
        {
            terminalFreq += (nodeFreq - branchFreq);
        }
    }
    currResult += terminalFreq;
    if (nodeList[curr].first != 0)
    {
        currResult -= nodeFreq;
    }
    currResult += dawgFreq;
    return currResult;
}

int CompactTrie::getTotal(int index)
{
    int total = 0;
    bool lastBranch = false;
    for (int ind = index; !lastBranch; ind++)
    {
        lastBranch = branchList[ind].second;
        total += branchList[ind].first.second;
    }
    return total;
}

int CompactTrie::findLetter(int index, char letter)
{
    int branch = -1;
    bool lastBranch = false;
    for (int ind = index; !lastBranch; ind++)
    {
        if(branchList[ind].first.first == letter)
        {
            branch = ind;
            lastBranch = true;
        }
    }
    return branch;
}

int main(int argc, char *argv[])
{
    CompactTrie compactTrie = CompactTrie(argv[1], false);
    compactTrie.displayLists();
    cout << endl << endl;
    compactTrie.writeToFile("compact.txt");
    CompactTrie compactTrie2 = CompactTrie("compact.txt", true);
    compactTrie2.displayLists();
    compactTrie2.writeLexicon();
}