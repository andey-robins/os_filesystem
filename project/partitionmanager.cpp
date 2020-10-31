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
  myPartitionSize = myDM->getPartitionSize(myPartitionName);
  myBitVector = new BitVector(myPartitionSize);
  char temp[64];
  char buffer[64];

  /* If needed, initialize bit vector to keep track of free and allocted
     blocks in this partition */
  readDiskBlock(0, buffer);

  if (buffer[0] != '#')
  {
    myBitVector->setBitVector((unsigned int *) buffer);
  }

  else if (buffer[0] == '#')
  {
    myBitVector->setBitVector((unsigned int *) temp);
  }

  //Bit probably needs to be set here for first block of each partition, 
  //root is automatically not available. 
  //setbit?
  myBitVector->getBitVector((unsigned int *) temp);
  writeDiskBlock(0, temp); 
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
  /* write the code for allocating a partition block */
}

/*
 * return 0 for sucess, -1 otherwise
 */
int PartitionManager::returnDiskBlock(int blknum)
{
  /* write the code for deallocating a partition block */
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
