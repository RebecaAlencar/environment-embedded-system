#include "Restriction.hpp"

Restriction::Restriction(std::string name, std::string value){
	setName(name);
	setValue(value);
}

std::string Restriction::getName(){ return name; }
std::string Restriction::getValue(){ return value; }

void Restriction::setName(std::string name){ this->name = name; }
void Restriction::setValue(std::string value){ this->value = value; }