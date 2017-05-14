#include "Modifier.hpp"

Modifier ModifierFromString(std::string name){
	std::transform(name.begin(), name.end(), name.begin(), ::toupper);

	if(name == "PRIVATE") return PRIVATE;
	else if(name == "PROTECTED") return PROTECTED;

	return PUBLIC;
}