CC=g++
#CFLAGS = -ggdb 
CFLAGS = -Wall -ggdb

all: drTester1 drTester2 drTester3 drTester4 drTester5 drTester6

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
drTester1: drivers/driver1.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o dnode.o fnode.o inode.o
	$(CC) $(CFLAGS) -o drTester1 drivers/driver1.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o dnode.o fnode.o inode.o

drTester2: drivers/driver2.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o dnode.o fnode.o inode.o
	$(CC) $(CFLAGS) -o drTester2 drivers/driver2.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o dnode.o fnode.o inode.o

drTester3: drivers/driver3.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o dnode.o fnode.o inode.o
	$(CC) $(CFLAGS) -o drTester3 drivers/driver3.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o dnode.o fnode.o inode.o

drTester4: drivers/driver4.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o dnode.o fnode.o inode.o
	$(CC) $(CFLAGS) -o drTester4 drivers/driver4.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o dnode.o fnode.o inode.o

drTester5: drivers/driver5.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o dnode.o fnode.o inode.o
	$(CC) $(CFLAGS) -o drTester5 drivers/driver5.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o dnode.o fnode.o inode.o

drTester6: drivers/driver6.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o dnode.o fnode.o inode.o
	$(CC) $(CFLAGS) -o drTester6 drivers/driver6.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o dnode.o fnode.o inode.o

#
# CLEAN
#
run:
	./drTester ./drTester2 ./drTester3
clean:
	rm -f partitionmanager.o diskmanager.o bitvector.o disk.o filesystem.o dnode.o fnode.o inode.o DISK1 drTester drTester1 drTester2 drTester3 drTester4 drTester5 drTester6
