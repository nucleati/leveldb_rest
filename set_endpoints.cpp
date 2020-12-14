#include <iostream>
#include "set_endpoints.hpp"
#include "rest_errors/exceptions.hpp"

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;
using namespace std;

void set_endpoints(HttpServer & server, leveldb::DB* db, std::string & mp){
	std::string p1 = "^/dbs/" + mp + "/(\\S+)$" ;
	std::string p2 = "^/dbs/" + mp + "/?";

	server.resource[p1]["GET"] = [&, db](shared_ptr<HttpServer::Response> response, 
		shared_ptr<HttpServer::Request> request) {
		std::string key = request->path_match[1];
		std::string value;
		leveldb::Status s = db->Get(leveldb::ReadOptions(), static_cast<leveldb::Slice>(key), &value);
		if(s.ok())
		{
			std::stringstream return_response;
			return_response << "{ \"value\" : \"";
			return_response << value ;
			return_response << "\"}" ;
			*response << "HTTP/1.1 200 Success\r\nContent-Length: " << return_response.tellp() << 
				"\r\n\r\n" << return_response.str(); 	
		} else {
			std::string message = "The queried key was not found";
        	ErrorResponse er = ErrorResponse(400, "Not Found" , message);
			*response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << er.toJSON().size() << 
				"\r\n\r\n" << er.toJSON();
		}
	};

	server.resource[p2]["PUT"] = [&, db](shared_ptr<HttpServer::Response> response, 
		shared_ptr<HttpServer::Request> request) {
		// std::cout << "Inside PUT" << std::endl;
		try {
			leveldb::WriteBatch batchToWrite;
			std::map<std::string, std::string> kvs;
			try {
				boost::property_tree::ptree root;
				read_json(request->content, root);
				for(boost::property_tree::ptree::value_type &v: root.get_child("data")){
					boost::property_tree::ptree subtree2 = v.second;
					std::string key = subtree2.get<std::string>("key");
					std::string value = subtree2.get<std::string>("value");
					kvs.insert(std::pair<std::string,std::string>(key,value));
				}			
			} catch(...) {
				std::string message = "Unable to parse JSON payload, please make sure the provided data is in correct format" ;
				throw(LevelDBRestException(message));
			}

			try {
				for(auto it = kvs.begin(); it != kvs.end() ; ++it){
					batchToWrite.Put(it->first, it->second) ;
				}
				db->Write(leveldb::WriteOptions(), &batchToWrite);
			} catch(...) {
				std::string message = "Although the JSON payload was parsed, we were not able to write the key/values to database" ;
				throw(LevelDBRestException(message));
			}
			std::stringstream return_response ;
			return_response << "{\"status\": \"success\"}";
			*response << "HTTP/1.1 200 Success\r\nContent-Length: " << return_response.tellp() << 
				"\r\n\r\n" << return_response.str(); 	
		} catch(LevelDBRestException e){
	    	ErrorResponse er = ErrorResponse(400, "Bad Request" , e.what());
			*response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << er.toJSON().size() << 
				"\r\n\r\n" << er.toJSON();
		} catch(std::exception e){
	    	ErrorResponse er = ErrorResponse(500, "Internal Server Error" , e.what());
			*response << "HTTP/1.1 500 Internal Server Error\r\nContent-Length: " << er.toJSON().size() << 
				"\r\n\r\n" << er.toJSON();			
		} catch(...){
	    	ErrorResponse er = ErrorResponse(500, "Internal Server Error (non std::exception)" , "Unkonwn error occured, please contact admin.");
			*response << "HTTP/1.1 500 Internal Server Error\r\nContent-Length: " << er.toJSON().size() << 
				"\r\n\r\n" << er.toJSON();				
		}
	};

	server.resource[p2]["POST"] = [&, db](shared_ptr<HttpServer::Response> response, 
		shared_ptr<HttpServer::Request> request) {
		std::cout << "Inside POST" << std::endl;
		std::stringstream return_response ;
		try {
			std::vector<std::string> keys;
			try {
				boost::property_tree::ptree root;
				read_json(request->content, root);
				for(boost::property_tree::ptree::value_type &v: root.get_child("keys")){
					keys.push_back(v.second.get<std::string>(""));
				}
			} catch(...) {
				std::string message = "Unable to parse JSON payload, please make sure the provided data is in correct format" ;
				throw(LevelDBRestException(message));
			}

			try {
				return_response << "{\"data\" : [";
				for(auto it = keys.begin(); it != keys.end(); ++it){
					std::string value;
					return_response << "{\"key\": " << "\"" << *it << "\"";
					leveldb::Status s = db->Get(leveldb::ReadOptions(), static_cast<leveldb::Slice>(*it), &value);					
					if(s.ok()) {
						return_response << ",\n\"value\":" << "\"" << value << "\"" ; 
					}
					return_response <<  "}";
					if(std::next(it) != keys.end()){
						return_response << ",\n";
					}
				}
				return_response << "]}";
				*response << "HTTP/1.1 200 Success\r\nContent-Length: " << return_response.tellp() << 
					"\r\n\r\n" << return_response.str(); 	
			} catch(...){
				std::string message = "Although provided keys were parsed from JSON, querying database or generating JSON response was unsuccesfull" ;
				throw(LevelDBRestException(message));				
			}
		} catch(LevelDBRestException e){
	    	ErrorResponse er = ErrorResponse(400, "Bad Request" , e.what());
			*response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << er.toJSON().size() << 
				"\r\n\r\n" << er.toJSON();
		} catch(std::exception e){
	    	ErrorResponse er = ErrorResponse(500, "Internal Server Error" , e.what());
			*response << "HTTP/1.1 500 Internal Server Error\r\nContent-Length: " << er.toJSON().size() << 
				"\r\n\r\n" << er.toJSON();			
		} catch(...){
	    	ErrorResponse er = ErrorResponse(500, "Internal Server Error (non std::exception)" , "Unkonwn error occured, please contact admin.");
			*response << "HTTP/1.1 500 Internal Server Error\r\nContent-Length: " << er.toJSON().size() << 
				"\r\n\r\n" << er.toJSON();				
		}
	};

	server.resource[p1]["PATCH"] = [&, db](shared_ptr<HttpServer::Response> response, 
		shared_ptr<HttpServer::Request> request) {

		std::cout << "Inside PATCH" << std::endl;
		try{	
			std::string key = request->path_match[1];
			std::cout << "Requested key is " << key << std::endl;
			std::string new_value ;	
			try {
				boost::property_tree::ptree root;
				read_json(request->content, root);
				std::cout << request->content.string() << std::endl;
				new_value = root.get<std::string>("value");
			} catch(...) {
				std::string message = "Unable to parse JSON payload, please make sure the provided data is in correct JSON format and has a property named \"value\"" ;
				throw(LevelDBRestException(message));			
			}

			std::string value ;
			leveldb::Status s1 = db->Get(leveldb::ReadOptions(), static_cast<leveldb::Slice>(key), &value);
			
			if(s1.ok()){
				leveldb::Status s2 = db->Put(leveldb::WriteOptions(), static_cast<leveldb::Slice>(key), new_value);
				if(s2.ok()){
					std::stringstream return_response ;
					return_response << "{\"status\": \"success\"}";
					*response << "HTTP/1.1 200 Success\r\nContent-Length: " << return_response.tellp() << 
						"\r\n\r\n" << return_response.str(); 					
				} else {
					std::string message = "Attemp to update key/value failed." ;
					throw(LevelDBRestException(message));						
				}
			} else {
				std::string message = "The key to be updated was not found";
	        	ErrorResponse er = ErrorResponse(400, "Not Found" , message);
				*response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << er.toJSON().size() << 
					"\r\n\r\n" << er.toJSON();
			}
		} 
		catch(LevelDBRestException e){
	    	ErrorResponse er = ErrorResponse(400, "Bad Request" , e.what());
			*response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << er.toJSON().size() << 
				"\r\n\r\n" << er.toJSON();
		}
		catch(std::exception e){
	    	ErrorResponse er = ErrorResponse(500, "Internal Server Error" , e.what());
			*response << "HTTP/1.1 500 Internal Server Error\r\nContent-Length: " << er.toJSON().size() << 
				"\r\n\r\n" << er.toJSON();
			}
		catch(...){
	    	ErrorResponse er = ErrorResponse(500, "Internal Server Error (non std::exception)" , "Unkonwn error occured, please contact admin.");
			*response << "HTTP/1.1 500 Internal Server Error\r\nContent-Length: " << er.toJSON().size() << 
				"\r\n\r\n" << er.toJSON();			
		}
	};


	server.resource[p1]["DELETE"] = [&, db](shared_ptr<HttpServer::Response> response, 
		shared_ptr<HttpServer::Request> request) {
		std::cout << "Inside DELETE" << std::endl;
		std::string key = request->path_match[1];
		try{
			// First get to make sure the key exists
			std::string value ;
			leveldb::Status s1 = db->Get(leveldb::ReadOptions(), static_cast<leveldb::Slice>(key), &value);
			if(s1.ok()){
				leveldb::Status s2 = db->Delete(leveldb::WriteOptions(), static_cast<leveldb::Slice>(key));
				if(s2.ok()){
					std::stringstream return_response ;
					return_response << "{\"status\": \"success\"}";
					*response << "HTTP/1.1 200 Success\r\nContent-Length: " << return_response.tellp() << 
						"\r\n\r\n" << return_response.str(); 					
				} else {
					std::string message = "Attemp to delete existing key/value failed." ;
					throw(LevelDBRestException(message));						
				}
			} else {
				std::string message = "The key to be deleted was not found";
	        	ErrorResponse er = ErrorResponse(400, "Not Found" , message);
				*response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << er.toJSON().size() << 
					"\r\n\r\n" << er.toJSON();
			}
		} 
		catch(LevelDBRestException e){
	    	ErrorResponse er = ErrorResponse(400, "Bad Request" , e.what());
			*response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << er.toJSON().size() << 
				"\r\n\r\n" << er.toJSON();
		}
		catch(std::exception e){
	    	ErrorResponse er = ErrorResponse(500, "Internal Server Error" , e.what());
			*response << "HTTP/1.1 500 Internal Server Error\r\nContent-Length: " << er.toJSON().size() << 
				"\r\n\r\n" << er.toJSON();
			}
		catch(...){
	    	ErrorResponse er = ErrorResponse(500, "Internal Server Error (non std::exception)" , "Unkonwn error occured, please contact admin.");
			*response << "HTTP/1.1 500 Internal Server Error\r\nContent-Length: " << er.toJSON().size() << 
				"\r\n\r\n" << er.toJSON();			
		}
	};
}