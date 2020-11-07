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

DNode createDirNode(char name){};
DNode loadDirNode(int blknum){};
INode createIndirNode(){};
INode loadIndirNode(int blknum){};


#endif
