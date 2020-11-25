#include "fileFeatures.h"
#include "deque"
#include "nodes/nodes.h"

class FileSystem
{
    DiskManager *myDM;
    PartitionManager *myPM;
    char myfileSystemName;
    int myfileSystemSize;

    struct DerivedLockedFile : LockedFiles
    {
    };
    struct DerivedOpenFile : OpenFiles
    {
    };
    struct DerivedFileDescriptor : FileDescriptor
    {
    };

    // what're these used for as class level variables? -andey
    DerivedFileDescriptor fileDescriptorGenerator;
    DerivedOpenFile openFileInstance;
    DerivedLockedFile lockedFileInstance;
    deque<DerivedLockedFile> *lockedFileQueue;
    deque<DerivedOpenFile> *openFileQueue;

    /* declare other private members here */

public:
    FileSystem(DiskManager *dm, char fileSystemName);
    ~FileSystem();
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
    int getAttribute(char *filename, int fnameLen, char attributeType, char *attributeRes, int attributeResLen);
    int setAttribute(char *filename, int fnameLen, char attributeType, char *attributeVal, int attributeValLen);

    // custom helper methods
    int findFileINode(DerivedOpenFile exisitingOpenFile);
    int assignDirectAddress(FNode fNode, int memBlocks, int fileSize, int iNodeBlockPosition);
    int assignIndirectAddress(FNode fNode, int memBlocks, int iNodeBlockPosition);

    bool validateFilename(char *fname, int fnameLen);
    int findFile(char *fname, int fnameLen);
    int findDirectory(char *dname, int dnameLen);

    int pathExists(char *path, int pathLen);
    int updateDirectory(char *path, int pathLen, char typeAdded, int nodeAdded);
    bool openOrLocked(char *filename, int fNameLen);
};
