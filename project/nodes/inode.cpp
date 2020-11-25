#include "inode.h"
#include <string.h>
#include <iostream>

INode INode::createIndirNode()
{
    INode inode;
    for (int i = 0; i < 16; i++)
    {
        inode.directPointers[i] = 0;
    }
    return inode;
}

INode INode::loadIndirNode(char *nodebuffer)
{
    INode inode;

    // direct addresses
    for (int i = 0; i < 16; i++)
    {
        char directPointerChars[5];
        for (int j = 0; j < 4; j++)
        {
            directPointerChars[j] = nodebuffer[i * 4 + j];
        }
        directPointerChars[4] = '\0';
        inode.directPointers[i] = atoi(directPointerChars);
    }

    return inode;
}

void INode::indirNodeToBuffer(INode n, char *outBuff)
{
    // direct addresses
    for (int i = 0; i < 16; i++)
    {
        intToChar(outBuff, n.directPointers[i], 4 * i);
    }
}

void INode::intToChar(char *buffer, int num, int pos)
{
    char four[5];
    sprintf(four, "%.4d", num);
    for (int i = 0; i < 4; i++)
    {
        buffer[i + pos] = four[i];
    }
    return;
}