#include "Attribute.hpp"

Attribute::Attribute(std::string name, std::string type, Modifier modifier){
	setName(name);
	setType(type);
	setModifier(modifier);
}

std::string Attribute::getName(){ return name; }
std::string Attribute::getType(){ return type; }
Modifier Attribute::getModifier(){ return modifier; }

void Attribute::setName(std::string name){ this->name = name; }
void Attribute::setType(std::string type){ this->type = type; }
void Attribute::setModifier(Modifier type){ this->modifier = type; }