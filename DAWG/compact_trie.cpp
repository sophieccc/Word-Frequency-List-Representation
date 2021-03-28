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
        createCode();
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
            if (alphabet.find(it->first) == alphabet.end())
            {
                alphabet.insert(it->first);
            }
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

void CompactTrie::createCode()
{
    for (int i = 1; i <= 256; i += 64)
    {
        int index = i;
        for (auto it = alphabet.begin(); it != alphabet.end(); ++it)
        {
            numToChar[index] = *it;
            if (i == 1)
            {
                charToNum[*it] = index;
            }
            index++;
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
    for (int i = 0; i < words.size(); i++)
    {
        output_file << words.at(i) << endl;
    }
}

void CompactTrie::getWords(vector<string> *words, string word, int index)
{
    bool lastBranch = false;
    for (int ind = index; !lastBranch; ind++)
    {
        lastBranch = branchList[ind].second;
        string tempWord = word + branchList[ind].first.first;
        if (nodeList[ind].first == 0)
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
        if (i != word.size() - 1)
        {
            curr = nodeList[curr].first;
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
        if (branchList[ind].first.first == letter)
        {
            branch = ind;
            lastBranch = true;
        }
    }
    return branch;
}

/* *
FILE I/O SECTION
*  */

void CompactTrie::writeToFile(string fileName)
{
    ofstream outfile;
    outfile.open(fileName, ios::out | ios::binary);
    unsigned char alphabetSize = alphabet.size();
    outfile.write((char *)(&alphabetSize), sizeof(alphabetSize));
    for (auto it = alphabet.begin(); it != alphabet.end(); ++it)
    {
        outfile.write((char *)(&(*it)), sizeof(char));
    }
    unsigned int listSize = branchList.size();
    outfile.write(reinterpret_cast<char *>(&listSize), sizeof(int));
    int queueMode = getIntegerMode(listSize);

    unsigned int maxFreq = getMaxFrequency();
    setMinLogBase(maxFreq);
    unsigned int logMaxFreq = round((log(maxFreq) / log(logBase)) * multiplier);
    outfile.write(reinterpret_cast<char *>(&logBase), sizeof(int));
    outfile.write(reinterpret_cast<char *>(&logMaxFreq), sizeof(int));

    int freqMode = getIntegerMode(logMaxFreq);
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
        writeInteger(frequency, &outfile, freqMode, true);
    }
    for (auto it = nodeList.begin(); it != nodeList.end(); ++it)
    {
        unsigned int index = it->first;
        writeInteger(index, &outfile, queueMode, false);
    }
}

int CompactTrie::getMaxFrequency()
{
    int maxFreq = 0;
    for (auto it = branchList.begin(); it != branchList.end(); ++it)
    {
        int freq = it->first.second;
        if (freq > maxFreq)
        {
            maxFreq = freq;
        }
    }
    return maxFreq;
}

void CompactTrie::setMinLogBase(int maxFreq)
{
    float divisor = (float)multiplier / 100.0;
    float maxLog = log(maxFreq) / log(logBase);
    float difference = (5.0 / (float)multiplier);
    float limit = ((float)multiplier / divisor - difference) / 10;
    while (maxLog >= limit && logBase < 20)
    {
        logBase++;
        maxLog = log(maxFreq) / log(logBase);
    }
}

int CompactTrie::getIntegerMode(int listSize)
{
    if (listSize < 256)
    {
        return 2;
    }
    else if (listSize < 32768)
    {
        return 1;
    }
    else if (listSize < 65536)
    {
        return 3;
    }
    else if (listSize < 4194304)
    {
        return -1;
    }
    else if (listSize < 8388608)
    {
        return 0;
    }
    else
    {
        return 4;
    }
}

void CompactTrie::writeInteger(unsigned int index, ofstream *outfile, int mode, bool logVals)
{
    if (logVals == true)
    {
        if (index <= 0)
        {
            index = 1;
        }
        index = round((log(index) / log(logBase)) * (double)multiplier);
    }
    if (mode == -1)
    {
        origIntegerWrite(index, outfile);
    }
    else if (mode == 1)
    {
        oneOrTwoBytesWrite(index, outfile);
    }
    else if( mode ==0)
    {
        twoOrThreeBytesWrite(index, outfile);
    }
    else
    {
        unsigned char firstChar = index % 256;
        outfile->write((char *)(&firstChar), sizeof(firstChar));
        if (mode >= 3)
        {
            unsigned char secondChar = index / 256;
            outfile->write((char *)(&secondChar), sizeof(secondChar));
            if (mode == 4)
            {
                unsigned char thirdChar = index / (256 * 256);
                outfile->write((char *)(&thirdChar), sizeof(thirdChar));
            }
        }
    }
}

void CompactTrie::origIntegerWrite(unsigned int index, ofstream *outfile)
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

void CompactTrie::oneOrTwoBytesWrite(unsigned int index, ofstream *outfile)
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
        curr = index;
        outfile->write((char *)(&curr), sizeof(curr));
    }
}

void CompactTrie::twoOrThreeBytesWrite(unsigned int index, ofstream *outfile)
{
    unsigned char firstChar = ((index % 256) + 256);
    cout << int(firstChar) << endl;
    outfile->write((char *)(&firstChar), sizeof(firstChar));
    index /= 256;
    unsigned char secondChar;
    if (index < 128)
    {
        secondChar = index;
        cout << "two " << int(secondChar) << endl;
        outfile->write((char *)(&secondChar), sizeof(secondChar));
    }
    else
    {
        secondChar = (index % 256) + 256;
        cout << "three " << int(secondChar) << endl;
        outfile->write((char *)(&secondChar), sizeof(secondChar));
        unsigned char thirdChar = index / 128;
        cout << int(thirdChar) << endl;
        outfile->write((char *)(&thirdChar), sizeof(thirdChar));
    }
}

void CompactTrie::readFromFile(string fileName)
{
    std::ifstream infile;
    infile.open(fileName, ios::in | ios::binary);
    unsigned char alphabetSize;
    infile.read((char *)(&alphabetSize), sizeof(alphabetSize));
    for (int i = 0; i < alphabetSize; i++)
    {
        unsigned char letter;
        infile.read((char *)(&letter), sizeof(letter));
        alphabet.insert(letter);
    }
    createCode();
    unsigned int listSize = 0;
    infile.read(reinterpret_cast<char *>(&listSize), sizeof(int));
    int queueMode = getIntegerMode(listSize);
    infile.read(reinterpret_cast<char *>(&logBase), sizeof(int));

    unsigned int maxFreq = 0;
    infile.read(reinterpret_cast<char *>(&maxFreq), sizeof(maxFreq));
    int freqMode = getIntegerMode(maxFreq);
    readArrays(listSize, queueMode, freqMode, &infile);
}

void CompactTrie::readArrays(int listSize, int queueMode, int freqMode, ifstream *infile)
{
    unsigned char curr;
    vector<bool> terminality;
    for (int j = 0; j < listSize; j++)
    {
        infile->read((char *)(&curr), sizeof(curr));
        bool lastBranch = false;
        unsigned char letter = numToChar[int(curr)];
        if (int(curr) > 192 || (int(curr) > 64 && int(curr) < 129))
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
        infile->read((char *)(&curr), sizeof(char));
        unsigned int frequency = getIntegerVal(infile, curr, freqMode, true);
        branchList.push_back(pair<pair<char, int>, bool>(pair<char, int>(letter, frequency), lastBranch));
    }
    int i = 0;
    while (infile->read((char *)(&curr), sizeof(char)))
    {
        bool terminal = terminality[i];
        i++;
        int index = getIntegerVal(infile, curr, queueMode, false);
        nodeList.push_back(pair<int, bool>(index, terminal));
    }
}

int CompactTrie::getIntegerVal(ifstream *infile, unsigned char firstChar, int mode, bool logVals)
{
    unsigned int index = 0;
    if (mode == -1)
    {
        index = origIntegerRead(infile, firstChar);
    }
    else if (mode == 1)
    {
        index = oneOrTwoBytesRead(infile, firstChar);
    }
    else if (mode ==0)
    {
        index = twoOrThreeBytesRead(infile, firstChar);
    }
    else
    {
        unsigned char secondChar = 0;
        unsigned char thirdChar = 0;
        if (mode >= 3)
        {
            infile->read((char *)(&secondChar), sizeof(secondChar));
            if (mode == 4)
            {
                infile->read((char *)(&thirdChar), sizeof(thirdChar));
            }
        }
        index = firstChar + (secondChar * 256) + (256 * 256 * thirdChar);
    }
    if (logVals == true)
    {
        if (index == 0)
        {
            index = 1;
        }
        else
        {
            float z = (float)index / (float)multiplier;
            index = round(pow(logBase, z));
        }
    }
    return index;
}

int CompactTrie::origIntegerRead(ifstream *infile, unsigned char curr)
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

int CompactTrie::oneOrTwoBytesRead(ifstream *infile, unsigned char curr)
{
    int index = 0;
    if (curr <= 127)
    {
        index += curr;
    }
    else
    {
        unsigned char secondChar;
        index += (curr - 128);
        infile->read((char *)(&secondChar), sizeof(secondChar));
        index += (secondChar * 128);
    }
    return index;
}

int CompactTrie::twoOrThreeBytesRead(ifstream *infile, unsigned char curr)
{
    int index = curr;
    unsigned char secondChar;
    unsigned char thirdChar;
    int remainingBits = (int)pow(2, 15);

    infile->read((char *)(&secondChar), sizeof(secondChar));
    if (secondChar > 127)
    {
        index += ((secondChar - 128) * 256);
        infile->read((char *)(&thirdChar), sizeof(thirdChar));
        index += (thirdChar * remainingBits);
    }
    else
    {
        index += (secondChar * 256);
    }
    return index;
}

int main(int argc, char *argv[])
{
    CompactTrie compactTrie = CompactTrie(argv[1], false);
    compactTrie.writeToFile("compact.txt");
    CompactTrie compactTrie2 = CompactTrie("compact.txt", true);
    compactTrie2.writeLexicon();
}