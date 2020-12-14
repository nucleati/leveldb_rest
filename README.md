# leveldb_rest
LevelDB will let only one process to access the database at a given point of time. As a result when LevelDB is part of actual application container, it will not scale in absence of threading. To overcome that issue, there are several alternatives suggested as a workaround but they are workarounds. As a result, I was forced to create a small application. This simple application takes the leveldb database configuration, port and REST mount points from the configuration file. Then it creates REST requests for each of the database that will help to interact with the databases. 

## Example Configuration file

```
{
 "port": 16010,
 "dbs":[
   {
     "db" : "test1",
     "mp" : "t1",
     "path": "/db/"
   },
   {
     "db" : "test2",
     "mp" : "t2",
     "path": "/db/"
   },
   {
     "db" : "test3",
     "mp" : "t3",
     "path": "/db/"
   }
 ]
}
```

## REST Paths that will be available upon running ./leveldb_rest config.json

There are five key requests available for each database defined in the configuration file. They should be sufficient to interact with leveldb.

| TYPE | PATH| Example Payload | Response(200)|
|----|-----------|-----------|--------------|
|GET|/dbs/{db}/{key}|N/A|{ "value" : "v3"}|
|PUT|/dbs/{db}/|{ "data" : [ { "key" : "thiskey1", "value" :"thisvalue1" }, { "key" : "thiskey2", "value" :"thisvalue2" } ] } | {"status": "success"} |
|DELETE|/dbs/{db}/{key}|N/A|{"status": "success"}|
|POST|/dbs/{db}/|{"keys": ["thiskey1", "thiskey2"]}| {"data" : [{"key": "thiskey1", "value":"thisvalue1"},{"key": "thiskey2","value":"thisvalue2"}]} |
|PATCH|/dbs/{db}/{key}|{"value": "new_value"}|{"status": "success"}|

## Dockerfile that runs the service.
```
FROM ubuntu:18.04
RUN apt-get --fix-missing update
RUN apt-get --assume-yes install gcc
RUN apt-get --assume-yes install g++
RUN apt-get --assume-yes install make
RUN apt-get --assume-yes install build-essential
RUN apt-get --assume-yes install libboost-all-dev
RUN apt-get --assume-yes install openssl
RUN apt-get --assume-yes install cmake
RUN apt-get --assume-yes install git
RUN apt-get --fix-missing update
RUN apt-get --assume-yes install curl
RUN apt-get --assume-yes install vim
WORKDIR /
RUN git clone --recurse-submodules https://github.com/google/leveldb.git
WORKDIR /leveldb
RUN mkdir build
WORKDIR /leveldb/build
RUN cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build .
RUN make
RUN make install
WORKDIR /
RUN git clone https://gitlab.com/eidheim/Simple-Web-Server.git
WORKDIR /Simple-Web-Server
RUN mkdir build
WORKDIR /Simple-Web-Server/build
RUN cmake ..
RUN make
WORKDIR /
RUN git clone https://github.com/nucleati/leveldb_rest
WORKDIR /leveldb_rest
RUN make
CMD ./leveldb_rest /config/config.json
```

## Run the container

To run the built container use following. Make sure to change the docker image id if it is different from 117696becf2e

```
docker run -d -p 16010:16010 --volume $PWD/db:/db --volume $PWD/.bash_history:/root/.bash_history --volume $PWD/config:/config --volume $PWD/log:/log/ 117696becf2e
```

## docker-compose.yml

Example docker-compose file to run along other containers.

```
version: '3.0'
services:
  leveldb_rest:
    image: 117696becf2e
    command: /leveldb_rest/leveldb_rest /config/config.json
    ports:
    - "16010:16010"
    volumes:
    - ./db:/db
    - ./config:/config
    - ./log:/log
    
```
