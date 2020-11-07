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
    char name;
    int subPointer;
    char type;
};

FNode createFileNode(char name, int dirAddressOne)
{
    char buffer[64];
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
    }
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
DNode createDirNode(char name){};
DNode loadDirNode(int blknum){};
INode createIndirNode(){};
INode loadIndirNode(int blknum){};

#endif