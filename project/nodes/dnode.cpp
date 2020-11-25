#include "dnode.h"
#include <string.h>
#include <iostream>

DNode DNode::createDirNode(char name, int ptr, char type)
{
    DNode inode;
    inode.entries[0].name = name;
    inode.entries[0].subPointer = ptr;
    inode.entries[0].type = type;
    //Fill the rest of the inode with nothing for now
    for (int i = 1; i < 10; i++)
    {
        inode.entries[i].name = '0';
        inode.entries[i].subPointer = 0;
        inode.entries[i].type = '0';
    }
    inode.nextDirectPointer = 0;
    return inode;
}

DNode DNode::loadDirNode(char *nodeBuffer)
{
    DNode inode;
    char temp[5];
    // 10 "Entries"
    for (int i = 0; i < 10; i++)
    {
        inode.entries[i].name = nodeBuffer[i * 6];

        // get each char from the subpointer
        //char * subpointerChars[4];
        for (int j = 0; j < 4; j++)
        {
            temp[j] = nodeBuffer[i * 6 + j + 1];
        }
        temp[4] = '\0';
        inode.entries[i].subPointer = atoi(temp);

        inode.entries[i].type = nodeBuffer[i * 6 + 5];
    }

    // next dir
    //char * nextPointerChars[4];
    for (int i = 0; i < 4; i++)
    {
        temp[i] = nodeBuffer[i + 60];
    }
    temp[4] = '\0';
    inode.nextDirectPointer = atoi(temp);

    return inode;
}

void DNode::dirNodeToBuffer(DNode d, char *outBuff)
{
    for (int i = 0; i < 10; i++)
    {
        FileEntry temp = d.entries[i];
        outBuff[6 * i] = temp.name;
        intToChar(outBuff, temp.subPointer, (6 * i) + 1);
        outBuff[(6 * i) + 5] = temp.type;
    }
    intToChar(outBuff, d.nextDirectPointer, 60);
    return;
}

void DNode::intToChar(char *buffer, int num, int pos)
{
    char four[5];
    sprintf(four, "%.4d", num);
    for (int i = 0; i < 4; i++)
    {
        buffer[i + pos] = four[i];
    }
    return;
}