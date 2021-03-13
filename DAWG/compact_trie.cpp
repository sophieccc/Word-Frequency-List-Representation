#include <iostream>
#include <fstream>
#include <iterator>
#include "compact_trie.h"

using namespace std;

// Compacted = Read pre-created array ver. of trie.
// Non-Compacted = Read word-freq list to first create pointer ver. of trie.
CompactTrie::CompactTrie(ifstream &file, bool compacted)
{
    if (compacted)
    {
        // Read in arrays.
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
            branchList.push_back(pair<char, bool>(it->first, last));
            processNode(it->second, last);
        }
        nodes.pop();
    }
}

void CompactTrie::processNode(Node* n, bool last) {
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
            if (branchList[index].first == word[i])
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

int main(int argc, char *argv[])
{

    ifstream file;
    file.open(argv[1], ios_base::in);
    CompactTrie compactTrie = CompactTrie(file, false);
}