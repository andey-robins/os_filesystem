#include "disk.h"
#include "diskmanager.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cmath>
using namespace std;

DiskManager::DiskManager(Disk *d, int partcount, DiskPartition *dp)
{
    myDisk = d;
    partCount = partcount;
    int r = myDisk->initDisk();
    char buffer[64];

    /* If needed, initialize the disk to keep partition information */
    /*
    When the need to create and partition a new disk arises, we start here and create
    the partition table.
    The partition table scheme used in this program is unique, and has a specific ordering.
    We begin with the name, then the partition size, the start block (exlcuding the first,
    as it is the root directory), and the end block. After the name and overal size is written,
    the start and end blocks are separated by the '|' character. Any remaining space is filled
    with the '*' character, instead of garbage. 
  */
    if (r == 1)
    {
        int partitionInProg = 0;
        int bufferPosition = 1;
        diskP = dp;
        buffer[0] = '|';
        while (partitionInProg < partCount)
        {
            //Insert partition name into buffer
            buffer[bufferPosition] = diskP[partitionInProg].partitionName;
            //Begin filling the buffer with partition information such as size, start, and end blocks.
            fillPartitionInfo(buffer, diskP[partitionInProg].partitionSize, bufferPosition + 1, partitionInProg);
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
        myDisk->writeDiskBlock(0, buffer);

        //Reset all critical shared variables.
        buffer[0] = 0;
        bufferIndexer = 1;
        blockCount = 0;
        rootOffset = 1;
    }

    /* else  read back the partition information from the DISK1 */
    else if (r == 0)
    {
        diskP = new DiskPartition[partCount];

        myDisk->readDiskBlock(0, buffer);
        int partitionInProg = 0;

        while (partitionInProg < partCount)
        {
            //Assign the partition name
            diskP[partitionInProg].partitionName = buffer[bufferIndexer];
            //Retrieve and assign the partition size.
            bufferIndexer++;
            int partSize = retrievePartitionInfo(buffer);
            diskP[partitionInProg].partitionSize = partSize;

            //Record starting block
            int startBlock = retrievePartitionInfo(buffer);

            diskP[partitionInProg].startBlock = startBlock;

            //Record ending block
            int endBlock = retrievePartitionInfo(buffer);
            diskP[partitionInProg].endBlock = endBlock;

            partitionInProg++;
        }

        //Reset all critical shared variables.
        buffer[0] = 0;
        bufferIndexer = 1;
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
    //Disk class already returns many of these values, simply check if partition exists,
    //and if the requested block number is within it's range of available blocks.
    for (int i = 0; i < partCount; i++)
    {
        if ((diskP[i].partitionName == partitionname) && (blknum <= diskP[i].endBlock - diskP[i].startBlock))
        {
            int absoluteDiskBlock = diskP[i].startBlock + blknum;
            return myDisk->readDiskBlock(absoluteDiskBlock, blkdata);
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
    //Disk class already returns many of these values, simply check if partition exists,
    //and if the requested block number is within it's range of available blocks.
    for (int i = 0; i < partCount; i++)
    {
        if ((diskP[i].partitionName == partitionname) && (blknum <= diskP[i].endBlock - diskP[i].startBlock))
        {
            int absoluteDiskBlock = diskP[i].startBlock + blknum;
            return myDisk->writeDiskBlock(absoluteDiskBlock, blkdata);
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

    for (int i = 0; i < partCount; i++)
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

void DiskManager::fillPartitionInfo(char *buffer, int num, int pos, int diskPIndex)
{

    //Use a private global variable to keep track of total blocks,
    blockCount += num;

    //Get the partition size ready for buffer insertion
    string sizeString = to_string(num);
    int strSize = sizeString.length();

    for (int i = 0; i < strSize; i++)
    {
        buffer[pos + i] = sizeString[i];
    }

    //Adjust position based on what has just been added
    pos += strSize;

    //Formatting symbol for separating name and size of partition from blocks used
    buffer[pos] = '|';
    pos++;
    /*
    The root offset is used to exclude the 0th block of any partition, as this is reserved for
    the root. The second (pos # 1) block is reserved for the bitvector, but this still needs to be available
    for our partition manager. With this logic in mind, the rootOffset is our 'start block'
    for each partition.
  */
    string startBlock = to_string(rootOffset);

    diskP[diskPIndex].startBlock = atoi(startBlock.c_str());
    string endBlock = to_string(blockCount + endBlockOffset);
    diskP[diskPIndex].endBlock = atoi(endBlock.c_str());

    //Once the first partition has been written, we need to bump up the offset by one, but
    //only once.
    if (rootOffset == 1)
    {
        rootOffset++;
        endBlockOffset++;
    }

    rootOffset += num;

    //Write the start block, and insert the formatting character.
    strSize = startBlock.length();
    for (int i = 0; i < strSize; i++)
    {
        buffer[pos] = startBlock[i];
        pos++;
    }
    buffer[pos] = '|';
    pos++;

    //Write the end block, and insert the formatting character.
    strSize = endBlock.length();
    for (int i = 0; i < strSize; i++)
    {
        buffer[pos] = endBlock[i];
        pos++;
    }
    buffer[pos] = '|';
    pos++;
    //Adjust the overall buffer index
    bufferIndexer = pos;
    return;
}

int DiskManager::retrievePartitionInfo(char *buffer)
{
    char *temp[4];
    for (int i = 0; i < 5; i++)
    {

        //If a formatting character is encountered, adjust the indexer to skip over
        //it, and break out of the loop. Because of the way the table is formated, pertinent
        //information has been recorded.
        if (buffer[bufferIndexer] == '|')
        {
            bufferIndexer++;
            break;
        }

        temp[i] = &buffer[bufferIndexer];
        bufferIndexer++;
    }

    return atoi(*temp);
}