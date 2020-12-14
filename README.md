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
