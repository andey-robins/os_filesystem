
/* Driver 6*/

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "../disk.h"
#include "../diskmanager.h"
#include "../partitionmanager.h"
#include "../filesystem.h"
#include "../client.h"

using namespace std;

/*
  This driver will test the getAttributes() and setAttributes()
  functions. You need to complete this driver according to the
  attributes you have implemented in your file system, before
  testing your program.
  
  
  Required tests:
  get and set on the fs1 on a file
    and on a file that doesn't exist
    and on a file in a directory in fs1
    and on a file that doesn't exist in a directory in fs1

 fs2, fs3
  on a file both get and set on both fs2 and fs3

  samples are provided below.  Use them and/or make up your own.


*/

int main()
{

    Disk *d = new Disk(300, 64, const_cast<char *>("DISK1"));
    DiskPartition *dp = new DiskPartition[3];

    dp[0].partitionName = 'A';
    dp[0].partitionSize = 100;
    dp[1].partitionName = 'B';
    dp[1].partitionSize = 75;
    dp[2].partitionName = 'C';
    dp[2].partitionSize = 105;

    //
    // All this stuff is commented out until we write the driver to supress compiler warnings
    //

    DiskManager *dm = new DiskManager(d, 3, dp);
    FileSystem *fs1 = new FileSystem(dm, 'A');
    FileSystem *fs2 = new FileSystem(dm, 'B');
    FileSystem *fs3 = new FileSystem(dm, 'C');
    Client *c1 = new Client(fs1);
    Client *c2 = new Client(fs2);
    Client *c3 = new Client(fs3);
    Client *c4 = new Client(fs1);
    Client *c5 = new Client(fs2);

    int r;
    char eBuf0[3], eBuf1[3], eBuf2[3], eBuf3[3];
    char eBufr0[3], eBufr1[3], eBufr2[3], eBufr3[3];
    char dBuf0[12], dBuf1[12], dBuf2[12], dBuf3[12];
    char dBufr0[12], dBufr1[12], dBufr2[12], dBufr3[12];

    eBuf0[0] = '>';
    eBuf0[1] = ':';
    eBuf0[2] = ')';

    eBuf1[0] = '>';
    eBuf1[1] = ':';
    eBuf1[2] = 'O';

    eBuf2[0] = ':';
    eBuf2[1] = 'P';
    eBuf2[2] = '\0';

    eBuf3[0] = ':';
    eBuf3[1] = 'D';
    eBuf3[2] = '\0';

    dBuf0[0] = '0';
    dBuf0[1] = '0';
    dBuf0[2] = '2';
    dBuf0[3] = '3';
    dBuf0[4] = '0';
    dBuf0[5] = '0';
    dBuf0[6] = '1';
    dBuf0[7] = '1';
    dBuf0[8] = '2';
    dBuf0[9] = '0';
    dBuf0[10] = '2';
    dBuf0[11] = '0';

    dBuf1[0] = '0';
    dBuf1[1] = '0';
    dBuf1[2] = '2';
    dBuf1[3] = '5';
    dBuf1[4] = '0';
    dBuf1[5] = '0';
    dBuf1[6] = '1';
    dBuf1[7] = '1';
    dBuf1[8] = '2';
    dBuf1[9] = '0';
    dBuf1[10] = '2';
    dBuf1[11] = '0';

    dBuf2[0] = '0';
    dBuf2[1] = '0';
    dBuf2[2] = '1';
    dBuf2[3] = '7';
    dBuf2[4] = '0';
    dBuf2[5] = '0';
    dBuf2[6] = '0';
    dBuf2[7] = '9';
    dBuf2[8] = '2';
    dBuf2[9] = '0';
    dBuf2[10] = '2';
    dBuf2[11] = '0';

    dBuf3[0] = '0';
    dBuf3[1] = '0';
    dBuf3[2] = '0';
    dBuf3[3] = '9';
    dBuf3[4] = '0';
    dBuf3[5] = '0';
    dBuf3[6] = '0';
    dBuf3[7] = '3';
    dBuf3[8] = '2';
    dBuf3[9] = '0';
    dBuf3[10] = '2';
    dBuf3[11] = '0';

    //What every need to show your set and get Attributes functions work

    r = c1->myFS->setAttribute(const_cast<char *>("/e/f"), 4, 'E', eBuf0, 3);
    cout << "rv from setAttributes /e/f is " << r << (r == 0 ? " correct" : " fail") << endl;
    r = c4->myFS->setAttribute(const_cast<char *>("/e/b"), 4, 'E', eBuf0, 3);
    cout << "rv from setAttributes /e/b is " << r << (r == 0 ? " correct" : " fail") << endl;
    r = c1->myFS->getAttribute(const_cast<char *>("/e/f"), 4, 'E', eBufr0, 3);
    cout << "rv from getAttributes /e/f is " << r << (r == 0 ? " correct" : " fail") << endl;
    cout << "Attribute is ";
    for (int i = 0; i < 3; i++)
    {
        cout << eBufr0[i];
    }
    cout << endl;
    r = c4->myFS->getAttribute(const_cast<char *>("/e/b"), 4, 'E', eBufr0, 3);
    cout << "rv from getAttributes /e/b is " << r << (r == 0 ? " correct" : " fail") << endl;
    cout << "Attribute is ";
    for (int i = 0; i < 3; i++)
    {
        cout << eBufr0[i];
    }
    cout << endl;
    r = c1->myFS->getAttribute(const_cast<char *>("/p"), 2, 'E', eBufr0, 3); //should failed!
    cout << "rv from getAttributes /p is " << r << (r == -2 ? " correct" : " fail") << endl;
    r = c4->myFS->setAttribute(const_cast<char *>("/p"), 2, 'E', eBuf0, 3); //should failed!
    cout << "rv from setAttributes /p is " << r << (r == -2 ? " correct" : " fail") << endl;

    r = c2->myFS->setAttribute(const_cast<char *>("/f"), 2, 'E', eBuf2, 3);
    cout << "rv from setAttributes /f is " << r << (r == 0 ? " correct" : " fail") << endl;
    r = c5->myFS->setAttribute(const_cast<char *>("/z"), 2, 'E', eBuf1, 3);
    cout << "rv from setAttributes /z is " << r << (r == 0 ? " correct" : " fail") << endl;
    r = c2->myFS->getAttribute(const_cast<char *>("/f"), 2, 'E', eBufr2, 3);
    cout << "rv from getAttributes /f is " << r << (r == 0 ? " correct" : " fail") << endl;
    cout << "Attribute is ";
    for (int i = 0; i < 3; i++)
    {
        cout << eBufr2[i];
    }
    cout << endl;
    r = c5->myFS->getAttribute(const_cast<char *>("/z"), 2, 'E', eBufr1, 3);
    cout << "rv from getAttributes /z is " << r << (r == 0 ? " correct" : " fail") << endl;
    cout << "Attribute is ";
    for (int i = 0; i < 3; i++)
    {
        cout << eBufr1[i];
    }
    cout << endl;
    //Could've sworn that these got created in driver 4 but okay?
    r = c3->myFS->createFile(const_cast<char *>("/o/o/o/z/l"), 10);
    r = c3->myFS->setAttribute(const_cast<char *>("/o/o/o/z/l"), 10, 'D', dBuf0, 12);
    cout << "rv from setAttributes /o/o/o/z/l is " << r << (r == 0 ? " correct" : " fail") << endl;
    r = c3->myFS->createFile(const_cast<char *>("/o/o/o/z/d"), 10);
    r = c3->myFS->setAttribute(const_cast<char *>("/o/o/o/z/d"), 10, 'D', dBuf1, 12);
    cout << "rv from setAttributes /o/o/o/z/d is " << r << (r == 0 ? " correct" : " fail") << endl;
    r = c3->myFS->getAttribute(const_cast<char *>("/o/o/o/z/l"), 10, 'D', dBufr0, 12);
    cout << "rv from getAttributes /o/o/o/z/l is " << r << (r == 0 ? " correct" : " fail") << endl;
    cout << "Attribute is ";
    for (int i = 0; i < 12; i++)
    {
        cout << dBufr0[i];
    }
    cout << endl;
    r = c3->myFS->getAttribute(const_cast<char *>("/o/o/o/z/d"), 10, 'D', dBufr1, 12);
    cout << "rv from getAttributes /o/o/o/z/d is " << r << (r == 0 ? " correct" : " fail") << endl;
    cout << "Attribute is ";
    for (int i = 0; i < 12; i++)
    {
        cout << dBufr1[i];
    }
    cout << endl;

    r = c2->myFS->setAttribute(const_cast<char *>("/f"), 2, 'D', dBuf2, 12);
    cout << "rv from setAttributes /f is " << r << (r == 0 ? " correct" : " fail") << endl;
    r = c5->myFS->setAttribute(const_cast<char *>("/z"), 2, 'D', dBuf3, 12);
    cout << "rv from setAttributes /z is " << r << (r == 0 ? " correct" : " fail") << endl;
    r = c2->myFS->getAttribute(const_cast<char *>("/f"), 2, 'D', dBufr2, 12);
    cout << "rv from getAttributes /f is " << r << (r == 0 ? " correct" : " fail") << endl;
    cout << "Attribute is ";
    for (int i = 0; i < 12; i++)
    {
        cout << dBufr2[i];
    }
    cout << endl;
    r = c5->myFS->getAttribute(const_cast<char *>("/z"), 2, 'D', dBufr3, 12);
    cout << "rv from getAttributes /z is " << r << (r == 0 ? " correct" : " fail") << endl;
    cout << "Attribute is ";
    for (int i = 0; i < 12; i++)
    {
        cout << dBufr3[i];
    }
    cout << endl;
    r = c3->myFS->setAttribute(const_cast<char *>("/o/o/o/z/l"), 10, 'E', eBuf3, 3);
    cout << "rv from setAttributes /o/o/o/z/l is " << r << (r == 0 ? " correct" : " fail") << endl;
    r = c3->myFS->setAttribute(const_cast<char *>("/o/o/o/z/d"), 10, 'E', eBuf0, 3);
    cout << "rv from setAttributes /o/o/o/z/d is " << r << (r == 0 ? " correct" : " fail") << endl;
    r = c3->myFS->getAttribute(const_cast<char *>("/o/o/o/z/l"), 10, 'E', eBufr3, 3);
    cout << "rv from getAttributes /o/o/o/z/l is " << r << (r == 0 ? " correct" : " fail") << endl;
    cout << "Attribute is ";
    for (int i = 0; i < 3; i++)
    {
        cout << eBufr3[i];
    }
    cout << endl;
    r = c3->myFS->getAttribute(const_cast<char *>("/o/o/o/z/d"), 10, 'E', eBufr0, 3);
    cout << "rv from getAttributes /o/o/o/z/d is " << r << (r == 0 ? " correct" : " fail") << endl;
    cout << "Attribute is ";
    for (int i = 0; i < 3; i++)
    {
        cout << eBufr0[i];
    }
    cout << endl;

    cout << "Driver 6 finished. Big thank @ Dr. Ward. This project was hella frustrating, but learned bigly." << endl;
    cout << "Wow, that's a lot of code! - NYTimes" << endl;
    cout << "'Uge. Tremenjus. - The DT himself" << endl;
    cout << "A modern example of how nobody knows what they're doing. - Our Group" << endl;
    cout << "Debugger? I hardly know her! - Mental jokes I tell myself to keep myself going" << endl;
    return 0;
}
