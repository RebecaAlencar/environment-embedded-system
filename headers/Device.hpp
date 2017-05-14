#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <string>
#include <list>
#include "ExecutionEnvironment.hpp"

class Device {

private:
	std::string name;
	std::list<ExecutionEnvironment*> executionsEnvironment;
	
public:
	Device(std::string);

	void setName(std::string);

	std::string getName();
	std::list<ExecutionEnvironment*>* getExecutionsEnvironment();

};

#endif