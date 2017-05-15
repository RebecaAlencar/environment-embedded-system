#ifndef OUTPUT_C_PROJECT_HPP
#define OUTPUT_C_PROJECT_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <list>

#include "Module.hpp"
#include "Transaction.hpp"

class OutputCProject {

private:
	std::string output_dir;
	void createMainFile(std::list<Module*>*, std::list<Transaction*>*);
	void createHeaderFiles();
	void createSourceFiles();
	
public:
	OutputCProject(std::string, std::list<Module*>*, std::list<Transaction*>*);

};

#endif