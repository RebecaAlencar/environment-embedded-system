#ifndef RESTRICTION_HPP
#define RESTRICTION_HPP


#include <string>

class Restriction {

private:
	std::string name;
	std::string value;
	
public:
	Restriction(std::string, std::string);

	std::string getName();
	std::string getValue();

	void setName(std::string);
	void setValue(std::string);
};

#endif