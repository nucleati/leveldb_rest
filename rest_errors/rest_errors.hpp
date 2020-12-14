#ifndef ERROR_HELPER_FOR_RESTAPI
#define ERROR_HELPER_FOR_RESTAPI

#include<iostream>
#include<string>
using namespace std;

class ErrorResponse {
public:
	ErrorResponse(unsigned short int , std::string , std::string);
	ErrorResponse(std::string , std::string);
	~ErrorResponse();
	unsigned short int code;	
	std::string message;
	std::string details;
	std::string toString();
	std::string toJSON();
	std::string noCodeJSON();
};

#endif