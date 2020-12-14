#ifndef CONFIGURATION_PARSER
#define CONFIGURATION_PARSER

#include<iostream>
#include<sstream>
#include<string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

template <typename Tree>
Tree query(Tree& pt, typename Tree::path_type path) {
    if (path.empty())
        return pt;

    auto const head = path.reduce();

    auto subscript = head.find('[');
    auto name      = head.substr(0, subscript);
    auto index     = std::string::npos != subscript && head.back() == ']'
        ? std::stoul(head.substr(subscript+1))
        : 0u;

    auto matches = pt.equal_range(name);
    if (matches.first==matches.second)
        throw std::out_of_range("name:" + name);

    for (; matches.first != matches.second && index; --index)
        ++matches.first;

    if (index || matches.first==matches.second)
        throw std::out_of_range("index:" + head);

    return query(matches.first->second, path);
}

class Config
{
public:
	Config(std::string filename){
		boost::property_tree::read_json(filename, *config_tree);
	};
	Config(){};
	~Config(){};
	void readJson(std::string filename){
		boost::property_tree::read_json(filename, *config_tree);
	};
	std::string getValue(std::string property){
		return query(*config_tree, property).get_value<std::string>();
	};
	boost::property_tree::ptree * config_tree = new boost::property_tree::ptree();
};

#endif