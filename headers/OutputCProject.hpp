#ifndef OUTPUT_C_PROJECT_HPP
#define OUTPUT_C_PROJECT_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <boost/filesystem.hpp>

#include "Module.hpp"
#include "Transaction.hpp"

class OutputCProject {

private:
	std::string output_dir;
	void createMainFile(std::list<Module*>*, std::list<Transaction*>*);
	void createHeaderFiles(std::list<Module*>*);
	void createSourceFiles(std::list<Module*>*);
	
public:
	OutputCProject(std::string, std::list<Module*>*, std::list<Transaction*>*);

};

#endif