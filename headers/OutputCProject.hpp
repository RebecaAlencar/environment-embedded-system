#ifndef OUTPUT_C_PROJECT_HPP
#define OUTPUT_C_PROJECT_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <list>
#include <boost/filesystem.hpp>

#include "Module.hpp"
#include "Transaction.hpp"

class OutputCProject {

private:
	std::string output_dir;
	void createMainFile(std::list<Module*>*, std::list<Transaction*>*);
	void createConfigFile();
	void createMakefile();
	void createTypes(std::ofstream&, Module*);
	void createHeaderFiles(std::list<Module*>*, std::list<Transaction*>*);
	void createSourceFiles(std::list<Module*>*, std::list<Transaction*>*);
	
public:
	OutputCProject(std::string, std::list<Module*>*, std::list<Transaction*>*);

};

#endif