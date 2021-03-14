#include <iostream>
#include <fstream>
#include <iterator>
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
    std::ofstream outfile;
    outfile.open(fileName, ios::out | ios::binary);
    for (auto it = branchList.begin(); it != branchList.end(); ++it)
    {
        unsigned char letter = it->first.first;
        unsigned int frequency = it->first.second;
        bool onlyDaughter = it->second;
        outfile.write((char *)(&letter), sizeof(letter));
        outfile.write((char *)(&frequency), sizeof(frequency));
        outfile.write((char *)(&onlyDaughter), sizeof(onlyDaughter));
    }
    outfile << "\n";
    for (auto it = nodeList.begin(); it != nodeList.end(); ++it)
    {
        bool terminal = it->second;
        unsigned int index = it->first;
        outfile.write((char *)(&terminal), sizeof(terminal));
        outfile.write((char *)(&index), sizeof(index));
    }
}

void CompactTrie::readFromFile(string fileName)
{
    std::ifstream infile;
    infile.open(fileName, ios::in | ios::binary);
    unsigned char curr;
    infile.read((char *)(&curr), sizeof(curr));
    while (curr != '\n')
    {
        unsigned char letter;
        unsigned int frequency = 0;
        bool onlyDaughter;
        letter = curr;
        infile.read(reinterpret_cast<char *>(&curr), sizeof(int));
        frequency += curr;
        infile.read(reinterpret_cast<char *>(&curr), sizeof(bool));
        onlyDaughter = curr;
        cout << letter << " " << frequency << " " << onlyDaughter << endl;
        infile.read((char *)(&curr), sizeof(curr));
    }
    cout << endl;
    while (infile.read(reinterpret_cast<char *>(&curr), sizeof(bool)))
    {
        bool terminal = curr;
        unsigned int index = 0;
        infile.read(reinterpret_cast<char *>(&curr), sizeof(int));
        index += curr;
        cout << terminal << " " << index << endl;
    }
}

int main(int argc, char *argv[])
{
    CompactTrie compactTrie = CompactTrie(argv[1], false);
    compactTrie.writeToFile("compact.txt");
    compactTrie.readFromFile("compact.txt");
}