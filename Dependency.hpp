#ifndef DEPENDENCY_HPP
#define DEPENDENCY_HPP

#include "Element.hpp"

class Dependency {

private:
	Element* caller;
	Element* callee;
	Element* transaction;

public:
	Dependency(Element*, Element*, Element*);

	Element* getCaller();
	Element* getCallee();
	Element* getTransaction();

	void setCaller(Element*);
	void setCallee(Element*);
	void setTransaction(Element*);
};

#endif

