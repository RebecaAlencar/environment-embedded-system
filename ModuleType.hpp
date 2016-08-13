#ifndef MODULETYPE_HPP
#define MODULETYPE_HPP

#include <string>
#include <algorithm>

enum ModuleType {
	NONE,

	HARDWARE,
	SOFTWARE
};

ModuleType ModuleTypeFromString(std::string);
std::string ModuleTypeToString(ModuleType);

#endif