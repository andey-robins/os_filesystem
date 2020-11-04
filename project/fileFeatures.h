#ifndef FILEFEATURES_H
#define FILEFEATURES_H

#include <deque>
#include<cstdlib>
#include<string>
#include<iostream>
#include <bits/stdc++.h> 

using namespace std;

class LockedFiles 
{
    public:
        char fileName;
        bool isLocked;
        int fileNameLength;
        int lockId;
    
    /*public:
        void addToDeque(char fName, int fNameLen, int lId);
        void removeFromDeque(char fName, int fNameLen, int lId);


    LockedFiles(void);
    ~LockedFiles(void);

    LockedFiles::LockedFiles(void)
    {
        //For now, 1 deque should be enough.
        lockedQueue = new deque<LockedFiles>[1];

        if (lockedQueue == NULL)
        {
            cout << "Not enough memory to create the queue" << endl;
            exit(-1);
        }
    }

    LockedFiles::~LockedFiles(void)
    {
        delete[] lockedQueue;
    }

    void LockedFiles::addToDeque(char fName, int fNameLen, int lId)
    {

        fileName = fName;
        fileName = fNameLen;
        lockId = lId;
        isLocked = true;
        lock
    }*/
};

class OpenFiles
{
    public:
        int fileDescription;
};


class FileDescriptor
{
    private:
        int keyValues[100];
        int keyIndexer = 0;
    
    public:
        int uniqueNumberResult;
        void initShuffle(void);
        int getUniqueNumber(void);

    void FileDescriptor::initShuffle(void)
    {
        //Don't want any keys to be equal to zero
        for (int i = 1; i < 100; i++)
        {
            keyValues[i-1] = i;
        }
        int sizeOfArr = sizeof(keyValues) / sizeof(keyValues[0]);
        shuffle(keyValues, keyValues+sizeOfArr, default_random_engine(0));
    }

    int FileDescriptor::getUniqueNumber(void)
    {
        uniqueNumberResult = keyValues[keyIndexer];
        keyIndexer++;
        return uniqueNumberResult;
    }
};


//Do we need this?
class INode
{

};

#endif