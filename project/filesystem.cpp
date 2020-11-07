#include "disk.h"
#include "diskmanager.h"
#include "partitionmanager.h"
#include "filesystem.h"
#include "fileFeatures.h"
#include <time.h>
#include <cstdlib>
#include <iostream>
#include <deque>
#include <typeinfo>
#include <cmath>
using namespace std;


FileSystem::FileSystem(DiskManager *dm, char fileSystemName)
{
  myDM = dm;
  myfileSystemName = fileSystemName;
  myfileSystemSize = myDM->getPartitionSize(fileSystemName);
  myPM = new PartitionManager(myDM, myfileSystemName, myfileSystemSize);
  

  //Should only need one deque for each file system. Need to get them allocated here.
  lockedFileQueue = new deque<DerivedLockedFile>[1];
  openFileQueue = new deque<DerivedOpenFile>[1];
  // Do Need something for the indirect inode(s)?

}
int FileSystem::createFile(char *filename, int fnameLen)
{
  // validate filename
  for (int i = 0; i < fnameLen; i++) {
    if (i % 2 == 0) {
      // should be /
      if (filename[i] != '/') {
        return -3;
      }
    } else {
      // should be alpha char
      if (!isalpha(filename[i])) {
        return -3;
      }
    }
  }

  // file exists: return -1
  if (!false) {
    return -1;
  }

  // allocate the file blocks
  int nodeBlock = myPM->getFreeDiskBlock();
  int dataBlock = myPM->getFreeDiskBlock();

  // check that there is space for the file
  if (nodeBlock == -1 || dataBlock == -1) {
    // if the node was correctly allocated, free the block since we can't create the file
    if (nodeBlock != -1) {
      myPM->returnDiskBlock(nodeBlock);
    }

    // i believe this should be redundant, but to be safe, also check the dataBlock in case it was allocated incorrectly -andey
    if (nodeBlock != -1) {
      myPM->returnDiskBlock(dataBlock);
    }

    return  -2;
  }

  // create file iNode
  char fileInode[64];
  fileInode[0] = filename[fnameLen - 1]; // set file name
  fileInode[1] = 'F'; // set type as file
  // set fileInode.size
  // set fileInode.directAddr.1 to dataBlock
  // set fileInode.indirectaddr to nothing
  // set fileInode.attributes


  // write iNode to disk
  int writeStatus = myPM->writeDiskBlock(nodeBlock, fileInode);

  // check for some other error
  if (writeStatus != 0) {
    return -4;
  }
  return 0;
}
int FileSystem::createDirectory(char *dirname, int dnameLen)
{

}
int FileSystem::lockFile(char *filename, int fnameLen)
{

}
int FileSystem::unlockFile(char *filename, int fnameLen, int lockId)
{

}
int FileSystem::deleteFile(char *filename, int fnameLen)
{

}
int FileSystem::deleteDirectory(char *dirname, int dnameLen)
{

}
int FileSystem::openFile(char *filename, int fnameLen, char mode, int lockId)
{

  int fileDescriptor = fileDescriptorGenerator.getUniqueNumber();
  openFileInstance.fileDescription = fileDescriptor;
  openFileQueue->push_back(openFileInstance);
  return fileDescriptor;

}
int FileSystem::closeFile(int fileDesc)
{
  if (fileDesc < 1 || typeid(fileDesc) != typeid(int))
  {
    //File descriptor is invalid.
    return -1;
  }
  //Our file descriptor is valid, and we can begin iterating through our deque
  //to find the structure related to the file that is currently open, and needs to be closed. 
  else if (fileDesc >= 1)
  {
    deque<int>::iterator it;
    for (auto it = openFileQueue->begin(); it != openFileQueue->end(); ++it)
    {
      DerivedOpenFile temp = *it;
      if (temp.fileDescription == fileDesc)
      {
        openFileQueue->erase(it);
        return 0;
      }
    }
  }

  //Anything else happens, return -2
  return -2;
}

int FileSystem::readFile(int fileDesc, char *data, int len)
{
  return 0;
}

/*
  Returns the following values based on the following conditions
  -1 if file descriptor is invalid
  -2 if length is negative
  -3 if operation is not permitted
  The number of bytes written, if successful

*/
int FileSystem::writeFile(int fileDesc, char *data, int len)
{
  //Unique nums generated will never be below 1, should also be an int
  if (fileDesc < 1 || typeid(fileDesc) != typeid(int)) 
  {
    return -1;
  }

  //Is length negative?
  else if (len < 0)
  {
    return -2;
  }

  //To check if operation is permitted, make sure that the file is open in the 
  //openFileQueue, and it's mode is write. If not, we can't write to it, so return -3
  deque<int>::iterator it;
  for (auto it = openFileQueue->begin(); it != openFileQueue->end(); ++it)
  {
    DerivedOpenFile temp = *it;
    if (temp.mode == 'w')
    {
      //If the rwpointer is at zero, this means that we can start at the start of a block.
      //If it is not, we need to start at the pointer, and adjust from there
      int memBlocksRequired = ceil((len + temp.readWritePointer)/64.0);

      //Need to get Inode setup next, still trying to do this
    }
  }

  return -3;
}
int FileSystem::appendFile(int fileDesc, char *data, int len)
{

}
int FileSystem::seekFile(int fileDesc, int offset, int flag)
{

}
int FileSystem::renameFile(char *filename1, int fnameLen1, char *filename2, int fnameLen2)
{

}
int FileSystem::getAttribute(char *filename, int fnameLen /* ... and other parameters as needed */)
{

}
int FileSystem::setAttribute(char *filename, int fnameLen /* ... and other parameters as needed */)
{

}
