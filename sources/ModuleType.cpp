#include "ModuleType.hpp"

ModuleType ModuleTypeFromString(std::string name){
	std::transform(name.begin(), name.end(), name.begin(), ::toupper);

	if(name == "HW" || name == "HARDWARE" || name == "HW_MODULE") return HARDWARE;
	else if(name == "SW" || name == "SOFTWARE" || name == "SW_MODULE") return SOFTWARE;

	return NONE;
}

std::string ModuleTypeToString(ModuleType moduleType){
	if(moduleType == HARDWARE) return "hw_module";
	else if(moduleType == SOFTWARE) return "sw_module";

	return "NONE";
}