#include "FrameType.hpp"

FrameType FrameTypeFromString(std::string name){
	std::transform(name.begin(), name.end(), name.begin(), ::toupper);

	if(name == "SEQ" || name == "seq") return SEQ;
	else if(name == "PAR" || name == "par") return PAR;

	return LOOP;
}