#ifndef ELEMENT_HPP
#define ELEMENT_HPP

#include <string>
#include <list>
#include "Restriction.hpp"

class Element {

private:
	std::string name;
	std::string value;

	std::list<Restriction*> restrictions;
	
public:
	Element(std::string, std::string);

	std::string getName();
	std::string getValue();
	std::list<Restriction*>* getRestrictions();

	void setName(std::string);
	void setValue(std::string);
};

#endif