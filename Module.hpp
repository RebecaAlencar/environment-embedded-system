#ifndef MODULE_HPP
#define MODULE_HPP

#include <string>
#include <list>
#include "ModuleType.hpp"
#include "Attribute.hpp"
#include "Method.hpp"
#include "Service.hpp"
#include "Element.hpp"

class Module {

private:
	std::string name;
	ModuleType moduleType;
	
	std::list<Attribute*> attributes;
	std::list<Method*> methods;
	std::list<Service*> services;
	std::list<Element*> instances;

public:
	Module(std::string, ModuleType);

	std::string getName();
	ModuleType getModuleType();
	std::list<Attribute*>* getAttributes();
	std::list<Method*>* getMethods();
	std::list<Service*>* getServices();
	std::list<Element*>* getInstances();

	void setName(std::string);
	void setModuleType(ModuleType);
};

#endif