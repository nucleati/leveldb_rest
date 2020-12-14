#include </Simple-Web-Server/client_http.hpp>
#include </Simple-Web-Server/server_http.hpp>
#include <future>

// Added for the json-example
#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <algorithm>
#include <boost/filesystem.hpp>
#include <fstream>
#include <vector>
#include <map>
#ifdef HAVE_OPENSSL
#include "crypto.hpp"
#endif

#include "config/config.hpp"
#include "set_endpoints.hpp"

#include "leveldb/db.h"
#include "leveldb/write_batch.h"
#include "leveldb/filter_policy.h"


using namespace std;
// Added for the json-example:
using namespace boost::property_tree;

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;

int main(int argc, char** argv) {

  Config config;

  if(argc != 2) {
      std::cerr << "Can't start server without configuration file: " << std::endl;
      std::cerr << "Correct use: " << argv[0] << " configuration.json" << endl; 
      return(-1);
  } else {
    try{
      char* filename = argv[1];
      config.readJson(filename);
    }
    catch(...){
      throw("Something went wrong while reading configuration file.");
    }
  }

  HttpServer server;
  server.config.port = std::stoi(config.getValue("port"));

  std::map<std::string, leveldb::DB*> ldbs;
  leveldb::Options options;
  options.create_if_missing = true;

  // User/Project database
  for(unsigned i = 0 ; i < config.config_tree->get_child("dbs").size() ; i++){
    std::string db_name  = "dbs.[" + std::to_string(i) + "].db";
    std::string db_mount = "dbs.[" + std::to_string(i) + "].mp";
    std::string db_path  = "dbs.[" + std::to_string(i) + "].path";

    std::string db_name_and_path = config.getValue(db_path) + config.getValue(db_name) ;
    
    leveldb::DB* db;
    leveldb::Status status = leveldb::DB::Open(options, db_name_and_path, &db) ;

    std::cout << "Received handle from database : " << db_name_and_path << std::endl;

    if (false == status.ok()){
      cout << "Unable to open database " << db_name_and_path << " ." << endl;
      cout << status.ToString() << endl;
      return -1;
    }
    // Set up paths for requests
    std::string dbm = config.getValue(db_mount);
    set_endpoints(server, db, dbm);
  }

  server.on_error = [](shared_ptr<HttpServer::Request> /*request*/, const SimpleWeb::error_code & /*ec*/) {
    // Handle errors here
    // Note that connection timeouts will also call this handle with ec set to SimpleWeb::errc::operation_canceled
  };

  // Start server and receive assigned port when server is listening for requests
  promise<unsigned short> server_port;
  thread server_thread([&server, &server_port]() {
    // Start server
    server.start([&server_port](unsigned short port) {
      server_port.set_value(port);
    });
  });

  cout << "Server listening on port " << server_port.get_future().get() << endl
       << endl;

  server_thread.join();
}
