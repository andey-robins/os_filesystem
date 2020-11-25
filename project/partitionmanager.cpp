#include "disk.h"
#include "diskmanager.h"
#include "partitionmanager.h"
#include "bitvector.h"
#include <iostream>
using namespace std;

PartitionManager::PartitionManager(DiskManager *dm, char partitionname, int partitionsize)
{
    myDM = dm;
    myPartitionName = partitionname;
    myPartitionSize = partitionsize;
    myBitVector = new BitVector(myPartitionSize);
    char buffer[64];

    // read in the section that should contain the bitvector for the partition
    readDiskBlock(0, buffer);

    // case where bitvector has already been set up and can simply be read in from the 0th block
    // of the partition
    if (buffer[0] != '#')
    {
        myBitVector->setBitVector((unsigned int *)buffer);
    }
    // the case where the bitvector hasn't been set up yet
    else
    {
        myBitVector->setBit(0); // set 0th bit to signal that the 0th sector is in use for the bitvector
        myBitVector->setBit(1); // set 1st bit to signal that the 1st sector is in use for the root dir
    }

    // save the bitvector to the disk
    myBitVector->getBitVector((unsigned int *)buffer);
    writeDiskBlock(0, buffer);
}

PartitionManager::~PartitionManager()
{
    delete[] myBitVector;
}

/*
 * return blocknum, -1 otherwise
 */
int PartitionManager::getFreeDiskBlock()
{
    char buffer[64];
    /* write the code for allocating a partition block */
    // this should be able to stat at two since block 0 is partition info and block 1 is the root dir?
    for (int i = 0; i < myPartitionSize; i++)
    {
        if (myBitVector->testBit(i) == OFF)
        {
            myBitVector->setBit(i);
            myBitVector->getBitVector((unsigned int *)buffer);
            writeDiskBlock(0, buffer);
            return i;
        }
    }

    return -1;
}

/*
 * return 0 for sucess, -1 otherwise
 */
int PartitionManager::returnDiskBlock(int blknum)
{
    /* write the code for deallocating a partition block */
    // reset block's bitvector to free it
    myBitVector->resetBit(blknum);

    // overwrite the deallocated block with cs
    char overwriteBuffer[64];
    for (int i = 0; i < 64; i++)
    {
        overwriteBuffer[i] = 'c';
    }

    int success = this->writeDiskBlock(blknum, overwriteBuffer);
    //Write changes back to disk because if not the bitvector gets funky
    char buff1[64];
    myBitVector->getBitVector((unsigned int *)buff1);
    writeDiskBlock(0, buff1);

    if (success == 0)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

int PartitionManager::readDiskBlock(int blknum, char *blkdata)
{
    return myDM->readDiskBlock(myPartitionName, blknum, blkdata);
}

int PartitionManager::writeDiskBlock(int blknum, char *blkdata)
{
    return myDM->writeDiskBlock(myPartitionName, blknum, blkdata);
}

int PartitionManager::getBlockSize()
{
    return myDM->getBlockSize();
}
