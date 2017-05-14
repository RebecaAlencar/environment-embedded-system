#ifndef FRAMETYPE_HPP
#define FRAMETYPE_HPP

#include <string>
#include <algorithm>

enum FrameType {
	SEQ,

	PAR,
	LOOP
};

FrameType FrameTypeFromString(std::string);

#endif