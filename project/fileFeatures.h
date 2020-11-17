#ifndef FILEFEATURES_H
#define FILEFEATURES_H

#include <deque>
#include <cstdlib>
#include <string>
#include <iostream>
#include <bits/stdc++.h>

using namespace std;

class LockedFiles
{
public:
    char *fileName;
    bool isLocked;
    int fileNameLength;
    int lockId;
};

class OpenFiles
{
public:
    char *fileName;
    int fileNameLength;
    int fileDescription;
    int readWritePointer;
    char mode;
    int lockId;
};

class FileDescriptor
{
private:
    int keyValues[100];
    int keyIndexer = 0;

public:
    int uniqueNumberResult;
    //    void initShuffle(void);
    //    int getUniqueNumber(void);

    void initShuffle(void)
    {
        //Don't want any keys to be equal to zero
        for (int i = 1; i < 101; i++)
        {
            keyValues[i - 1] = i;
        }
        int sizeOfArr = sizeof(keyValues) / sizeof(keyValues[0]);
        shuffle(keyValues, keyValues + sizeOfArr, default_random_engine(1));
    }

    int getUniqueNumber(void)
    {
        uniqueNumberResult = keyValues[keyIndexer];
        keyIndexer++;
        return uniqueNumberResult;
    }
};

#endif
