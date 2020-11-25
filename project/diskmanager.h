using namespace std;

class DiskPartition
{
public:
    char partitionName;
    int partitionSize;
    int startBlock;
    int endBlock;
};

class DiskManager
{
    Disk *myDisk;
    int partCount;
    DiskPartition *diskP;

    /* declare other private members here */
private:
    void fillPartitionInfo(char *buffer, int num, int pos, int diskPIndex);
    int retrievePartitionInfo(char *buffer);
    //Global shared variables to help with block listing and indexing
    int bufferIndexer = 1;
    int rootOffset = 1;
    int endBlockOffset = 1;
    int blockCount = 0;

public:
    DiskManager(Disk *d, int partCount, DiskPartition *dp);
    ~DiskManager();
    int readDiskBlock(char partitionname, int blknum, char *blkdata);
    int writeDiskBlock(char partitionname, int blknum, char *blkdata);
    int getBlockSize() { return myDisk->getBlockSize(); };
    int getPartitionSize(char partitionname);
};
