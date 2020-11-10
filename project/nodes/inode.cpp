#include "inode.h"
#include <string.h>
#include <iostream>

INode INode::createIndirNode()
{
    INode inode;
    return inode;
}

INode INode::loadIndirNode(char *nodebuffer)
{
    INode inode;

    // direct addresses
    for (int i = 0; i < 16; i++)
    {
        char directPointerChars[4];
        for (int j = 0; j < 4; j++)
        {
            directPointerChars[j] = nodebuffer[i * 4 + j];
        }
        inode.directPointers[i] = atoi(directPointerChars);
    }

    return inode;
}

char* INode::indirNodeToBuffer(INode n)
{
    char inode[64];

    // direct addresses
    for (int i = 0; i < 16; i++)
    {
        // convert addresses to characters
        const char *dirAddrChars = std::to_string(n.directPointers[i]).c_str();
        for (int j = 0; j < 4; j++)
        {
            inode[i * 4 + j] = dirAddrChars[j];
        }
    }

    return inode;
}