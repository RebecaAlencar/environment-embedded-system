#include "Device.hpp"

Device::Device(std::string name){
	setName(name);
}

std::string Device::getName(){ return name; }
std::list<ExecutionEnvironment*>* Device::getExecutionsEnvironment(){ return &executionsEnvironment; };

void Device::setName(std::string name){ this->name = name; }
