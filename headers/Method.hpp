#ifndef METHOD_HPP
#define METHOD_HPP

#include <string>
#include <list>
#include "Modifier.hpp"
#include "Parameter.hpp"

class Method {

private:
	std::string name;
	std::string type;
	bool service;
	Modifier modifier;

	std::list<Parameter*> parameters;
	
public:
	Method(std::string, std::string, Modifier);

	std::string getName();
	std::string getType();
	Modifier getModifier();
	std::list<Parameter*>* getParameters();
	bool getService();

	void setName(std::string);
	void setType(std::string);
	void setModifier(Modifier);
	void setService(bool);
};

#endif