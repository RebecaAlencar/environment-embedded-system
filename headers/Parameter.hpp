#ifndef PARAMETER_HPP
#define PARAMETER_HPP

#include <string>

class Parameter {

private:
	std::string name;
	std::string type;
	
public:
	Parameter(std::string, std::string);

	std::string getName();
	std::string getType();

	void setName(std::string);
	void setType(std::string);
};

#endif