#ifndef EXECUTIONENVIRONMENT_HPP
#define EXECUTIONENVIRONMENT_HPP


#include <string>
#include <list>
#include "Component.hpp"
#include "Artifact.hpp"

class ExecutionEnvironment {

private:
	std::string name;
	Artifact artifact;
	std::list<Component*> components;
	
public:
	ExecutionEnvironment(std::string, Artifact artifact);
	Artifact getArtifact();
	std::string getName();
	std::list<Component*>* getComponents();

	void setName(std::string);
	void setArtifact(Artifact);
	
};

#endif