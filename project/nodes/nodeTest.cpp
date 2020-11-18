//WM
//Testing Functionality of FNode functions

#include "nodes.h"
#include "fnode.h"
#include "inode.h"
#include "dnode.h"
#include <iostream>
#include <iomanip>
#include <cstring>
using namespace std;

void printNode(FNode f);
void printNode(DNode d);
void printNode(INode i);
void intToChar(char *buffer, int num, int pos);
void printbuffer(char *buffer, int size);

int main()
{
    //Declarations for FNode, DNode, and INode objects as well as buffers
    char myBuff[64];
    char exBuff[64];
    FNode myFNode;
    DNode myDNode;
    INode myINode;

    cout << endl
         << "First we will run tests on the file iNode functions..." << endl;
    //Begin testing the default file iNode
    myFNode = FNode::createFileNode('A', 10);
    cout << endl
         << "Here is the file iNode created by createFileNode('A', 10):" << endl;
    printNode(myFNode);
    //NOTE: This test seems to pass as expected.

    cout << "Next we create a custom buffer to test the loadFileNode function:" << endl;
    /*
	Create a buffer that would correspond to a file inode with the following fields:
	Name: X
	Type: F
	Size: 128
	Direct Addresses: 10, 15, 21
	Indirect Addresses: 13
	Emoji: >:)
	Date: 13/11/2020
	*/
    exBuff[0] = 'X';
    exBuff[1] = 'F';
    intToChar(exBuff, 128, 2);
    intToChar(exBuff, 10, 6);
    intToChar(exBuff, 15, 10);
    intToChar(exBuff, 21, 14);
    intToChar(exBuff, 13, 18);
    intToChar(exBuff, 13, 22);
    intToChar(exBuff, 11, 26);
    intToChar(exBuff, 2020, 30);
    exBuff[34] = '>';
    exBuff[35] = ':';
    exBuff[36] = ')';
    for (int i = 37; i < 64; i++)
    {
        exBuff[i] = '#';
    }
    printbuffer(exBuff, 64);

    //Begin testing loadFileNode
    myFNode = FNode::loadFileNode(exBuff);
    cout << "Here is the file iNode created with a buffer: " << endl;
    printNode(myFNode);
    //NOTE: This test seems to pass as expected.

    //Compare output of fileNodeToBuffer to that of the original buffer
    FNode::fileNodeToBuffer(myFNode, myBuff);
    cout << "Now we pass the above file object to fileNodeToBuffer and expect to get back" << endl
         << "the original buffer that we created. Here it is: " << endl;
    printbuffer(myBuff, 64);
    //NOTE: This test seems to pass as expected.

    //---------------------------------------BEGIN TESTING DNODE---------------------------------------------
    cout << endl
         << endl
         << endl
         << endl
         << "Now we are ready to test the functionality of the directory node: " << endl;
    //Begin by testing the default directory iNode
    myDNode = DNode::createDirNode('A', 5, 'F');
    cout << endl
         << "Here is the directory iNode created by createDirNode('A', 5, 'F'): " << endl;
    printNode(myDNode);
    //NOTE: This test seems to pass as expected.

    cout << "Next we create a custom buffer to test the loadDirNode function:" << endl;
    /*
	Create a buffer that would correspond to a directory inode with the following fields:
	File Entries as follows by (name, pointer, type)
	1: ('b', 3, 'F')
	2: ('c', 4, 'F')
	3: ('d', 5, 'F')
	4: ('e', 6, 'F')
	5: ('f', 7, 'F')
	6: ('G', 8, 'D')
	7: ('H', 9, 'D')
	8: ('I', 10, 'D')
	9: ('J', 11, 'D')
	10: ('x', 35, 'F')
	Block for Directory overflow: 2
	*/
    char dirBuff[] = {'b', '0', '0', '0', '0', 'F',
                      'c', '0', '0', '0', '0', 'F',
                      'd', '0', '0', '0', '0', 'F',
                      'e', '0', '0', '0', '0', 'F',
                      'f', '0', '0', '0', '0', 'F',
                      'G', '0', '0', '0', '0', 'D',
                      'H', '0', '0', '0', '0', 'D',
                      'I', '0', '0', '0', '0', 'D',
                      'J', '0', '0', '0', '0', 'D',
                      'x', '0', '0', '0', '0', 'F'};

    for (int i = 0; i < 9; i++)
    {
        intToChar(dirBuff, (i + 3), 1 + (6 * i));
    }
    intToChar(dirBuff, 35, 55);
    intToChar(dirBuff, 2, 60);
    printbuffer(dirBuff, 64);

    //Begin testing loadDirNode
    myDNode = DNode::loadDirNode(dirBuff);
    cout << "Here is the directory iNode created with a buffer: " << endl;
    printNode(myDNode);
    //This test seems to pass as expected.

    //Compare output of dirNodeToBuffer to that of the original buffer
    DNode::dirNodeToBuffer(myDNode, myBuff);
    cout << "Now we pass the above directory object to dirNodeToBuffer and expect to get back" << endl
         << "the original buffer that we created. Here it is: " << endl;
    printbuffer(myBuff, 64);
    //NOTE: This test seems to pass as expected.

    //---------------------------------------BEGIN TESTING INODE---------------------------------------------
    cout << endl
         << endl
         << endl
         << endl
         << "Now we are ready to test the functionality of the indirect node: " << endl;
    //Begin by testing the default indirect iNode
    myINode = INode::createIndirNode();
    cout << endl
         << "Here is the indirect iNode created by createIndirNode(): " << endl;
    printNode(myINode);
    //NOTE: This test seems to pass as expected.

    cout << "Next we create a custom buffer to test the loadIndirNode function:" << endl;
    //Our buffer simply contains 16 direct addresses
    //Which will be 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
    for (int i = 0; i < 16; i++)
    {
        intToChar(exBuff, i + 1, 4 * i);
    }
    printbuffer(exBuff, 64);

    //Begin testing loadIndirNode
    myINode = INode::loadIndirNode(exBuff);
    cout << "Here is the indirect iNode created with a buffer: " << endl;
    printNode(myINode);
    //This test seems to pass as expected.

    //Compare output of dirNodeToBuffer to that of the original buffer
    INode::indirNodeToBuffer(myINode, myBuff);
    cout << "Now we pass the above indirect inode to indirNodeToBuffer and expect to get back" << endl
         << "the original buffer that we created. Here it is: " << endl;
    printbuffer(myBuff, 64);
    //NOTE: This test seems to pass as expected.

    return 0;
}

void printNode(FNode f)
{
    cout << "----------Information for the FNode named: " << f.name << "----------" << endl
         << setw(50) << left << "File is of type: " << f.type << endl
         << setw(50) << "File has size: " << f.size << endl
         << setw(50) << "File has the following direct addresses: "
         << "[" << f.directAddress[0]
         << ", " << f.directAddress[1] << ", " << f.directAddress[2] << "]" << endl
         << setw(50) << "File has the following indirect address: " << f.indirectAddress << endl
         << setw(50) << "File has emoji attribute: " << f.emoji << endl
         << setw(50) << "File has date attribute (d/m/y): " << f.date[0] << "/" << f.date[1]
         << "/" << f.date[2] << endl
         << "--------------------------------------------------------" << endl
         << endl;
}

void printNode(DNode d)
{
    cout << "----------Information for Directory iNode-----------------------" << endl
         << "When this directory fills, it will overflow into block: " << d.nextDirectPointer << endl;
    for (int i = 0; i < 10; i++)
    {
        cout << "FILE ENTRY NUMBER " << i << " IN THE DIRECTORY: " << endl
             << left << setw(20) << "Name: " << d.entries[i].name << endl
             << setw(20) << "Type: " << d.entries[i].type << endl
             << setw(20) << "With inode held in block: " << d.entries[i].subPointer << endl;
    }
    cout << "-----------------------------------------------------------------" << endl
         << endl;
}

void printNode(INode i)
{
    cout << "----------Information for Indirect iNode-------------------------" << endl
         << "These are the direct pointers held by the inode: " << endl;
    cout << "[" << i.directPointers[0] << ", ";
    for (int j = 1; j < 15; j++)
    {
        cout << i.directPointers[j] << ", ";
    }
    cout << i.directPointers[15] << "]" << endl;
    cout << "-----------------------------------------------------------------"
         << endl
         << endl;
}

void intToChar(char *buffer, int num, int pos)
{
    char four[5];
    sprintf(four, "%.4d", num);
    for (int i = 0; i < 4; i++)
    {
        buffer[i + pos] = four[i];
    }
    return;
}

void printbuffer(char *buffer, int size)
{
    for (int i = 0; i < size; i++)
    {
        cout << buffer[i];
        if (i % 4 == 3)
        {
            cout << " ";
        }
        if (i % 16 == 15)
        {
            cout << endl;
        }
    }
    cout << endl
         << endl;
}
