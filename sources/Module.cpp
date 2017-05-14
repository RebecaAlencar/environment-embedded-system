#include "Module.hpp"

Module::Module(std::string name, ModuleType moduleType){
	setName(name);
	setModuleType(moduleType);
}

std::string Module::getName(){ return name; }
ModuleType Module::getModuleType(){ return moduleType; }
std::list<Attribute*>* Module::getAttributes(){ return &attributes; };
std::list<Method*>* Module::getMethods(){ return &methods; };
std::list<Service*>* Module::getServices(){ return &services; };
std::list<Element*>* Module::getInstances(){ return &instances; };

void Module::setName(std::string name){ this->name = name; }
void Module::setModuleType(ModuleType moduleType){ this->moduleType = moduleType; }