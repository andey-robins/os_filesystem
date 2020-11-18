#ifndef DNODE_H
#define DNODE_H

// supporting struct that is a part of the DNode
class FileEntry
{
public:
    char name;
    int subPointer;
    char type;
};

class DNode
{
public:
    // data struct
    int nextDirectPointer;
    FileEntry entries[10];

    // functions for interaction with the struct
    static DNode createDirNode(char name, int ptr, char type);
    static DNode loadDirNode(char *nodeBuffer);
    static void dirNodeToBuffer(DNode d, char *outBuff);
    static void intToChar(char *buffer, int num, int pos);
};

#endif