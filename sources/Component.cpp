#include "Component.hpp"

Component::Component(std::string name){
	setName(name);
}

std::string Component::getName(){ return name; }

void Component::setName(std::string name){ this->name = name; }