#include "disk.h"
#include "diskmanager.h"
#include "partitionmanager.h"
#include "filesystem.h"
#include "fileFeatures.h"
#include <time.h>
#include <cstdlib>
#include <iostream>
#include <deque>
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
  
}
int FileSystem::createDirectory(char *dirname, int dnameLen)
{

}
int FileSystem::lockFile(char *filename, int fnameLen)
{

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
//Will Malone: The openFile system is very incomplete right now
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
  
  //INCOMPLETE: Search inodes to try to find file with name filename
  //Return -1 to signify that the file could not be found within the filesystem
  return -1;
  
  //Create the open file instance and add it to the open file queue
  int fileDescriptor = fileDescriptorGenerator.getUniqueNumber();
  openFileInstance.fileDescription = fileDescriptor;
  openFileQueue->push_back(openFileInstance);
  return fileDescriptor;
  //Return value for any other unspecified reason
  return -4;
}
int FileSystem::closeFile(int fileDesc)
{
  if (fileDesc < 1)
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
      }
    }
    return 0;
  }

  //Anything else happens, return -2
  return -2;
}

int FileSystem::readFile(int fileDesc, char *data, int len)
{
  return 0;
}
int FileSystem::writeFile(int fileDesc, char *data, int len)
{

  return 0;
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
