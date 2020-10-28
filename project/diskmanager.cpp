#include "disk.h"
#include "diskmanager.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstring>
using namespace std;

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
    diskP = dp;
    myDisk->writeDiskBlock(0, buffer);
  }
  /* else  read back the partition information from the DISK1 */

  else if (r == 0)
  {
    diskP = new DiskPartition[partCount];
    myDisk->readDiskBlock(0, buffer);
  }

}

/*
 *   returns: 
 *   0, if the block is successfully read;
 *  -1, if disk can't be opened; (same as disk)
 *  -2, if blknum is out of bounds; (same as disk)
 *  -3 if partition doesn't exist
 */
int DiskManager::readDiskBlock(char partitionname, int blknum, char *blkdata)
{
  if ((blknum < 0) || (blknum >= myDisk->blkCount))
  {
    return -2;
  }

  for(int i = 0; i < partCount; i++)
  {
    if (diskP[i].partitionName == partitionname)
    {
      //If the name is found, it exists, and we can break out of the loop
      break;
    }

    else if (diskP[i].partitionName != partitionname)
    {
      //If no partition names match, the last loop check will complete and drop down to 
      //return -3
      continue;
    }

    return -3;    
  }

  ifstream file((myDisk->diskFilename), ios::in);
  if (!file)
  {
    return -1;
  }

  //Is this the right way to read from a specific partition?
  file.seekg(blknum * myDisk->blkSize);
  file.read(blkdata, myDisk->blkSize);
  file.close();
  return 0;
}


/*
 *   returns: 
 *   0, if the block is successfully written;
 *  -1, if disk can't be opened; (same as disk)
 *  -2, if blknum is out of bounds;  (same as disk)
 *  -3 if partition doesn't exist
 */
int DiskManager::writeDiskBlock(char partitionname, int blknum, char *blkdata)
{
  if ((blknum < 0) || (blknum >= myDisk->blkCount))
  {
    return -2;
  }

  for(int i = 0; i < partCount; i++)
  {
    if (diskP[i].partitionName == partitionname)
    {
      //If the name is found, it exists, and we can break out of the loop
      break;
    }

    else if (diskP[i].partitionName != partitionname)
    {
      //If no partition names match, the last loop check will complete and drop down to 
      //return -3
      continue;
    }

    return -3;    
  }

  fstream file((myDisk->diskFilename), ios::in);
  if (!file)
  {
    return -1;
  }

  //Is this the right way to write to a specific partition?
  file.seekg(blknum * myDisk->blkSize);
  file.write(blkdata, myDisk->blkSize);
  file.close();
  return 0; 
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

    else if (diskP[i].partitionName != partitionname)
    {
      //If no partition names match, the last loop check will complete and drop down to 
      //return -1
      continue;
    }
    return -1;    
  }

}
