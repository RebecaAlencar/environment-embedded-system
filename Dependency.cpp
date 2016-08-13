#include "Dependency.hpp"

Dependency::Dependency(Element* caller, Element* callee, Element* transaction){
	setCaller(caller);
	setCallee(callee);
	setTransaction(transaction);
}

Element* Dependency::getCaller(){ return caller; }
Element* Dependency::getCallee(){ return callee; }
Element* Dependency::getTransaction(){ return transaction; }

void Dependency::setCaller(Element* caller){ this->caller = caller; }
void Dependency::setCallee(Element* callee){ this->callee = callee; }
void Dependency::setTransaction(Element* transaction){ this->transaction = transaction; }