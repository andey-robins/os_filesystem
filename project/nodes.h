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

FNode loadFileNode(int blknum){};

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

DNode createDirNode(char name, int ptr, char type)
{
    DNode inode;
    inode.entries[0].name = name;
    inode.entries[0].subPointer = ptr;
    inode.entries[0].type = type;
    return inode;
}

DNode loadDirNode(char *nodeBuffer)
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

INode createIndirNode()
{
    INode inode;
    return inode;
}

INode loadIndirNode(char *nodebuffer)
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

char *indirNodeToBuffer(INode n)
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
DNode createDirNode(char name){};
DNode loadDirNode(int blknum){};

INode createIndirNode(){};
INode loadIndirNode(int blknum){};

#endif