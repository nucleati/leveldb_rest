#include<iostream>
#include<string>
#include<sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "rest_errors.hpp"
using namespace std;

ErrorResponse::ErrorResponse(unsigned short int code , std::string message, std::string details):code(code),message(message),details(details){};
ErrorResponse::ErrorResponse(std::string message, std::string details):message(message),details(details){ code = 200 ; };

ErrorResponse::~ErrorResponse(){};

std::string ErrorResponse::toString(){
	std::ostringstream text_response;
	text_response << "code => " << std::to_string(code) <<  
					"; message => " << message << 
					"; details => " << details;
	return(text_response.str());
}

std::string ErrorResponse::toJSON(){
	std::ostringstream text_response;

	boost::property_tree::ptree root;
	root.put("code" , std::to_string(code) );
	root.put("message" , message );
	root.put("details" , details );
	boost::property_tree::write_json(text_response, root);
	
	return(text_response.str());
}

std::string ErrorResponse::noCodeJSON(){
	std::ostringstream text_response;
	boost::property_tree::ptree root;
	root.put("message" , message );
	root.put("details" , details );
	boost::property_tree::write_json(text_response, root);
	return(text_response.str());
}