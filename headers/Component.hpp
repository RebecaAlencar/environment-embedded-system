#ifndef COMPONENT_HPP
#define COMPONENT_HPP


#include <string>
#include <list>

class Component {

private:
	std::string name;

	
public:
	Component(std::string);

	std::string getName();
	
	void setName(std::string);
};

#endif