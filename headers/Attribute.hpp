#ifndef ATTRIBUTE_HPP
#define ATTRIBUTE_HPP

#include <string>
#include "Modifier.hpp"

class Attribute {

private:
	std::string name;
	std::string type;
	Modifier modifier;
	
public:
	Attribute(std::string, std::string, Modifier);

	std::string getName();
	std::string getType();
	Modifier getModifier();

	void setName(std::string);
	void setType(std::string);
	void setModifier(Modifier);
};

#endif