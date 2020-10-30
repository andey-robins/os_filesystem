#include <iostream>
#include <fstream>
#include <cstdlib>
#include "disk.h"
#include "diskmanager.h"
#include "partitionmanager.h"
#include "filesystem.h"
#include "client.h"

int main()
{
    Disk *d = new Disk(300, 64, const_cast<char *>("DISK1"));
    DiskPartition *dp = new DiskPartition[3];

    dp[0].partitionName = 'Z';
    dp[0].partitionSize = 200;
    dp[1].partitionName = 'Y';
    dp[1].partitionSize = 50;
    dp[2].partitionName = 'C';
    dp[2].partitionSize = 105;
    cout << "Complete!" << endl;

    DiskManager *dm = new DiskManager(d, 3, dp);

    return 0;
}