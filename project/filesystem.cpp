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
#include <algorithm>
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
    fileDescriptorGenerator.initShuffle();

    //Create the root directory and write it to block 1 in the partition
    //But only if it does not already exist
    char rootBuff[64];
    myPM->readDiskBlock(1, rootBuff);
    if (!isalpha(rootBuff[0]))
    {
        DNode root = DNode::createDirNode('0', 0, '0');
        DNode::dirNodeToBuffer(root, rootBuff);
        myPM->writeDiskBlock(1, rootBuff);
    }
}

/*
Creates a file whoise name is filename and the name has length fnameLen
Returns -1 if the file already exists
        -2 if there is not enough disk space
        -3 if the filename is invalid
        -4 if it fails for some other reason
        0 if the file is created successfully
*/
int FileSystem::createFile(char *filename, int fnameLen)
{
    int existence = pathExists(filename, fnameLen);
    //File already exists
    if (existence > 0)
        return -1;
    else if (existence == -3)
        return -3;

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
    //Everything has gone correctly, so store file's existence in its parent directory
    updateDirectory(filename, fnameLen, 'F', nodeBlock);
    return 0;
}

/*
TODO: Implement this method -- currently returns -100 as major failure
Creates a new directory whos name is dirname of length dnameLen
Returns -1 if the directory already exists
        -2 if there isn't enough disk space
        -3 if the name is invalid
        -4 if the directory cannot be created for any other reason
        0 if the directory is created successfully
*/
int FileSystem::createDirectory(char *dirname, int dnameLen)
{
    return -100;
}

/*
Locks a file and returns a unique lockId to identify the locked file by
Returns -1 if the file is already locked
        -2 if the file doesn't exist
        -3 if the file is currently opened
        -4 if the file cannot be locked for any other reason
        A positive, unique lockId if the file is locked successfully
*/
int FileSystem::lockFile(char *filename, int fnameLen)
{
    try
    {
        // file exists: no return -2
        //May need to validate that the path is not to a dir
        if (pathExists(filename, fnameLen) < 0)
            return -2;

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
    catch (exception &e)
    {
        // something unknown went wrong
        return -4;
    }
}

/*
Unlocks a file. the lockId is the unique lock associated with file filename that was associated
with the file when it was locked
Returns -1 if the lock is invalid
        -2 for any other reason
        0 if the file is unlocked successfully
*/
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

/*
Deletes the file with name filename unless it is locked or open
Returns -1 if the file does not exist
        -2 if the file is locked or open
        -3 if the file cannot be deleted for any other reason
        0 if the file is deleted successfully
*/
int FileSystem::deleteFile(char *filename, int fnameLen)
{
    int res;
    if (openOrLocked(filename, fnameLen))
        return -2;
    //Get the block number of the desired file
    int pathVal = pathExists(filename, fnameLen);
    if (pathVal == -1)
        return -1;
    else if (pathVal < 0)
        return -3;

    //Load the desired block into FNode structure
    char fBuffer[64];
    myPM->readDiskBlock(pathVal, fBuffer);
    FNode toDelete = FNode::loadFileNode(fBuffer);
    for (int i = 0; i < 3; i++)
    {
        //Deallocates a direct address block if it has been used
        if (toDelete.directAddress[i] != 0)
        {
            res = myPM->returnDiskBlock(toDelete.directAddress[i]);
            if (res == -1)
                return -3;
        }
        else
            break;
    }
    //Load in the indirect address and deallocate memory if used
    if (toDelete.indirectAddress != 0)
    {
        myPM->readDiskBlock(toDelete.indirectAddress, fBuffer);
        INode indirect = INode::loadIndirNode(fBuffer);
        for (int i = 0; i < 16; i++)
        {
            if (indirect.directPointers[i] != 0)
            {
                res = myPM->returnDiskBlock(indirect.directPointers[i]);
                if (res == -1)
                    return -3;
            }
            else
                break;
        }
    }
    //Remove the file from its parent node's entries
    int parent = pathExists(filename, fnameLen - 2);
    if (parent < 0)
        return -3;
    myPM->readDiskBlock(parent, fBuffer);
    DNode parentNode = DNode::loadDirNode(fBuffer);
    for (int i = 0; i < 10; i++)
    {
        if (parentNode.entries[i].name == filename[fnameLen - 1])
        {
            parentNode.entries[i].name = '0';
            parentNode.entries[i].subPointer = 0;
            parentNode.entries[i].type = '0';
            break;
        }
    }
    //Write the parent node back to its original location
    DNode::dirNodeToBuffer(parentNode, fBuffer);
    myPM->writeDiskBlock(parent, fBuffer);
    //Delete the block used by the file inode and restore bit vectors
    res = myPM->returnDiskBlock(pathVal);
    if (res == -1)
        return -3;
    else
        return 0;
}

/*
TODO: Implement this method -- currently returns -100 for a major failure
Deletes the directory dirname
Returns -1 if the directory doesn't exist
        -2 if the directory is not empty
        -3 if the directory can't be deleted for any other reason
        0 if the directory is deleted successfully
*/
int FileSystem::deleteDirectory(char *dirname, int dnameLen)
{
    return -100;
}

/*
Opens a file filename in mode mode with a unique lock id of lockId
Returns -1 if the file doesn't exist
        -2 if the mode is invalid
        -3 if the file can't be opened because locking restrictions
        -4 if the file cannot be opened for any other reason
        A unique, positive integer representing the file descriptor if successful        
*/
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
                if (lockId != tmp.lockId)
                    return -3;
                else
                {
                    locked = true;
                    break;
                }
            }
        }
    }
    //Return -3 to indicate the file is not locked and lockId is not -1
    if (!locked && lockId != -1)
        return -3;

    //Begin searching through the file system to see if the file exists
    if (pathExists(filename, fnameLen) > 0)
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
    //Return -1 to signify that the file could not be found within the filesystem
    return -1;
}

/*
Closes the file with file descriptor fileDesc
Returns -1 if the file descriptor is invalid
        -2 for any other reason
        0 if the file is closed successfully
*/
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
    catch (exception &e)
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
    if (len < 0)
    {
        return -2;
    }

    // an operation will not be permitted if:
    //  a) the file isn't open or
    //  b) the mode isn't 'r'
    bool operationPermitted = false;
    deque<int>::iterator it;
    DerivedOpenFile activeFile;
    for (auto it = openFileQueue->begin(); it != openFileQueue->end(); ++it)
    {
        DerivedOpenFile temp = *it;
        if ((temp.mode == 'r' || temp.mode == 'm') && temp.fileDescription == fileDesc)
        {
            // file fulfills operation conditions
            operationPermitted = true;
            activeFile = temp;
            openFileQueue->erase(it);
            break;
        }
    }

    // operation not permitted
    if (!operationPermitted)
    {
        return -3;
    }

    //
    // READ OPERATION
    //

    // get the position of the file iNode
    int fileInodePosition = findFileINode(activeFile);
    if (fileInodePosition == -1)
    {
        return -3;
    }

    // read in the iNode
    char fileNodeInputBuffer[64];
    int res = myPM->readDiskBlock(fileInodePosition, fileNodeInputBuffer);
    if (res != 0)
    {
        return -3;
    }

    // convert the read iNode buffer to an Fnode object
    FNode fileInode = FNode::loadFileNode(fileNodeInputBuffer);

    char activeBlock[64];
    int nextRwPointer = 0;

    for (int i = activeFile.readWritePointer; i < len + activeFile.readWritePointer + 1 && i < fileInode.size + 1; i++)
    {
        // check if we need new block
        if (i == activeFile.readWritePointer || i % 64 == 0)
        {

            int neededBlock = floor(i / 64.0);

            // checks that our neededBlock won't be outside the max possible size
            // is this redundant due to the i < fileInode.size check for the forloop?
            if (neededBlock > 19)
            {
                return -3;
            }

            // determine if we need to get from direct or indirect addressing & block address
            if (neededBlock < 3)
            {
                // direct address
                neededBlock = fileInode.directAddress[neededBlock];
            }
            else
            {
                // indirect address
                char indirectNodeBuffer[64];
                res = myPM->readDiskBlock(fileInode.indirectAddress, indirectNodeBuffer);
                // error reading indirect node block
                if (res != 0)
                {
                    return -3;
                }

                INode indirectInode = INode::loadIndirNode(indirectNodeBuffer);

                neededBlock = indirectInode.directPointers[neededBlock - 3];
            }

            res = myPM->readDiskBlock(neededBlock, activeBlock);
            if (res != 0)
            {
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
    // push the updated file back to the openFileQueue to ensure our work in this method is reflected in the system's states
    openFileQueue->push_back(activeFile);

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
                myPM->readDiskBlock(iNode.directPointers[startingBlock - 3], writeBuffer);
            }

            else if (startingBlock <= 2)
            {
                myPM->readDiskBlock(fNodeObj.directAddress[startingBlock], writeBuffer);
            }

            while (loopIndex != len)
            {
                writeBuffer[location % 64] = data[loopIndex];
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
                        myPM->writeDiskBlock(iNode.directPointers[(startingBlock)-3], writeBuffer);
                    }
                    startingBlock++;
                    //cout << "We are now on block " << startingBlock <<endl;
                }
            }
            //Assign written, update file inode, and rwpointer location, write to disk, and return
            int written = loopIndex;
            fNodeObj.size = max(temp.readWritePointer + written, fNodeObj.size);
            temp.readWritePointer = location;
            openFileQueue->erase(it);
            openFileQueue->push_back(temp);
            char outputBuffer[64];
            FNode::fileNodeToBuffer(fNodeObj, outputBuffer);
            myPM->writeDiskBlock(iNodeBlockPosition, outputBuffer);
            return written;
        }
    }
    return -3;
}

/*
  Returns the following values based on the following conditions
  -1 if file descriptor is invalid
  -2 if length is negative
  -3 if operation is not permitted
  returns the number of bytes appended, if successful
*/
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

/*
Modifies the rwPointer for the file fileDesc by offset forward if flag is set to 0
and otherwise offset forward from the beginning of the file
Returns -1 if the file descriptor, offset, or flag is invalid
        -2 if attempting to go outside the bounds of the file
        0 if the seek operation goes successfully
*/
int FileSystem::seekFile(int fileDesc, int offset, int flag)
{
    //Negative offsets are invalid if the flag is nonzero, so we return -1
    if (flag != 0 && offset < 0)
        return -1;
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
            if (flag == 0)
                potential_rw += offset;
            else
                potential_rw = offset;
            //Check if the new read write pointer is valid based on the file size above
            if (potential_rw >= 0 && potential_rw <= fileINode.size)
            {
                //Mutate the read/write pointer as desired and return 0 to mark success
                tmp.readWritePointer = potential_rw;
                openFileQueue->erase(it);
                openFileQueue->push_back(tmp);
                return 0;
            }
            //Do not modify the pointer and return -2 to indicate the offset and flag would have
            //resulted in a read_write pointer outside of the file bounds
            else
                return -2;
        }
    }
    //The file was not found matching the descriptor given, so return -1
    return -1;
}

/* Renames a file from filename1 to filename2, can be used on a directory as well
  Returns -1 if the filename is invalid
          -2 if the filename does not exist
          -3 if there is already a file with filename2
          -4 if the file is open or locked
          -5 for any other reason
          0 if successful
*/
int FileSystem::renameFile(char *filename1, int fnameLen1, char *filename2, int fnameLen2)
{
    //Check if filename1 does't exist or is invalid or is open/locked
    if (openOrLocked(filename1, fnameLen1))
        return -4;
    int pathRes = pathExists(filename1, fnameLen1);
    if (pathRes == -1)
        return -2;
    else if (pathRes == -3)
        return -1;
    //Check if filename2 already exists or is invalid
    int newPathRes = pathExists(filename2, fnameLen2);
    if (newPathRes > 0)
        return -3;
    else if (newPathRes == -3)
        return -1;
    //Store the old and new values for the name character
    char fileChar = filename1[fnameLen1 - 1];
    char newFileChar = filename2[fnameLen2 - 1];
    char fBuff[64];
    myPM->readDiskBlock(pathRes, fBuff);
    //Check if filename1 corresponded to a directory or a file
    if (fBuff[0] == fileChar)
    {
        //Then we have a file
        FNode original = FNode::loadFileNode(fBuff);
        original.name = newFileChar;
        FNode::fileNodeToBuffer(original, fBuff);
        myPM->writeDiskBlock(pathRes, fBuff);
    }
    //Note: if we have a directory, there is no name field to change in the inode
    //Now we have to also change the name in the parent directory
    int parent = pathExists(filename1, fnameLen1 - 2);
    if (parent < 0)
        return -5;
    myPM->readDiskBlock(parent, fBuff);
    DNode parentNode = DNode::loadDirNode(fBuff);
    for (int i = 0; i < 16; i++)
    {
        //Update the value of the correct parent node entry
        if (parentNode.entries[i].name == fileChar)
        {
            parentNode.entries[i].name = newFileChar;
            break;
        }
    }
    //Write the parent node back to the correct block
    DNode::dirNodeToBuffer(parentNode, fBuff);
    myPM->writeDiskBlock(parent, fBuff);
    return 0;
}

// TODO: Implement this method, define its actions -- returns -100 as a major failure for now
int FileSystem::getAttribute(char *filename, int fnameLen /* ... and other parameters as needed */)
{
    return -100;
}

// TODO: Implement this method, define its actions -- returns -100 as a major failure for now
int FileSystem::setAttribute(char *filename, int fnameLen /* ... and other parameters as needed */)
{
    return -100;
}

int FileSystem::findFileINode(DerivedOpenFile existingOpenFile)
{
    int inode = pathExists(existingOpenFile.fileName, existingOpenFile.fileNameLength);
    if (inode > 0)
        return inode;
    else
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
                fNode.directAddress[blocksInUse - 1] = 0;
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
        // TODO: properly handle a bad result in write Status;
        // can we safely return writeStatus instead of 0? -andey
        int writeStatus = myPM->writeDiskBlock(indirectBlock, indirNodeBuff);
        return writeStatus;
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

/*
The pathExists function takes in the name of either a file or directory, which
is also the path through the filesystem one expects to take to get to the file/directory.
It starts at the root of the filesystem and uses the path to search downward until either the
desired object is found or the search reaches a terminal point.

It returns -1 if the file/directory cannot be found
           -3 if the file/directory name is invalid
           blockNum of the node if it is found
*/
int FileSystem::pathExists(char *path, int pathLen)
{
    //If pathLen is 0, we are looking for the directory
    if (pathLen == 0)
        return 1;
    // validate filename
    for (int i = 0; i < pathLen; i++)
    {
        if (i % 2 == 0)
        {
            // should be /
            if (path[i] != '/')
            {
                return -3;
            }
        }
        else
        {
            // should be alpha char
            if (!isalpha(path[i]))
            {
                return -3;
            }
        }
    }
    //Start searching at root
    bool nextCharFound;
    char dirBuff[64];
    myPM->readDiskBlock(1, dirBuff);
    DNode currentDir = DNode::loadDirNode(dirBuff);

    //Iterate through path name, increment by 2 to account for '/'
    for (int i = 1; i < pathLen; i += 2)
    {
        nextCharFound = false;
        char searchChar = path[i];
        for (int j = 0; j < 10; j++)
        {
            FileEntry tmp = currentDir.entries[j];
            if (tmp.name == searchChar)
            {
                nextCharFound = true;
                //We are at the end of our search
                if (i == (pathLen - 1))
                {
                    return tmp.subPointer;
                }
                //Move into next directory
                if (tmp.type == 'D')
                {
                    myPM->readDiskBlock(tmp.subPointer, dirBuff);
                    currentDir = DNode::loadDirNode(dirBuff);
                    break;
                }
                //Nonterminal path is file, means path invalid
                else
                {
                    return -3;
                }
            }
        }
        //Path invalid if next step not found and not at end of path
        if (!nextCharFound && (i < pathLen - 1))
            return -3;
    }
    //Path is valid, but file/directory does not exist yet
    return -1;
}

/*
Takes in a path that has been added to the system. Updates the directory that holds the
added file or directory to reflect the addition. Assumes the calling function has already validated
the path with pathExists.

Returns directory node pointer if successful
        -1 if the added file does not actually exist
        -2 if out of disk space (and directory overflow)
        -3 if path invalid
*/
int FileSystem::updateDirectory(char *path, int pathLen, char typeAdded, int nodeAdded)
{
    //Read block data
    char buff1[64];
    //Access and validate blocknum of parent directory
    int parentNode = pathExists(path, pathLen - 2);
    if (parentNode < 0)
        return parentNode;
    //Load the parent directory info, edit, then rewrite
    myPM->readDiskBlock(parentNode, buff1);
    DNode parent = DNode::loadDirNode(buff1);
    //We added a directory
    myPM->readDiskBlock(nodeAdded, buff1);

    //Find a directory entry that hasn't been filled
    for (int i = 0; i < 10; i++)
    {
        if (parent.entries[i].subPointer == 0)
        {
            parent.entries[i].subPointer = nodeAdded;
            parent.entries[i].name = path[pathLen - 1];
            parent.entries[i].type = typeAdded;
            break;
        }
    }
    //Rewrite modified directory to its original position
    DNode::dirNodeToBuffer(parent, buff1);
    myPM->writeDiskBlock(parentNode, buff1);
    //Success, return pointer to directory modified
    return parentNode;
}

/*
Checks if the file named filename is in the open queue or locked queue
Returns true if the file is in either queue
        false if the file is not in either queue
*/
bool FileSystem::openOrLocked(char *filename, int fNameLen)
{
    for (auto it = lockedFileQueue->begin(); it != lockedFileQueue->end(); ++it)
    {
        DerivedLockedFile temp = *it;
        if (temp.fileNameLength == fNameLen)
        {
            if (strcmp(temp.fileName, filename) == 0)
            {
                return -1;
            }
        }
    }
    for (auto it = openFileQueue->begin(); it != openFileQueue->end(); ++it)
    {
        DerivedOpenFile temp = *it;
        if (temp.fileNameLength == fNameLen)
        {
            if (strcmp(temp.fileName, filename) == 0)
            {
                return -1;
            }
        }
    }
    return 0;
}