#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include "Module.hpp"
#include "Method.hpp"
#include "TransactionType.hpp"
#include <vector>

class Transaction {

private:
	std::list<std::string*> name_caller;
	std::string name_callee;
	Module* caller;
	Module* callee;
	Method* service;
	std::string order;
	TransactionType transactionType;
	// bool selfCall;
public:
	Transaction(std::string, Module*, Module*, Method*, std::string, TransactionType);

	Module* getCaller();
	Module* getCallee();
	Method* getService();
	std::string getOrder();
	TransactionType getTransactionType();
	std::list<std::string*>* getNameCaller();
    std::string getNameCallee();
    // bool getSelfCall();

	void setNameCallee(std::string);
	void setCaller(Module*);
	void setCallee(Module*);
	void setService(Method*);
	void setOrder(std::string);
	void setTransactionType(TransactionType);
	// void setSelfCall(bool self); 

};

#endif