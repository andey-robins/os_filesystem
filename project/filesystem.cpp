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
