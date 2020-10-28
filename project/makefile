CC=g++
#CFLAGS = -ggdb 
CFLAGS = -Wall -ggdb

all: dmTester

 
bitvector.o: bitvector.cpp  bitvector.h
	$(CC) $(CFLAGS) -c bitvector.cpp

bitvector_driver: bitvector.o
	$(CC) $(CFLAGS) bitvector_driver.cpp bitvector.o -o bitvector_driver

disk.o: disk.cpp  disk.h
	$(CC) $(CFLAGS) -c disk.cpp

diskmanager.o: diskmanager.cpp  diskmanager.h
	$(CC) $(CFLAGS) -c diskmanager.cpp

partitionmanager.o: partitionmanager.cpp  partitionmanager.h
	$(CC) $(CFLAGS) -c partitionmanager.cpp

dmTester: dmTester.cpp bitvector.o disk.o diskmanager.o partitionmanager.o
	$(CC) $(CFLAGS)  -o dmTester dmTester.cpp bitvector.o disk.o diskmanager.o partitionmanager.o