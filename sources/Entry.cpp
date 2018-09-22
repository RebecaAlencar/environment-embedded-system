#include "Entry.hpp"

Entry::Entry(){
	setModule(NULL);
}

Module* Entry::getModule(){ return module; }
std::string Entry::getModuleInstance(){ return moduleInstance; }

Module* Entry::getExitModule(){ return exitModule; }
std::string Entry::getExitModuleInstance(){ return exitModuleInstance; }

Method* Entry::getMethod(){ return method; }
std::list<Transaction*>* Entry::getTransactions(){ return &transactions; };

void Entry::setModule(Module* module){ this->module = module; }
void Entry::setModuleInstance(std::string moduleInstance){ this->moduleInstance = moduleInstance; }

void Entry::setExitModule(Module* exitModule){ this->exitModule = exitModule; }
void Entry::setExitModuleInstance(std::string exitModuleInstance){ this->exitModuleInstance = exitModuleInstance; }

void Entry::setMethod(Method* method){ this->method = method; }