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
    static FNode createFileNode(char name, int dirAddressOne);
    static FNode loadFileNode(char* buffer);
    static char* fileNodeToBuffer(FNode f);
};

#endif