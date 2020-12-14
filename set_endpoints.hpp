#ifndef SET_ENDPOINTS_HPP
#define SET_ENDPOINTS_HPP

#include<iostream>
#include </Simple-Web-Server/client_http.hpp>
#include </Simple-Web-Server/server_http.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <algorithm>
#include "leveldb/db.h"
#include "leveldb/write_batch.h"
#include "leveldb/filter_policy.h"
#include "config/config.hpp"
#include "rest_errors/rest_errors.hpp"
#include "rest_errors/exceptions.hpp"

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;
using namespace std;
using namespace boost::property_tree;

void set_endpoints(HttpServer &, leveldb::DB*, std::string &);

#endif // SET_ENDPOINTS_HPP