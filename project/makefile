CC=g++
#CFLAGS = -ggdb 
CFLAGS = -Wall -ggdb

all: drTester

#
# FILE SYSTEM
#
bitvector.o: bitvector.cpp  bitvector.h
	$(CC) $(CFLAGS) -c bitvector.cpp

filesystem.o: filesystem.cpp filesystem.h
	$(CC) $(CFLAGS) -c filesystem.cpp

#
# DISK AND LOW LEVEL FILES
#
disk.o: disk.cpp  disk.h
	$(CC) $(CFLAGS) -c disk.cpp

diskmanager.o: diskmanager.cpp  diskmanager.h
	$(CC) $(CFLAGS) -c diskmanager.cpp

partitionmanager.o: partitionmanager.cpp  partitionmanager.h
	$(CC) $(CFLAGS) -c partitionmanager.cpp

#
# NODES
#
dnode.o: nodes/dnode.cpp nodes/dnode.h
	$(CC) $(CFLAGS) -c nodes/dnode.cpp

fnode.o: nodes/fnode.cpp nodes/fnode.h
	$(CC) $(CFLAGS) -c nodes/fnode.cpp

inode.o: nodes/inode.cpp nodes/inode.h
	$(CC) $(CFLAGS) -c nodes/inode.cpp

#
# MAIN BUILD
#
drTester: driver.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o dnode.o fnode.o inode.o
	$(CC) $(CFLAGS) -o drTester driver.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o dnode.o fnode.o inode.o

#
# CLEAN
#
clean:
	rm -f partitionmanager.o diskmanager.o bitvector.o disk.o filesystem.o dnode.o fnode.o inode.o DISK1 drTester