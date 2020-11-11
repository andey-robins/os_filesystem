//WM
//Testing Functionality of FNode functions

#include "nodes.h"
#include "fnode.h"
#include <iostream>
#include <iomanip>
#include <cstring>
using namespace std;

void printNode (FNode f);
void intToChar(char * buffer, int num, int pos);
void printbuffer(char * buffer, int size);

int main() 
{
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
	char exBuff[64];
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
	
	char myBuff[64];
	FNode myFNode;

	//Begin testing the default file iNode
	myFNode = FNode::createFileNode('A', 10); 
	cout << endl << "Here is the file iNode created by createFileNode('A', 10):" << endl;
	printNode(myFNode);
	//NOTE: This test seems to pass as expected.

	//Begin testing loadFileNode
	myFNode = FNode::loadFileNode(exBuff);
	cout << "Here is the file iNode created with a buffer: " << endl;
	printNode(myFNode);
	//NOTE: This test seems to pass as expected.

	//Compare output of fileNodeToBuffer to that of the original buffer
	FNode::fileNodeToBuffer(myFNode, myBuff);
	cout << "Here is the buffer created from the above file iNode: " << endl;
	printbuffer(myBuff, 64);
	//NOTE: This test seems to pass as expected.

}

void printNode (FNode f)
{
	cout << "----------Information for the FNode named: " << f.name << "----------" << endl
	<< setw(50) << left << "File is of type: " << f.type << endl
	<< setw(50) << "File has size: " << f.size << endl
	<< setw(50) << "File has the following direct addresses: " << "[" << f.directAddress[0]
	<< ", " << f.directAddress[1] << ", " << f.directAddress[2] << "]" << endl
	<< setw(50) << "File has the following indirect address: " << f.indirectAddress << endl
	<< setw(50) << "File has emoji attribute: " << f.emoji << endl
	<< setw(50) << "File has date attribute (d/m/y): " << f.date[0] << "/" << f.date[1]
	<< "/" << f.date[2] << endl
	<< "--------------------------------------------------------" << endl << endl;
}

void intToChar(char * buffer, int num, int pos) {
    char four[4];
    sprintf( four, "%.4d", num);
    for (int i = 0; i < 4; i++) {
        buffer[i + pos] = four[i];
    }
    return;
}

void printbuffer(char * buffer, int size) {
    for (int i = 0; i < size; i++) {
        cout << buffer[i];
        if (i % 4 == 3) {
            cout << " ";
        }
        if (i % 16 == 15) {
            cout << endl;
        }
    }
}
