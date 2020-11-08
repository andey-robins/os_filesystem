#include "fileFeatures.h"
#include "deque"


class FileSystem {
  DiskManager *myDM;
  PartitionManager *myPM;
  char myfileSystemName;
  int myfileSystemSize;

  struct DerivedLockedFile : LockedFiles{};
  struct DerivedOpenFile : OpenFiles{};
  struct DerivedFileDescriptor : FileDescriptor{};
  struct DerivedFileExists : FileExists{};

  DerivedFileDescriptor fileDescriptorGenerator;
  DerivedOpenFile openFileInstance;
  DerivedLockedFile lockedFileInstance;
  DerivedFileExists fileExistsInstance;
  deque<DerivedLockedFile>* lockedFileQueue;
  deque<DerivedOpenFile>* openFileQueue;
  deque<DerivedFileExists>* fileExistsQueue;
  
  /* declare other private members here */
 
  public:
    FileSystem(DiskManager *dm, char fileSystemName);
    int createFile(char *filename, int fnameLen);
    int createDirectory(char *dirname, int dnameLen);
    int lockFile(char *filename, int fnameLen);
    int unlockFile(char *filename, int fnameLen, int lockId);
    int deleteFile(char *filename, int fnameLen);
    int deleteDirectory(char *dirname, int dnameLen);
    int openFile(char *filename, int fnameLen, char mode, int lockId);
    int closeFile(int fileDesc);
    int readFile(int fileDesc, char *data, int len);
    int writeFile(int fileDesc, char *data, int len);
    int appendFile(int fileDesc, char *data, int len);
    int seekFile(int fileDesc, int offset, int flag);
    int renameFile(char *filename1, int fnameLen1, char *filename2, int fnameLen2);
    int getAttribute(char *filename, int fnameLen /* ... and other parameters as needed */);
    int setAttribute(char *filename, int fnameLen /* ... and other parameters as needed */);

    /* declare other public members here */

};
