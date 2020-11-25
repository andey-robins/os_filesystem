#include "fnode.h"
#include <string.h>
#include <iostream>
using std::cerr;
using std::endl;
FNode FNode::createFileNode(char name)
{
    FNode fNode;
    fNode.name = name;
    fNode.type = 'F';
    fNode.size = 0;
    fNode.directAddress[0] = 0;
    fNode.directAddress[1] = 0;
    fNode.directAddress[2] = 0;
    fNode.indirectAddress = 0;

    for (int i = 0; i < 3; i++)
    {
        fNode.emoji[i] = '0';
    }

    for (int i = 0; i < 3; i++)
    {
        fNode.date[i] = 0;
    }

    return fNode;
}

FNode FNode::loadFileNode(char *buffer)
{
    FNode node;
    //The first char in the buffer is the node name
    node.name = buffer[0];
    //Next is node type
    node.type = buffer[1];
    char tmp[5];
    //Read the next four bytes in for node size
    for (int i = 2; i < 6; i++)
    {
        tmp[i - 2] = buffer[i];
    }
    //Because atoi LOVES to cause random segfaults
    //Gotta tell it where the end is >:(
    tmp[4] = '\0';
    node.size = atoi(tmp);
    //Read in 3 sections of 4 chars which will be the node's direct addresses
    for (int j = 0; j < 3; j++)
    {
        for (int i = 0; i < 4; i++)
        {
            tmp[i] = buffer[6 + i + (4 * j)];
        }
        tmp[4] = '\0';
        node.directAddress[j] = atoi(tmp);
    }
    //Read in next 4 chars which will be the indirect address
    for (int i = 0; i < 4; i++)
    {
        tmp[i] = buffer[i + 18];
    }
    tmp[4] = '\0';
    node.indirectAddress = atoi(tmp);
    //Next 12 bytes is the date
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            tmp[j] = buffer[22 + j + (4 * i)];
        }
        tmp[4] = '\0';
        node.date[i] = atoi(tmp);
    }
    //Final 3 bytes is the emoji
    for (int i = 0; i < 3; i++)
    {
        node.emoji[i] = buffer[34 + i];
    }
    return node;
};

void FNode::fileNodeToBuffer(FNode f, char *inode)
{
    inode[0] = f.name;
    inode[1] = 'F';
    // size
    intToChar(inode, f.size, 2);
    // three direct addresses
    for (int i = 0; i < 3; i++)
    {
        // direct address
        intToChar(inode, f.directAddress[i], 6 + (4 * i));
    }
    //indirect address
    intToChar(inode, f.indirectAddress, 18);
    // date
    for (int i = 0; i < 3; i++)
    {
        intToChar(inode, f.date[i], 22 + (4 * i));
    }
    // emoji
    for (int i = 0; i < 3; i++)
    {
        inode[i + 34] = f.emoji[i];
    }

    // padding
    for (int i = 37; i < 64; i++)
    {
        inode[i] = '0';
    }
}

//Helper method for fileNodeToBuffer to facilitate easy conversion from int to char
void FNode::intToChar(char *buffer, int num, int pos)
{
    char four[5];
    sprintf(four, "%.4d", num);
    for (int i = 0; i < 4; i++)
    {
        buffer[i + pos] = four[i];
    }
    return;
}
