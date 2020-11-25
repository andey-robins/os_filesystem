CC=g++
#CFLAGS = -ggdb 
CFLAGS = -Wall -ggdb

all: driver1 driver2 driver3 driver4 driver5 driver6

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
driver1: drivers/driver1.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o dnode.o fnode.o inode.o
	$(CC) $(CFLAGS) -o driver1 drivers/driver1.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o dnode.o fnode.o inode.o

driver2: drivers/driver2.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o dnode.o fnode.o inode.o
	$(CC) $(CFLAGS) -o driver2 drivers/driver2.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o dnode.o fnode.o inode.o

driver3: drivers/driver3.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o dnode.o fnode.o inode.o
	$(CC) $(CFLAGS) -o driver3 drivers/driver3.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o dnode.o fnode.o inode.o

driver4: drivers/driver4.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o dnode.o fnode.o inode.o
	$(CC) $(CFLAGS) -o driver4 drivers/driver4.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o dnode.o fnode.o inode.o

driver5: drivers/driver5.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o dnode.o fnode.o inode.o
	$(CC) $(CFLAGS) -o driver5 drivers/driver5.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o dnode.o fnode.o inode.o

driver6: drivers/driver6.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o dnode.o fnode.o inode.o
	$(CC) $(CFLAGS) -o driver6 drivers/driver6.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o dnode.o fnode.o inode.o

driver: driver.cpp client.h bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o dnode.o fnode.o inode.o
	$(CC)  -o driver driver.cpp bitvector.o disk.o diskmanager.o partitionmanager.o filesystem.o dnode.o fnode.o inode.o
#
# CLEAN
#
clean:
	rm -f partitionmanager.o diskmanager.o bitvector.o disk.o filesystem.o dnode.o fnode.o inode.o DISK1 driver1 driver2 driver3 driver4 driver5 driver6 driver
