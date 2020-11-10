CC=g++
#CFLAGS = -ggdb 
CFLAGS = -Wall -ggdb

all: dmTester

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
nodes/dnode.o: dnode.cpp dnode.h
	$(CC) $(CFLAGS) -c dnode.cpp

nodes/fnode.o: fnode.cpp fnode.h
	$(CC) $(CFLAGS) -c fnode.cpp

nodes/inode.o: inode.cpp inode.h
	$(CC) $(CFLAGS) -c inode.cpp

#
# MAIN BUILD
#
dmTester: dmTester.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o nodes/dnode.o nodes/fnode.o nodes/inode.o
	$(CC) $(CFLAGS) -o dmTester dmTester.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o nodes/dnode.o nodes/fnode.o nodes/inode.o

#
# CLEAN
#
clean:
	rm -f partitionmanager.o diskmanager.o bitvector.o disk.o filesystem.o DISK1 dmTester