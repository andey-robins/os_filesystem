#include "disk.h"
#include "diskmanager.h"
#include "partitionmanager.h"
#include "filesystem.h"
#include "fileFeatures.h"
#include  "nodes.h"
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
  fileExistsQueue = new deque<DerivedFileExists>[1];
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

    return -2;
  }

  // create file iNode
  char fileInode[64];
  fileInode[0] = filename[fnameLen - 1]; // set file name
  fileInode[1] = 'F'; // set type as file
  // set fileInode.size
  // set fileInode.directAddr.1 to dataBlock
  // set fileInode.indirectaddr to nothing
  // set fileInode.attributes

  fileExistsInstance.fileName = filename;
  fileExistsInstance.fileNameLength = fnameLen;
  fileExistsInstance.iNodePosition = nodeBlock;
  fileExistsQueue->push_back(fileExistsInstance);

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
  //Andey will do.
}
int FileSystem::unlockFile(char *filename, int fnameLen, int lockId)
{
  //Create a boolean for whether the desired locked file has been found within the locked file queue
  bool found;
  
  //Iterate through the locked file queue looking for file matching filename
  deque<int>::iterator it;
  for (auto it = lockedFileQueue->begin(); it != lockedFileQueue->end(); it++)
  {
    DerivedLockedFile temp = *it;
    //First we check to see if the lengths of our filenames are equal
    if (temp.fileNameLength == fnameLen)
    {
      //Begin checking for character equality to see if filenames are of equal length
      found = true;
      for (int i = 0; i < fnameLen; i++) 
      {
        if (filename[i] != temp.fileName[i])
        {
          found = false;
          break;
        }
      }
      //This is the case where we have found a matching filename in the locked file queue
      if (found) 
      {
        if (temp.lockId == lockId)
        {
          lockedFileQueue->erase(it);
          //Return value indicating successful unlocking of the file (erased from queue)
          return 0;
        }
        //The lock IDs do not match, so we return -1
        else return -1;
      }
    }
  }
  //Return value for any other reason
  return -2;
}
int FileSystem::deleteFile(char *filename, int fnameLen)
{

}
int FileSystem::deleteDirectory(char *dirname, int dnameLen)
{

}
int FileSystem::openFile(char *filename, int fnameLen, char mode, int lockId)
{
  //Address the case where the mode provided is invalid
  if (mode != 'r' && mode != 'w' && mode != 'm') return -2;
  //Attempt to unlock the file with lockId and save the return value
  int unlockResult = unlockFile(filename, fnameLen, lockId);
  //Return -3 to signify locking problem if the file is locked and lockId does not match or if 
  //the file is not in the locked queue but lockId is not -1
  if (unlockResult == -1) return -3;
  else if (unlockResult == -2 && lockId != -1) return -3;
  
  //Begin searching through the file existence queue to see if the file exists
  deque<int>::iterator it;
  for (auto it = fileExistsQueue->begin(); it != fileExistsQueue->end(); it++)
  {
    DerivedFileExists tmp = *it;
    if (tmp.fileNameLength == fnameLen)
    {
      bool found = true;
      for (int i = 0; i < fnameLen; i++)
      {
        if (filename[i] != tmp.fileName[i])
        {
          found = false;
          break;
        }
      }
      //We have found the correct file in our system, now we open it
      if (found)
      {
        //Create the open file instance and add it to the open file queue
        //We first have to generate a fileDescriptor integer and fill in
        //the DerivedOpenFile fields of fileDescriptor, fileName, fileNameLength,
        //readWritePointer, mode, and ?lockId.
        DerivedOpenFile opened;
        int fileDescriptor = fileDescriptorGenerator.getUniqueNumber();
        opened.fileDescription = fileDescriptor;
        opened.fileName = filename;
        opened.fileNameLength = fnameLen;
        opened.readWritePointer = 0;
        opened.mode = mode;
        opened.lockId = lockId;
        openFileQueue->push_back(opened);
        //Return the fileDescriptor to indicate a successful open
        return fileDescriptor;
      }
    }
  }
  //Return -1 to signify that the file could not be found within the filesystem
  return -1;
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
      int startingBlock = ceil(temp.readWritePointer/64.0);
      
      char fNodeBuff[64];
      int iNodeBlockPosition = findFileINode(temp);

      if (iNodeBlockPosition == -1)
      {
        return -3;
      }
      //Need to load the file iNode that was written to disk from createFile 
      myPM->readDiskBlock(iNodeBlockPosition, fNodeBuff);
      //Must convert this buffer into an FNode object
      FNode fNodeObj = loadFileNode(fNodeBuff);

      /*Now that we have an object to work with, we must parse the file bytes required for 
        writting, and get direct/indirect addressing set up. If the file bytes required are
        more than three direct addressing blocks, we will divert to indirect addressing.
        We already have one direct address assigned from createFile, so determine if the 
        size needed will need that one, or more.
      */
      int newSize = temp.readWritePointer + len;

      //We now need to count the current blocks, and determine if more are needed
      //in addition to updating addressing.
      assignDirectAddress(fNodeObj, memBlocksRequired, newSize);
      //Once the above function has been called, all direct/indirect addressing has been counted
      //and allocated accordingly, need to start writing where the read write pointer begins
      //and update other fNode attributes like new size and rwPointer pos

    }
  }

  return -3;
}
int FileSystem::appendFile(int fileDesc, char *data, int len)
{

}
int FileSystem::seekFile(int fileDesc, int offset, int flag)
{
  //Negative offsets are invalid if the flag is nonzero, so we return -1
  if (flag != 0 && offfset < 0) return -1;
  //Iterate through the queue of open files, looking for one with a matching file description
  for (auto it = openFileQueue->begin(); it != openFileQueue->end(); ++it)
  {
    DerivedOpenFile tmp = *it;
    //Check if we have found the desired open file
    if (tmp.fileDescription == fileDesc)
    {
      //Calculate the value of updated pointer and check it before altering original
      int potential_rw = tmp.readWritePointer;
      if (flag == 0) potential_rw += offset;
      else potential_rw = offset;
      //INCOMPLETE
      //Need to check if the potential_rw pointer is outside the bounds of the file
      //Either need a filesize in openfile data structure or need to access the inode
      //corresponding to the file to access filesize.
      //Currently, we are simply using a dummy size of 100 bytes as an upper bound.
      if (potential_rw > 0 && potential_rw < 100)
      {
        //Mutate the read/write pointer as desired and return 0 to mark success
        tmp.readWritePointer = potential_rw;
        return 0;
      }
      //Do not modify the pointer and return -2 to indicate the offset and flag would have
      //resulted in a read_write pointer outside of the file bounds
      else return -2;
    }
  }
  //The file was not found matching the descriptor given, so return -1
  return -1;
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

int FileSystem::findFileINode(DerivedOpenFile existingOpenFile)
{
  deque<int>::iterator it;
  for (auto it = fileExistsQueue->begin(); it != fileExistsQueue->end(); ++it)
  {
    DerivedFileExists temp = *it;
    if (temp.fileName == existingOpenFile.fileName)
    {
      return temp.iNodePosition;
    }
  }
  return -1;
}

void FileSystem::assignDirectAddress(FNode fNode, int memBlocks, int fileSize)
{
  int blocksInUse = 0;
  //Begin by looking at all current addressing, direct and indirect to tally what is in 
  //use right now.
  //If new filesize only requires one block, return as this is already prepared for us
  
  if (ceil(fileSize/64.0) == 1)
  {
    return;
  }
  //immediately check if we have/need indirect address
  if (memBlocks > 3)
  {
    //If we are here, we know that we will have the max of three direct addresses filled. 
    int directBlocks = 3;
    assignIndirectAddress(fNode, directBlocks);
    return;
  }

  //Check how many blocks we are already using
  for(int i = 0; i < 3; i++)
  {
    if (fNode.directAddress[i] != 0)
    {
      blocksInUse++;
    }

  }

  //Begin allocating direct address blocks as needed
  if (blocksInUse != memBlocks)
  {
    //We need to adjust direct address blocks based on the difference 
    int diff = memBlocks-blocksInUse;

    if (diff < 0)
    {
      //The difference is negative, and so we need to release some direct address blocks
      while (diff != 0)
      {
        //Do I need to release a block via the PM as well?
        myPM->returnDiskBlock(fNode.directAddress[blocksInUse]);
        fNode.directAddress[blocksInUse] = 0;
        blocksInUse--;
        diff = memBlocks-blocksInUse;
      }
    }

    //The difference is positive, and so we need to add some direct address blocks
    else if (diff > 0)
    {
      while (diff !=0)
      {
        fNode.directAddress[blocksInUse] = myPM->getFreeDiskBlock();
        blocksInUse++;
        diff = memBlocks-blocksInUse;
      }
    }
  }
}

void FileSystem::assignIndirectAddress(FNode fNode, int directBlocks)
{
  if (fNode.indirectAddress == 0)
  {
    //We need to create and indirect address
    int indirectBlock = myPM->getFreeDiskBlock();
    if (indirectBlock == -1)
    {
      //Not enough space, so return.
      return;
    }
    fNode.indirectAddress = indirectBlock;
  }

  //Take care of all direct addresses unless they are already filled
  for (int i = 1; i < 3; i++)
  {
    if (fNode.directAddress[i] == 0)
    {
      fNode.directAddress[i] = myPM->getFreeDiskBlock();
    }
  }

  //TODO: Finish this function, not sure if blocks are getting allocated correctly
}
