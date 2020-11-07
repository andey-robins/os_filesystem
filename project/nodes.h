#include <string.h>
#include <iostream>

#ifndef NODES_H
#define NODES_H

class FNode
{
public:
    char name;
    char type;
    int size;
    int directAddress[3];
    int indirectAddress;
    char emoji[3];
    int date[3];
};

class INode
{
public:
    int directPointers[16];
};

class DNode
{
public:
    int nextDirectPointer;
    FileEntry entries[10];
};

class FileEntry
{
    public:
        char name;
        int subPointer;
        char type;
};


FNode createFileNode(char name, int dirAddressOne)
{
    /*char buffer[64];
    buffer[0] = name;
    buffer[1] = 'F';
    for (int i = 2; i < 6; i++)
    {
        buffer[i] = '0';
    }
    const char *dirAddrChars = std::to_string(dirAddressOne).c_str();

    for (int i = 6; i < 10; i++)
    {
        buffer[i] = dirAddrChars[i];
    }

    for (int i = 10; i < 64; i++)
    {
        buffer[i] = '0';
    }*/

    FNode fNode;
    fNode.name = name;
    fNode.type = 'F';
    fNode.size = 0;  
    fNode.directAddress[0] = dirAddressOne;
    fNode.indirectAddress = 0;
    
    for (int i = 0; i < 3; i++)
    {
        fNode.emoji[i] = '0';
    }

    for (int i = 0; i < 3; i++)
    {
        fNode.date[i] = '0';
    }

    return fNode;
}

FNode loadFileNode(char* buffer)
{
    FNode node = FNode();
    //The first char in the buffer is the node name
    node.name = buffer[0];
    //Next is node type
    node.type = buffer[1];
    char tmp [4];
    //Read the next four bytes in for node size
    for (int i = 2; i < 6; i++)
    {
      tmp[i-2] = buffer[i];
    }
    node.size = atoi(tmp);
    //Read in 3 sections of 4 chars which will be the node's direct addresses
    for (int j = 0; j < 3; j++)
    {
      for (int i = 0; i < 4; i++)
      {
        tmp[i] = buffer[7 + i + (4 * j)];
      }
      node.directAddress[j] = atoi(tmp);
    }
    //Read in next 4 chars which will be the indirect address
    for (int i = 18; i < 22; i++)
    {
      tmp[i-19] = buffer[i];
    }
    node.indirectAddress = atoi(tmp);
    //Next 3 bytes is the date
    for (int i = 0; i < 3; i++)
    {
      node.date[i] = (int) buffer[22 + i];
    }
    //Final 3 bytes is the emoji
    for (int i = 0; i < 3; i++)
    {
      node.emoji[i] = (int) buffer[25 + i];
    }
    return node;
};

char *fileNodeToBuffer(FNode f)
{
    char inode[64];
    inode[0] = f.name;
    inode[1] = 'F';

    // size
    const char *sizeChars = std::to_string(f.size).c_str();
    for (int i = 0; i < 4; i++)
    {
        inode[i + 2] = sizeChars[i];
    }

    // three direct addresses
    for (int i = 0; i < 3; i++)
    {
        // direct address
        const char *dirAddrChars = std::to_string(f.directAddress[i]).c_str();
        for (int j = 0; j < 4; j++)
        {
            inode[j + 6] = dirAddrChars[j];
        }
    }

    // date
    for (int i = 0; i < 3; i++)
    {
        const char *dateByte = std::to_string(f.date[i]).c_str();
        inode[i + 10] = dateByte[0];
    }

    // emoji
    for (int i = 0; i < 3; i++)
    {
        inode[i + 13] = f.emoji[i];
    }

    // padding
    for (int i = 16; i < 64; i++)
    {
        inode[i] = '0';
    }

    return inode;
}


char *dirNodeToBuffer(DNode d)
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

#endif
