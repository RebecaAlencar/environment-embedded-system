#include "Parameter.hpp"

Parameter::Parameter(std::string name, std::string type){
	setName(name);
	setType(type);
}

std::string Parameter::getName(){ return name; }
std::string Parameter::getType(){ return type; }

void Parameter::setName(std::string name){ this->name = name; }
void Parameter::setType(std::string type){ this->type = type; }