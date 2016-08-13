#include "ExecutionEnvironment.hpp"

ExecutionEnvironment::ExecutionEnvironment(std::string name,Artifact artifact){
	setName(name);
	setArtifact(artifact);
}

std::string ExecutionEnvironment::getName(){ return name; }
Artifact ExecutionEnvironment::getArtifact(){return artifact;}
std::list<Component*>* ExecutionEnvironment::getComponents(){ return &components; };

void ExecutionEnvironment::setName(std::string name){ this->name = name; }
void ExecutionEnvironment::setArtifact(Artifact artifact){ this->artifact = artifact; }