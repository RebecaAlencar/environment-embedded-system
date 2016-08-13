#include "Method.hpp"

Method::Method(std::string name, std::string type, Modifier modifier){
	setName(name);
	setType(type);
	setModifier(modifier);
}

std::string Method::getName(){ return name; }
std::string Method::getType(){ return type; }
Modifier Method::getModifier(){ return modifier; }
std::list<Parameter*>* Method::getParameters(){ return &parameters; };
bool Method::getService(){return service;}

void Method::setName(std::string name){ this->name = name; }
void Method::setType(std::string type){ this->type = type; }
void Method::setModifier(Modifier type){ this->modifier = type; }
void Method::setService(bool service){this->service = service;}