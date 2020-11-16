#include "disk.h"
#include "diskmanager.h"
#include "partitionmanager.h"
#include "filesystem.h"
#include "fileFeatures.h"
#include "nodes/nodes.h"
#include <time.h>
#include <cstdlib>
#include <iostream>
#include <deque>
#include <typeinfo>
#include <cmath>
#include <cstring>
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
  fileDescriptorGenerator.initShuffle();
}
int FileSystem::createFile(char *filename, int fnameLen)
{

  // validate filename
  for (int i = 0; i < fnameLen; i++)
  {
    if (i % 2 == 0)
    {
      // should be /
      if (filename[i] != '/')
      {
        return -3;
      }
    }
    else
    {
      // should be alpha char
      if (!isalpha(filename[i]))
      {
        return -3;
      }
    }
  }

  // file exists: return -1
  deque<int>::iterator it;
  for (auto it = fileExistsQueue->begin(); it != fileExistsQueue->end(); ++it)
  {
    DerivedFileExists temp = *it;
    if (temp.fileName == filename)
    {
      return -1;
    }
  }

  // allocate the file blocks
  int nodeBlock = myPM->getFreeDiskBlock();
  int dataBlock = myPM->getFreeDiskBlock();

  // check that there is space for the file
  if (nodeBlock == -1 || dataBlock == -1)
  {
    // if the node was correctly allocated, free the block since we can't create the file
    if (nodeBlock != -1)
    {
      myPM->returnDiskBlock(nodeBlock);
    }

    // i believe this should be redundant, but to be safe, also check the dataBlock in case it was allocated incorrectly -andey
    if (nodeBlock != -1)
    {
      myPM->returnDiskBlock(dataBlock);
    }

    return -2;
  }

  // create file iNode
  char fileInode[64];
  FNode fileNode = FNode::createFileNode(filename[fnameLen - 1], dataBlock);
  FNode::fileNodeToBuffer(fileNode, fileInode);
 
  // write iNode to disk
  int writeStatus = myPM->writeDiskBlock(nodeBlock, fileInode);

  // check for some other error
  if (writeStatus != 0)
  {
    return -4;
  }

  // everything has gone correctly so store the file's existence
  fileExistsInstance.fileName = filename;
  fileExistsInstance.fileNameLength = fnameLen;
  fileExistsInstance.iNodePosition = nodeBlock;
  fileExistsQueue->push_back(fileExistsInstance);
  return 0;
}
int FileSystem::createDirectory(char *dirname, int dnameLen)
{
}
int FileSystem::lockFile(char *filename, int fnameLen)
{
  bool isFileExisting = false; 
  try
  {
    // file exists: no return -2
    deque<int>::iterator itLock;
    for (auto itLock = fileExistsQueue->begin(); itLock != fileExistsQueue->end(); ++itLock)
    {
      DerivedFileExists temp = *itLock;
      if (temp.fileName == filename)
      {
        isFileExisting = true;
      }
    }

    if (!isFileExisting)
    {
      return -2;
    }

    // file is unlocked: no return -1
    for (auto itLock = lockedFileQueue->begin(); itLock != lockedFileQueue->end(); ++itLock)
    {
      DerivedLockedFile temp = *itLock;
      if (temp.fileName == filename)
      {
        return -1;
      }
    }

    // file isn't opened: open return -3
    for (auto itLock = openFileQueue->begin(); itLock != openFileQueue->end(); ++itLock)
    {
      DerivedOpenFile temp = *itLock;
      if (temp.fileName == filename)
      {
        return -3;
      }
    }

    // lock file
    lockedFileInstance.fileName = filename;
    lockedFileInstance.fileNameLength = fnameLen;
    lockedFileInstance.isLocked = true;
    lockedFileInstance.lockId = fileDescriptorGenerator.getUniqueNumber();
    lockedFileQueue->push_back(lockedFileInstance);

    // return lockId
    return lockedFileInstance.lockId;
  }
  catch (exception e)
  {
    // something unknown went wrong
    return -4;
  }
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
        else
          return -1;
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
  if (mode != 'r' && mode != 'w' && mode != 'm')
    return -2;
  //Check if the file is currently locked. If the file is locked and the lockId does
  //not match with its lockId, return -3 to indicate locking error
  bool locked = false;
  deque<int>::iterator it;
  for (auto it = lockedFileQueue->begin(); it != lockedFileQueue->end(); it++)
  {
    DerivedLockedFile tmp = *it;
    if (tmp.fileNameLength == fnameLen)
    {
      if (strcmp(filename, tmp.fileName) == 0)
      {
        if (lockId != tmp.lockId) return -3;
        else
        {
          locked = true;
          break;
        } 
      }
    }
  }
  //Return -3 to indicate the file is not locked and lockId is not -1
  if (!locked && lockId != -1) return -3;
  
  //Begin searching through the file existence queue to see if the file exists
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
  try
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

    //Can't find item in the queue, so it must not exist or the descriptor is not valid
    // return -1
    return -1;
  }
  //Anything else, return -2
  catch(exception e)
  {
    return -2;
  }
}

/*
  Returns the following values based on the following conditions
  -1 if file descriptor is invalid
  -2 if length is negative
  -3 if operation is not permitted
  returns the number of bytes read, if successful

*/
int FileSystem::readFile(int fileDesc, char *data, int len)
{
  // invalid file descriptor
  if (fileDesc < 1 || typeid(fileDesc) != typeid(int))
  {
    return -1;
  }

  // negative length to read
  if (len < 0) {
    return -2;
  }

  // an operation will not be permitted if:
  //  a) the file isn't open or
  //  b) the mode isn't 'r'
  bool operationPermitted = false;
  deque<int>::iterator it;
  DerivedOpenFile activeFile;
  for (auto it = openFileQueue->begin(); it != openFileQueue->end(); ++it) {
    DerivedOpenFile temp = *it;
    if (temp.mode == 'r' && temp.fileDescription == fileDesc) {
      // file fulfills operation conditions
      operationPermitted = true;
      activeFile = temp;
    }
  }

  // operation not permitted
  if (!operationPermitted) {
    return -3;
  }

  //
  // READ OPERATION
  //

  // get the position of the file iNode
  int fileInodePosition = findFileINode(activeFile);
  if (fileInodePosition == -1) {
    return -3;
  }

  // read in the iNode
  char fileNodeInputBuffer[64];
  int res = myPM->readDiskBlock(fileInodePosition, fileNodeInputBuffer);
  if (res != 0) {
    return -3;
  }
  
  // convert the read iNode buffer to an Fnode object
  FNode fileInode = FNode::loadFileNode(fileNodeInputBuffer);

  char activeBlock[64];
  int nextRwPointer = 0;

  for (int i = activeFile.readWritePointer; i < len + activeFile.readWritePointer + 1 && i < fileInode.size + 1; i++) {
    // check if we need new block
    if (i == activeFile.readWritePointer || i % 64 == 0) {

      int neededBlock = floor(i / 64.0);

      // checks that our neededBlock won't be outside the max possible size
      // is this redundant due to the i < fileInode.size check for the forloop?
      if (neededBlock > 19) {
        return -3;
      }

      // determine if we need to get from direct or indirect addressing & block address
      if (neededBlock < 3) {
        // direct address
        neededBlock = fileInode.directAddress[neededBlock];
      } else {
        // indirect address
        char indirectNodeBuffer[64];
        res = myPM->readDiskBlock(fileInode.indirectAddress, indirectNodeBuffer);
        // error reading indirect node block
        if (res != 0) {
          return -3;
        }

        INode indirectInode = INode::loadIndirNode(indirectNodeBuffer);

        neededBlock = indirectInode.directPointers[neededBlock - 3];
      }

      res = myPM->readDiskBlock(neededBlock, activeBlock);
      if (res != 0) {
        return -3;
      }
    }

    // read from blockbuffer to databuffer
    data[i - activeFile.readWritePointer] = activeBlock[i % 64];
    nextRwPointer = i;
  }

  // update rwpointer
  int temp = activeFile.readWritePointer;
  activeFile.readWritePointer = nextRwPointer;

  // return
  return activeFile.readWritePointer - temp;
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
    if ((temp.mode == 'w' || temp.mode == 'm') && temp.fileDescription == fileDesc)
    {
      //If the rwpointer is at zero, this means that we can start at the start of a block.
      //If it is not, we need to start at the pointer, and adjust from there
      int memBlocksRequired = ceil((len + temp.readWritePointer) / 64.0);
      int startingBlock = floor(temp.readWritePointer / 64.0);
      //cout << "memory blocks required for " << temp.fileName <<" is" << memBlocksRequired << endl;
      //cout << "The rwPointer is at " << temp.readWritePointer << endl;
      //cout << "The starting block is " << startingBlock << endl;
      char fNodeBuff[64];
      int iNodeBlockPosition = findFileINode(temp);
      //cout << "The fnode block position required for " << temp.fileName <<" is" << iNodeBlockPosition << endl;

      if (iNodeBlockPosition == -1)
      {
        return -3;
      }
      //Need to load the file iNode that was written to disk from createFile
      myPM->readDiskBlock(iNodeBlockPosition, fNodeBuff);
      //Must convert this buffer into an FNode object
      FNode fNodeObj = FNode::loadFileNode(fNodeBuff);

      /*Now that we have an object to work with, we must parse the file bytes required for 
        writting, and get direct/indirect addressing set up. If the file bytes required are
        more than three direct addressing blocks, we will divert to indirect addressing.
        We already have one direct address assigned from createFile, so determine if the 
        size needed will need that one, or more.
      */
      int newSize = temp.readWritePointer + len;
      //cout <<"The new size of this file is " << newSize << endl; 

      //We now need to count the current blocks, and determine if more are needed
      //in addition to updating addressing.
      int success = assignDirectAddress(fNodeObj, memBlocksRequired, newSize, iNodeBlockPosition);
      //Once the above function has been called, all direct/indirect addressing has been counted
      //and allocated accordingly, need to start writing where the read write pointer begins
      //and update other fNode attributes like new size and rwPointer pos
      if (success == -1)
      {
        //We have a problem counting and assigning blocks
        return -4;
      }

      //Get updated FNode info
      fNodeBuff[0] = {'0'};
      myPM->readDiskBlock(iNodeBlockPosition, fNodeBuff);
      fNodeObj = FNode::loadFileNode(fNodeBuff);
      
      int location = temp.readWritePointer;
      int loopIndex = 0;
      char writeBuffer[64];
      bool isIndirect = false;
      INode iNode;
      //BEGIN WRITING
      //If we are starting with indirect addressing based on the starting block position
      if (startingBlock > 2)
      {
        char indirectAddressInfo[64];
        isIndirect = true;
        myPM->readDiskBlock(fNodeObj.indirectAddress, indirectAddressInfo);
        iNode = INode::loadIndirNode(indirectAddressInfo);
      }
      myPM->readDiskBlock(fNodeObj.directAddress[startingBlock], writeBuffer);
      while (loopIndex != len)
      {
        writeBuffer[location%64] = data[loopIndex];
        location++;
        loopIndex++;
        //If we have reached the next block, or the last iteration of the loop
        if (location % 64 == 0 || loopIndex == len)
        {
          //Check again to see if we need to switch to indirect addressing
          //Use the && to make sure that this section is only executed once.
          if (startingBlock > 2 && isIndirect == false)
          {
            char indirectAddressInfo[64];
            isIndirect = true;
            myPM->readDiskBlock(fNodeObj.indirectAddress, indirectAddressInfo);
            iNode = INode::loadIndirNode(indirectAddressInfo);
          }
          //192 bytes is the limit of our three direct addresses
          if (isIndirect == false)
          {
            //If we are here, we have reached the end of a direct block or about to exit loop
            //cout << "Writing to disk now via direct addressing! " << endl;
            //cout << "Writing to address " << fNodeObj.directAddress[startingBlock] << endl;
            myPM->writeDiskBlock(fNodeObj.directAddress[startingBlock], writeBuffer);
          }

          else if (isIndirect == true)
          {
            //If we are here, we have gone into indirect addressing or about to exit loop
            //cout << "Writing to disk now via indirect addressing! " << endl;
           //cout << "Writing to address " << iNode.directPointers[(startingBlock) - 3] << endl;
            myPM->writeDiskBlock(iNode.directPointers[(startingBlock) - 3], writeBuffer);
          }
          startingBlock++;
          //cout << "We are now on block " << startingBlock <<endl;
        }
      }
      //Assign written, update file inode, and rwpointer location, write to disk, and return
      int written = loopIndex;
      fNodeObj.size = temp.readWritePointer + written;
      temp.readWritePointer = location;
      openFileQueue->erase(it);
      openFileQueue->push_back(temp);
      char outputBuffer[64];
      FNode::fileNodeToBuffer(fNodeObj, outputBuffer);
      myPM -> writeDiskBlock(iNodeBlockPosition, outputBuffer);
      return written;
    }
  }
  return -3;
}

int FileSystem::appendFile(int fileDesc, char *data, int len)
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
    if ((temp.mode == 'w' || temp.mode == 'm') && temp.fileDescription == fileDesc)
    {
      char fNodeBuff[64];
      int iNodeBlockPosition = findFileINode(temp);

      if (iNodeBlockPosition == -1)
      {
        return -3;
      }
      //Need to load the file iNode that was written to disk from createFile
      myPM->readDiskBlock(iNodeBlockPosition, fNodeBuff);
      //Must convert this buffer into an FNode object
      FNode fNodeObj = FNode::loadFileNode(fNodeBuff);
      //To append, set the read/write pointer to the size of the file, and call write file 
      //with this new start block/location information.
      temp.readWritePointer = fNodeObj.size;
      openFileQueue->erase(it);
      openFileQueue->push_back(temp);
      //cout << "In append, the new rwpoint pos is " << temp.readWritePointer << endl;
      //cout << "In append, the name of file is " << temp.fileName << endl;
      return writeFile(fileDesc, data, len);
    }
  }
  return -3;
}
int FileSystem::seekFile(int fileDesc, int offset, int flag)
{
  //Negative offsets are invalid if the flag is nonzero, so we return -1
  if (flag != 0 && offset < 0) return -1;
  //Iterate through the queue of open files, looking for one with a matching file description
  deque<int>::iterator it;
  for (auto it = openFileQueue->begin(); it != openFileQueue->end(); ++it)
  {
    DerivedOpenFile tmp = *it;
    //Check if we have found the desired open file
    if (tmp.fileDescription == fileDesc)
    {
      //Look for the same file in the file existence queue so we can find its iNodePosition
      //We then read the block data into a temporary buffer and create a FNode object from it to
      //access the size of the File in bytes
      int iNodePos = findFileINode(tmp);
      char tempBuff[64];
      myPM->readDiskBlock(iNodePos, tempBuff);
      FNode fileINode = FNode::loadFileNode(tempBuff);

      //Calculate the value of updated pointer and check it before altering original
      int potential_rw = tmp.readWritePointer;
      if (flag == 0) potential_rw += offset;
      else potential_rw = offset;
      //Check if the new read write pointer is valid based on the file size above
      if (potential_rw > 0 && potential_rw < fileINode.size)
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

int FileSystem::assignDirectAddress(FNode fNode, int memBlocks, int fileSize, int inodeBlockPosition)
{
  int blocksInUse = 0;
  //Begin by looking at all current addressing, direct and indirect to tally what is in
  //use right now.
  //If new filesize only requires one block, return as this is already prepared for us

  if (ceil(fileSize / 64.0) == 1)
  {
    return 0;
  }
  //immediately check if we have/need indirect address
  if (memBlocks > 3)
  {

    //If we are here, we know that we will have the max of three direct addresses filled.
    int success = assignIndirectAddress(fNode, memBlocks, inodeBlockPosition);
    if (success == -1)
    {
      return -1;
    }
    return 0;
  }

  //Check how many blocks we are already using
  for (int i = 0; i < 3; i++)
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
    int diff = memBlocks - blocksInUse;

    if (diff < 0)
    {
      //The difference is negative, and so we need to release some direct address blocks
      while (diff != 0)
      {
        //Do I need to release a block via the PM as well?
        myPM->returnDiskBlock(fNode.directAddress[blocksInUse - 1]);
        fNode.directAddress[blocksInUse -1] = 0;
        blocksInUse--;
        diff = memBlocks - blocksInUse;
      }
    }

    //The difference is positive, and so we need to add some direct address blocks
    else if (diff > 0)
    {
      while (diff != 0)
      {
        fNode.directAddress[blocksInUse - 1] = myPM->getFreeDiskBlock();
        if (fNode.directAddress[blocksInUse - 1] == -1)
        {
          //Did we run out of space?
          fNode.directAddress[blocksInUse - 1] = 0;
          return -1;
        }
        blocksInUse++;
        diff = memBlocks - blocksInUse;
      }
    }
    //Write changes to disk
    char outputBuffer[64];
    FNode::fileNodeToBuffer(fNode, outputBuffer);
    myPM->writeDiskBlock(inodeBlockPosition, outputBuffer);
  }
  return 0;
}

int FileSystem::assignIndirectAddress(FNode fNode, int memBlocks, int iNodeBlockPosition)
{
  //Need to determine how many direct pointers we need in our indirect based on how many
  //blocks are required, and how many have been taken up by direct addressing.

  int diff = memBlocks - 3;

  //Take care of all direct addresses unless they are already filled, remember to not
  //mess with the first one.
  for (int i = 1; i < 3; i++)
  {
    if (fNode.directAddress[i] == 0)
    {
      fNode.directAddress[i] = myPM->getFreeDiskBlock();
      if (fNode.directAddress[i] == -1)
      {
        //Did we run out of space?
        
        fNode.directAddress[i] = 0;
        return -1;
      }
    }
  }
  //Write direct addressing to disk.
  char outputBuffer[64];
  //FNode::fileNodeToBuffer(fNode, outputBuffer);
  
  //myPM->writeDiskBlock(iNodeBlockPosition, outputBuffer);
  //cout << " I have written the new direct addressing " << endl;
  //This means that we have no indirect addressing set up yet.
  if (fNode.indirectAddress == 0)
  {
    //We need to create and indirect address
    int indirectBlock = myPM->getFreeDiskBlock();

    if (indirectBlock == -1)
    {
      //Not enough space, so return.
      return -1;
    }

    //Write new fNode information back to disk.
    fNode.indirectAddress = indirectBlock;
    FNode::fileNodeToBuffer(fNode, outputBuffer);
    //cout << "New file node is " << outputBuffer << endl; 
    myPM->writeDiskBlock(iNodeBlockPosition, outputBuffer);

    INode indirNode = INode::createIndirNode();

    for (int i = 0; i < diff; i++)
    {
      int directPointerValue = myPM->getFreeDiskBlock();
      if (directPointerValue == -1)
      {
        //We don't have enough space
        return -1;
      }
      indirNode.directPointers[i] = directPointerValue;
    }
  
    char indirNodeBuff[64];
    INode::indirNodeToBuffer(indirNode, indirNodeBuff);

    // write indirect node to disk
    //cout << "The indir node will look like " << indirNodeBuff << endl;
    int writeStatus = myPM->writeDiskBlock(indirectBlock, indirNodeBuff);
    return 0;
  }

  //We do have indirect addressing set up, and so we need to determine how many pointers
  //we are using, and how many we need.
  else if (fNode.indirectAddress != 0)
  {
    char indirectBuffer[64];
    int existingPointers = 0;
    myPM->readDiskBlock(fNode.indirectAddress, indirectBuffer);
    INode existingIndirectNode = INode::loadIndirNode(indirectBuffer);

    for (int i = 0; i < 16; i++)
    {
      if (existingIndirectNode.directPointers[i] != 0)
      {
        existingPointers++;
      }
    }
    //Get difference while taking into account direct addressing

    diff = (memBlocks - 3) - existingPointers;

    //If diff is zero, we are fine, but if it is positive or negative, we need to adjust.
    if (diff != 0)
    {
      if (diff < 0)
      {
        //The difference is negative, and so we need to release some indirect pointer values
        while (diff != 0)
        {
          //Do I need to release a block via the PM as well?
          myPM->returnDiskBlock(existingIndirectNode.directPointers[existingPointers - 1]);
          existingIndirectNode.directPointers[existingPointers - 1] = 0;
          existingPointers--;
          diff = (memBlocks - 3) - existingPointers;
        }
      }

      //The difference is positive, and so we need to add some indirect pointer values
      else if (diff > 0)
      {
        while (diff != 0)
        {
          existingIndirectNode.directPointers[existingPointers - 1] = myPM->getFreeDiskBlock();
          if (existingIndirectNode.directPointers[existingPointers - 1] == -1)
          {
            //Did we run out of space?
            existingIndirectNode.directPointers[existingPointers - 1] = 0;
            return -1;
          }
          existingPointers++;
          diff = (memBlocks - 3) - existingPointers;
        }
      }
      //Write new information to disk
      char outputBuffer[64];
      INode::indirNodeToBuffer(existingIndirectNode, outputBuffer);
      myPM->writeDiskBlock(fNode.indirectAddress, outputBuffer);
    }
    return 0;
  }
  return -1;
}
