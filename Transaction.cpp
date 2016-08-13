#include "Transaction.hpp"

Transaction::Transaction(std::string name_callee, Module* caller, Module* callee, Method* service, std::string order, TransactionType transactionType){
	setNameCallee(name_callee);
	setCaller(caller);
	setCallee(callee);
	setService(service);
	setOrder(order);
	setTransactionType(transactionType);
	//setSelfCall(selfCall);
}

std::list<std::string*>* Transaction::getNameCaller(){ return &name_caller; }
std::string Transaction::getNameCallee(){ return name_callee; }
Module* Transaction::getCaller(){ return caller; }
Method* Transaction::getService(){ return service; }
Module* Transaction::getCallee(){ return callee; }
std::string Transaction::getOrder(){ return order; }
TransactionType Transaction::getTransactionType(){ return transactionType; }
//bool Transaction::getSelfCall(){return selfCall;}



void Transaction::setNameCallee(std::string name_callee){ this->name_callee = name_callee; }
void Transaction::setCaller(Module* caller){ this->caller = caller; }
void Transaction::setCallee(Module* callee){ this->callee = callee; }
void Transaction::setService(Method* service){ this->service = service; }
void Transaction::setOrder(std::string order){ this->order = order; };
void Transaction::setTransactionType(TransactionType transactionType){ this->transactionType = transactionType; };
//void Transaction::setSelfCall(bool self){this->selfCall = self;};