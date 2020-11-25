#ifndef FNODE_H
#define FNODE_H

class FNode
{
public:
    // data structure
    char name;
    char type;
    int size;
    int directAddress[3];
    int indirectAddress;
    char emoji[3];
    int date[3];

    // functions for interacting with the structure
    static FNode createFileNode(char name);
    static FNode loadFileNode(char *buffer);
    static void fileNodeToBuffer(FNode f, char *inode);
    static void intToChar(char *buffer, int num, int pos);
};

#endif
