#ifndef ENTRY_HPP
#define ENTRY_HPP

#include "Module.hpp"
#include "Transaction.hpp"
#include <vector>

class Entry {

private:
	Module* module;
	std::string moduleInstance;
	
	Module* exitModule;
	std::string exitModuleInstance;
	
	Method* method;
	std::list<Transaction*> transactions;
	
public:
	Entry();

	Module* getModule();
	std::string getModuleInstance();
	
	Module* getExitModule();
	std::string getExitModuleInstance();
	
	Method* getMethod();
	std::list<Transaction*>* getTransactions();
	
	void setModule(Module*);
	void setModuleInstance(std::string);
	
	void setExitModule(Module*);
	void setExitModuleInstance(std::string);
	
	void setMethod(Method*);

};

#endif