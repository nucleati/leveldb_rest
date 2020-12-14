CC=c++
# CFLAGS=-O3 -Wall --std=c++11
CFLAGS=--std=c++11
#-g
LDFLAGS=-lleveldb -lpthread -lboost_system 
INC=-I/Simple-Web-Server/ 
#-I./rest_errors/

leveldb_rest: leveldb_rest.o set_endpoints.o rest_errors.o
	$(CC) $(CFLAGS) -o leveldb_rest leveldb_rest.o set_endpoints.o rest_errors.o $(LDFLAGS) $(INC)

leveldb_rest.o: leveldb_rest.cpp
	$(CC) $(CFLAGS) -c leveldb_rest.cpp

set_endpoints.o: set_endpoints.cpp
	$(CC) $(CFLAGS) -c set_endpoints.cpp

rest_errors.o: rest_errors/rest_errors.cpp
	$(CC) $(CFLAGS) -c rest_errors/rest_errors.cpp

clean:
	rm *.o leveldb_rest