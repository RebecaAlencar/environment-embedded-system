#include "Artifact.hpp"

Artifact ArtifactFromString(std::string name){
	std::transform(name.begin(), name.end(), name.begin(), ::toupper);

	if(name == "FIFO" || name == "fifo") return FIFO;
	else if(name == "ROUND-ROBIN" || name == "Round-robin") return ROUNDROBIN;
	else return CUSTOM;
}

std::string ArtifactFromString(Artifact artifact){
	if(artifact == FIFO) return "FIFO";
	else if(artifact == ROUNDROBIN) return "ROUND-ROBIN";
	else return "CUSTOM";
}