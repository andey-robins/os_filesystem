#include "dnode.h"
#include <string.h>
#include <iostream>

DNode DNode::createDirNode(char name, int ptr, char type)
{
    DNode inode;
    inode.entries[0].name = name;
    inode.entries[0].subPointer = ptr;
    inode.entries[0].type = type;
    return inode;
}

DNode DNode::loadDirNode(char *nodeBuffer)
{
    DNode inode;

    // 10 "Entries"
    for (int i = 0; i < 10; i++)
    {
        inode.entries[i].name = nodeBuffer[i * 6];

        // get each char from the subpointer
        char subpointerChars[4];
        for (int j = 0; j < 4; j++)
        {
            subpointerChars[j] = nodeBuffer[i * 6 + j + 1];
        }
        inode.entries[i].subPointer = atoi(subpointerChars);

        inode.entries[i].type = nodeBuffer[i * 6 + 5];
    }

    // next dir
    char nextPointerChars[4];
    for (int i = 0; i < 4; i++)
    {
        nextPointerChars[i] = nodeBuffer[i + 60];
    }
    inode.nextDirectPointer = atoi(nextPointerChars);

    return inode;
}

char* DNode::dirNodeToBuffer(DNode d)
{
    char dNode[64];
    int bufferIndexer = 1;
    dNode[0] = d.nextDirectPointer;

    //int entriesSize = sizeof(d.entries)/sizeof(d.entries[0]);
    
    for (int i = 0; i < 10; i++)
    {
        FileEntry temp = d.entries[i];
        dNode[bufferIndexer] = temp.name;
        bufferIndexer++;
        const char *subPointerChars = std::to_string(temp.subPointer).c_str();
        int subPointerCharsSize = sizeof(subPointerChars)/sizeof(subPointerChars[0]);
        for (int j = 0; j < subPointerCharsSize; j++)
        {
            dNode[bufferIndexer] = subPointerChars[j];
            bufferIndexer++;
        }
        bufferIndexer++;
        dNode[bufferIndexer] = temp.type;
        bufferIndexer++;
    }
    return dNode;
}