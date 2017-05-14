#ifndef MODIFIER_HPP
#define MODIFIER_HPP

#include <string>
#include <algorithm>

enum Modifier {
	PRIVATE,
	PROTECTED,
	PUBLIC
};

Modifier ModifierFromString(std::string);
#endif