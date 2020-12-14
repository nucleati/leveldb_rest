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
|PATH|Example Payload|Response|
||
