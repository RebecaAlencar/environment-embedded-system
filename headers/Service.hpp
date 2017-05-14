#ifndef SERVICE_HPP
#define SERVICE_HPP

#include <string>
#include "Method.hpp"

class Service : public Method {

public:
	Service(std::string, std::string);

};

#endif