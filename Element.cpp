#include "Element.hpp"

Element::Element(std::string name, std::string value){
	setName(name);
	setValue(value);
}

std::string Element::getName(){ return name; }
std::string Element::getValue(){ return value; }
std::list<Restriction*>* Element::getRestrictions(){ return &restrictions; };

void Element::setName(std::string name){ this->name = name; }
void Element::setValue(std::string value){ this->value = value; }