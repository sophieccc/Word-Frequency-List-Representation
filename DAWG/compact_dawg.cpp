#include <iostream>
#include <fstream>
#include <iterator>
#include <cmath>
#include "compact_dawg.h"

using namespace std;

// Compacted = TRUE -> Read in pre-created array ver. of dawg.
// Compacted = FALSE-> Read in word-freq list to first create pointer ver. of dawg.
CompactDawg::CompactDawg(string fileName, bool compacted)
{
    ifstream file;
    file.open(fileName, ios_base::in);
    if (compacted)
    {
        readFromFile(fileName);
    }
    else
    {
        Dawg dawg = Dawg();
        cout << "Adding Lexicon." << endl;
        dawg.addLexicon(file);
        cout << "Lexicon Added." << endl;
        processDawg(dawg);
        createCode();
    }
}

// Converts pointer-dawg (dawg.cpp) into a compacted array-dawg.
void CompactDawg::processDawg(Dawg t)
{
    counter = t.rootNode->branches.size();
    nodes.push(t.rootNode);
    // Processes the nodes in a queue.
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
            branchList.push_back(
                pair<pair<char, int>, bool>(pair<char, int>(it->first, freq), last));
            processNode(it->second, last);
        }
        nodes.pop();
    }
}

// Processes the current pointer node in order to create the compact dawg.
void CompactDawg::processNode(Node *n, bool last)
{
    // If the node has not been processed before,
    // the overall counter is used, i.e. the node points to
    // the current end of the array.
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
    // Otherwise, the node's current counter is used, i.e. the
    // node it points to is already known.
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

// Creates a mapping of letters to numbers and numbers to letters.
// The numbers determine whether the current branch is the last
// branch of a node and if the node it points to is terminal.
void CompactDawg::createCode()
{
    for (int i = 1; i <= 256; i += 64)
    {
        int index = i;
        for (auto it = alphabet.begin(); it != alphabet.end(); ++it, index++)
        {
            numToChar[index] = *it;
            if (i == 1)
            {
                charToNum[*it] = index;
            }
        }
    }
}

// Checks if a given word exists in the dawg.
bool CompactDawg::doesWordExist(string word)
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

// Displays the arrays of branches and nodes.
void CompactDawg::displayLists()
{
    cout << "branchList" << endl;
    for (auto it = branchList.begin(); it != branchList.end(); ++it)
    {
        cout << "[L=" << it->first.first /*<< ", L=" << it->second */<< ","
             << " F=" << it->first.second << "]";
    }
    cout << endl
         << "nodeList" << endl;
    for (auto it = nodeList.begin(); it != nodeList.end(); ++it)
    {
        cout << "[N=" << it->first << ", T=" << it->second << "]";
    }
}

// Writes all the possible words in the dawg to a file.
void CompactDawg::writeLexicon()
{
    vector<string> words;
    getWords(&words, "", 0);
    ofstream output_file("./results.txt");
    for (int i = 0; i < words.size(); i++)
    {
        output_file << words.at(i) << endl;
    }
}

// Gets all possible words in the dawg.
void CompactDawg::getWords(vector<string> *words, string word, int index)
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

// Gets the original word frequency of a word from the dawg.
// This function does not always work due to fundamental DAWG aspects
// that mean some information is lost. It should only be used for debugging.
int CompactDawg::getWordFrequency(string word)
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

// Gets the total frequency of all the branches coming out of a node.
int CompactDawg::getTotal(int index)
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

// Finds the branch labelled with the given letter from
// the nodes at the current index in the node array.
int CompactDawg::findLetter(int index, char letter)
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

// Gets the relative likelihood of a word in the dawg.
double CompactDawg::getWordProbability(string word)
{
    if (doesWordExist(word))
    {
        double proba = 1.0;
        int index = 0;
        for (int i = 0; i < word.size(); i++)
        {
            // Get total frequency of the parent node.
            double total = getTotal(index);
            bool foundChar = false;
            while (!foundChar)
            {
                if (branchList[index].first.first == word[i])
                {
                    // Current branch's frequency divided by node frequency.
                    proba *= ((double)branchList[index].first.second) / total;
                    index = nodeList[index].first;
                    foundChar = true;
                }
                else
                {
                    index++;
                }
            }
        }
        return proba;
    }
    else
    {
        return 0;
    }
}

// Writes the probability of each given word to a file.
void CompactDawg::writeWordProbas(string inputFile, string outputFile)
{
    ifstream infile;
    infile.open(inputFile, ios_base::in);
    ofstream outfile;
    outfile.open(outputFile, ios::out);
    string line;
    while (getline(infile, line))
    {
        int split = line.find(" ");
        string word = line.substr(0, split);
        double proba = getWordProbability(word);
        outfile << proba << "\n";
    }
    infile.close();
    outfile.close();
}

/* *
FILE I/O SECTION
*  */

// Writes the compacted dawg to a file for future construction/use.
void CompactDawg::writeToFile(string fileName, bool useLogs)
{
    ofstream outfile;
    outfile.open(fileName, ios::out | ios::binary);
    writeAlphabet(&outfile);
    pair<int, int> modes = writeDawgInfo(&outfile, useLogs);
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
        writeInteger(frequency, &outfile, modes.second, useLogs);
    }
    for (auto it = nodeList.begin(); it != nodeList.end(); ++it)
    {
        unsigned int index = it->first;
        writeInteger(index, &outfile, modes.first, false);
    }
}

// Writes all of the symbols existing in the dawg to a file.
void CompactDawg::writeAlphabet(ofstream *outfile)
{
    unsigned char alphabetSize = alphabet.size();
    outfile->write((char *)(&alphabetSize), sizeof(alphabetSize));
    for (auto it = alphabet.begin(); it != alphabet.end(); ++it)
    {
        outfile->write((char *)(&(*it)), sizeof(char));
    }
}

// Writes all necessary information needed to reconstruct the dawg to a file.
// Returns the encoding modes for queue indices and frequencies.
pair<int, int> CompactDawg::writeDawgInfo(ofstream *outfile, bool useLogs)
{
    unsigned int listSize = branchList.size();
    outfile->write(reinterpret_cast<char *>(&listSize), sizeof(int));
    int queueMode = getIntegerMode(listSize);
    unsigned int maxFreq = getMaxFrequency();
    setMinLogBase(maxFreq);
    outfile->write(reinterpret_cast<char *>(&useLogs), sizeof(char));
    if (useLogs)
    {
        maxFreq = round((log(maxFreq) / log(logBase)) * multiplier);
    }
    outfile->write(reinterpret_cast<char *>(&logBase), sizeof(int));
    outfile->write(reinterpret_cast<char *>(&maxFreq), sizeof(int));
    int freqMode = getIntegerMode(maxFreq);
    return pair<int, int>(queueMode, freqMode);
}

// Gets the maximum possible frequency in the dawg.
int CompactDawg::getMaxFrequency()
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

// Sets the minimum possible log base to use in order for
// all of the logged frequencies up to maxFreq to be < 10.0
void CompactDawg::setMinLogBase(int maxFreq)
{
    float divisor = (float)multiplier / 100.0;
    float maxLog = log(maxFreq) / log(logBase);
    float difference = (5.0 / (float)multiplier);
    float limit = ((float)multiplier / divisor - difference) / 10;
    while (maxLog >= limit)
    {
        logBase++;
        maxLog = log(maxFreq) / log(logBase);
    }
}

// Gets the most suitable encoding mode to use based on what
// the maximum number needing to be encoded will be.
int CompactDawg::getIntegerMode(int maxNumber)
{
    if (maxNumber < 256)
    {
        return 1;
    }
    else if (maxNumber < 32768)
    {
        return 5;
    }
    else if (maxNumber < 65536)
    {
        return 2;
    }
    else if (maxNumber < 4194304)
    {
        return -1;
    }
    else if (maxNumber < 8388608)
    {
        return 0;
    }
    else if (maxNumber < 16777216)
    {
        return 3;
    }
    else
    {
        return 4;
    }
}

// Writes the current number to file in a suitable manner, based on mod and logVals.
void CompactDawg::writeInteger(unsigned int index, ofstream *outfile, int mode, bool logVals)
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
        anyByteWrite(index, outfile);
    }
    else if (mode == 5)
    {
        oneOrTwoBytesWrite(index, outfile);
    }
    else if (mode == 0)
    {
        twoOrThreeBytesWrite(index, outfile);
    }
    else if (mode == 4)
    {
        outfile->write((char *)(&index), sizeof(int));
    }
    else
    {
        unsigned char firstChar = index % 256;
        outfile->write((char *)(&firstChar), sizeof(firstChar));
        if (mode >= 2)
        {
            unsigned char secondChar = index / 256;
            outfile->write((char *)(&secondChar), sizeof(secondChar));
            if (mode == 3)
            {
                unsigned char thirdChar = index / (256 * 256);
                outfile->write((char *)(&thirdChar), sizeof(thirdChar));
            }
        }
    }
}

// The encoding mode used in Michael Burke's implementation of a DAWG.
// It can use one, two, or three bytes and therefore has a flag in both
// the first and second bytes.
void CompactDawg::anyByteWrite(unsigned int index, ofstream *outfile)
{
    unsigned char curr;
    if (index < 128)
    {
        curr = index;
        outfile->write((char *)(&curr), sizeof(curr));
    }
    else
    {
        curr = (index % 128) + 128;
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

// Writes the file in either one or two bytes, meaning only the first byte
// contains a flag.
void CompactDawg::oneOrTwoBytesWrite(unsigned int index, ofstream *outfile)
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
        curr = index / 128;
        outfile->write((char *)(&curr), sizeof(curr));
    }
}

// Writes the file in either two or three bytes, meaning only the second byte
// contains a flag.
void CompactDawg::twoOrThreeBytesWrite(unsigned int index, ofstream *outfile)
{
    unsigned char firstChar = index % 256;
    outfile->write((char *)(&firstChar), sizeof(firstChar));
    int dividedInd = index/256;
    unsigned char secondChar;
    if (dividedInd < 128)
    {
        secondChar = dividedInd;
        outfile->write((char *)(&secondChar), sizeof(secondChar));
    }
    else
    {
        secondChar = dividedInd % 256;
        if (secondChar < 128)
        {
            secondChar += 128;
        }
        outfile->write((char *)(&secondChar), sizeof(secondChar));
        unsigned char thirdChar = dividedInd / 128;
        outfile->write((char *)(&thirdChar), sizeof(thirdChar));
    }
}

// Reads the compacted dawg in from a file and constructs it.
void CompactDawg::readFromFile(string fileName)
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
    bool useLogs = false;
    infile.read(reinterpret_cast<char *>(&useLogs), sizeof(bool));
    infile.read(reinterpret_cast<char *>(&logBase), sizeof(int));
    unsigned int maxFreq = 0;
    infile.read(reinterpret_cast<char *>(&maxFreq), sizeof(maxFreq));
    int freqMode = getIntegerMode(maxFreq);
    readArrays(listSize, queueMode, freqMode, &infile, useLogs);
}

// Constructs the branchList and the nodeList.
void CompactDawg::readArrays(int listSize, int queueMode, int freqMode, ifstream *infile, bool useLogs)
{
    unsigned char curr;
    vector<bool> terminality;
    for (int j = 0; j < listSize; j++)
    {
        infile->read((char *)(&curr), sizeof(curr));
        pair<pair<char, int>, bool> branch =
            constructBranch(curr, freqMode, infile, useLogs, &terminality);
        branchList.push_back(branch);
    }
    int i = 0;
    int index = 0;
    if (queueMode == 4)
    {
        while (infile->read((char *)(&index), sizeof(int)))
        {
            bool terminal = terminality[i];
            i++;
            nodeList.push_back(pair<int, bool>(index, terminal));
        }
    }
    else
    {
        while (infile->read((char *)(&curr), sizeof(char)))
        {
            bool terminal = terminality[i];
            i++;
            index = getIntegerVal(infile, curr, queueMode, false);
            nodeList.push_back(pair<int, bool>(index, terminal));
        }
    }
}

// Constructs a branch for the compact dawg based on file information.
pair<pair<char, int>, bool> CompactDawg::constructBranch(
    unsigned char curr, int freqMode, ifstream *infile, bool useLogs, vector<bool> *terminality)
{
    bool lastBranch = false;
    unsigned char letter = numToChar[int(curr)];
    if (int(curr) > 192 || (int(curr) > 64 && int(curr) < 129))
    {
        lastBranch = true;
    }
    if (int(curr) > 128)
    {
        terminality->push_back(true);
    }
    else
    {
        terminality->push_back(false);
    }
    unsigned int frequency;
    if (freqMode == 4)
    {
        infile->read((char *)(&frequency), sizeof(int));
    }
    else
    {
        infile->read((char *)(&curr), sizeof(char));
        frequency = getIntegerVal(infile, curr, freqMode, useLogs);
    }
    return pair<pair<char, int>, bool>(pair<char, int>(letter,frequency),lastBranch);
}

// Gets an integer value from file based on the encoding mode.
int CompactDawg::getIntegerVal(ifstream *infile, unsigned char firstChar, int mode, bool logVals)
{
    unsigned int index = 0;
    if (mode == -1)
    {
        index = anyByteRead(infile, firstChar);
    }
    else if (mode == 5)
    {
        index = oneOrTwoBytesRead(infile, firstChar);
    }
    else if (mode == 0)
    {
        index = twoOrThreeBytesRead(infile, firstChar);
    }
    else
    {
        unsigned char secondChar = 0;
        unsigned char thirdChar = 0;
        if (mode >= 2)
        {
            infile->read((char *)(&secondChar), sizeof(secondChar));
            if (mode == 3)
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

// The encoding mode used in Michael Burke's implementation of a DAWG.
// It can use one, two, or three bytes and therefore has a flag in both
// the first and second bytes.
int CompactDawg::anyByteRead(ifstream *infile, unsigned char curr)
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

// Reads either one or two bytes from file, meaning only the first byte
// contains a flag.
int CompactDawg::oneOrTwoBytesRead(ifstream *infile, unsigned char curr)
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

// Reads either two or three bytes, meaning only the second byte contains a flag.
int CompactDawg::twoOrThreeBytesRead(ifstream *infile, unsigned char firstByte)
{
    int total = firstByte;
    unsigned char secondChar;
    infile->read((char *)(&secondChar), sizeof(secondChar));
    if (secondChar > 127)
    {
        total += ((secondChar - 128) * 256);
        unsigned char thirdChar;
        infile->read((char *)(&thirdChar), sizeof(thirdChar));
        int remainingBits = (int)pow(2, 15);
        total += (thirdChar * remainingBits);
    }
    else
    {
        total += (secondChar * 256);
    }
    return total;
}

int main(int argc, char *argv[])
{
    CompactDawg compactDawg = CompactDawg(argv[1], false);
    compactDawg.writeToFile("compact.txt", false);
    CompactDawg compactDawg2 = CompactDawg("compact.txt", true);
    compactDawg2.writeLexicon();
}