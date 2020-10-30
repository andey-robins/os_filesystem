#include "disk.h"
#include "diskmanager.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cmath>
using namespace std;

int bufferIndexer = 0;
int blockCount = 1;

DiskManager::DiskManager(Disk *d, int partcount, DiskPartition *dp)
{
  myDisk = d;
  partCount= partcount;
  int r = myDisk->initDisk();
  char buffer[64];

  /* If needed, initialize the disk to keep partition information */
  /*
    Stefan Emmons: Based on the return value of the initDisk function, we need to make
    a decision based on wether a disk has already been created (return val 0),
    or one has been created, and we need to create a new partition(s) for it (return val 1).
    
    This is almost certainly not complete, I think we need to establish some
    partition logic here, as this is where partitions should be created.
  */
  if (r == 1)
  {
    int partitionInProg = 0;
    int bufferPosition = 0;
    diskP = dp;

    while(partitionInProg < partCount)
    {
      //Insert partition name into buffer
      buffer[bufferPosition] = diskP[partitionInProg].partitionName;
      //Begin filling the buffer with partition information such as size, and blocks consumed
      fillPartition(buffer, diskP[partitionInProg].partitionSize, bufferPosition + 1);
      //Use the global indexer to adjust buffer position
      bufferPosition = bufferIndexer; 
      partitionInProg++;
    }

    for (int i = bufferPosition; i < 64; i++)
    {
      //Take up the rest of the free block space with a formatted character instead of garbage.
      buffer[i] = '*';
    }
    //Write to disk with all partition information ready to go
    myDisk -> writeDiskBlock(0, buffer);
    
    //Reset all critical shared variables.
    buffer[64];
    bufferIndexer = 0;
    blockCount = 1;
  }

  /* else  read back the partition information from the DISK1 */
  else if (r == 0)
  {
    diskP = new DiskPartition[partCount];
    myDisk->readDiskBlock(0, buffer);
    int partitionInProg = 0;
    int bufferPosition = 0;

    while(partitionInProg < partCount)
    {

      //Assign the partition name
      diskP[partitionInProg].partitionName = buffer[bufferPosition];

      //Retrieve and assign the partition size.
      int partSize = retrievePartition(buffer, bufferPosition + 1);
      diskP[partitionInProg].partitionSize = partSize;

      //We can assess how the buffer indexing needs to be adjusted based on how many
      //blocks there are in the disk for this partition
      int blockCount = ceil(partSize/64.0);

      //Adjust the buffer postion based on the calculated block count, the characters read this
      //round, and the formatting character. Multiply by four because this is how the 
      //blocks are formatted
      bufferPosition += ((blockCount*4) + bufferIndexer + 1);
      
      //This variable will need to be re-adjusted each time because the partion size and 
      //formatting variable jump will rarely be the same
      bufferIndexer = 0;
      partitionInProg++;
    }

    //Reset all critical shared variables.
    buffer[64];
    bufferIndexer = 0;
  }
}

/*
 *   returns: 
 *   0, if the block is successfully read;
 *  -1, if disk can't be opened; (same as disk)
 *  -2, if blknum is out of bounds; (same as disk)
 *  -3 if partition doesn't exist
 */
//Still iffy on this
int DiskManager::readDiskBlock(char partitionname, int blknum, char *blkdata)
{
  //Disk class already returns many of these values, simply check if partition exists.
  for(int i = 0; i < partCount; i++)
  {
    if (diskP[i].partitionName == partitionname)
    {
      myDisk->readDiskBlock(blknum, blkdata);
      return 0;
    }
  }
  return -3;
}


/*
 *   returns: 
 *   0, if the block is successfully written;
 *  -1, if disk can't be opened; (same as disk)
 *  -2, if blknum is out of bounds;  (same as disk)
 *  -3 if partition doesn't exist
 */
//Still iffy on this
int DiskManager::writeDiskBlock(char partitionname, int blknum, char *blkdata)
{
  //Disk class already returns many of these values, simply check if partition exists.
  for(int i = 0; i < partCount; i++)
  {
    if (diskP[i].partitionName == partitionname)
    {
      myDisk->writeDiskBlock(blknum, blkdata);
      return 0;
    }
  }
  return -3;
}

/*
 * return size of partition
 * -1 if partition doesn't exist.
 */
int DiskManager::getPartitionSize(char partitionname)
{

  for(int i = 0; i < partCount; i++)
  {
    if (diskP[i].partitionName == partitionname)
    {
      //If the name is found, it exists, and we can break out of the loop
      //with the return value.
      return diskP[i].partitionSize;
    }   
  }
  return -1;
}

void DiskManager::fillPartition(char * buffer, int num, int pos)
{

  //Keep track of how many blocks this partition will need.
  int blockNum = ceil(num/64.0);

  //Get the partition size ready for buffer insertion
  string sizeString = to_string(num);

  for(int i = 0; i < sizeString.length(); i++)
  {
    buffer[pos+i] = sizeString[i];
  }
  
  //Adjust position based on what has just been added
  pos += sizeString.length();

  //Formatting symbol for separating name and size of partition from blocks used 
  buffer[pos] = '|';
  pos++;
  
  for (int i = 0; i < blockNum; i++)
  {
    
    //Get the block number ready for insertion, using the global block count variable
    //that keeps track how many overall blocks we have.
    string intString = to_string(blockCount);
    int stringLengthRemainder = 4 - intString.length();
    
    //Format the block number so that it is easy to identify it
    if (stringLengthRemainder != 0)
    {
        intString.insert(0, stringLengthRemainder, '0');
    }
    
    //Insert into the buffer
    for (int j = 0; j < 4; j++)
    {
        buffer[pos+j] = intString[j];
    }

    //Adjust position
    pos+=4;
    blockCount++;
  }
  
  //Add another formatting character, and adjust the position again.
  //Be sure to update the global indexer with this information.
  buffer[pos] = '|';
  pos++;
  bufferIndexer = pos;
  return;
}

int DiskManager::retrievePartition(char * buffer, int pos)
{
  char temp[4];
  for (int i = 0; i < 4; i++)
  {
    
    //If a formatting character is encountered, adjust the indexer by to two to skip over
    //it, and break out of the loop, the size has been recored.
    if (buffer[pos+i] == '|')
    {
      bufferIndexer+=2;
      break;
    }

    temp[i] = buffer[pos+i];
    bufferIndexer++;
  }

  return atoi(temp);
}



