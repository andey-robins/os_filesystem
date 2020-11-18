#ifndef INODE_H
#define INODE_H

class INode
{
public:
    // data structure
    int directPointers[16];

    // functions to interact with the struct
    static INode createIndirNode();
    static INode loadIndirNode(char *nodebuffer);
    static void indirNodeToBuffer(INode n, char *outBuff);
    static void intToChar(char *buffer, int num, int pos);
};

#endif