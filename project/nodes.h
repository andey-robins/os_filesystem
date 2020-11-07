#include <string.h>
#include <iostream>

#ifndef NODES_H
#define NODES_H


class FNode
{
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
    int directPointers[16];
};

class DNode
{
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
    const char* dirAddrChars = std::to_string(dirAddressOne).c_str();

    for (int i = 6; i < 10; i++){
        buffer[i] = dirAddrChars[i];
    }

    for (int i = 10; i < 64; i++)
    {
        buffer[i] = '0';
    }
}

FNode loadFileNode(int blknum){};
DNode createDirNode(char name){};
DNode loadDirNode(int blknum){};
INode createIndirNode(){};
INode loadIndirNode(int blknum){};


#endif