#ifndef OUTPUT_C_PROJECT_HPP
#define OUTPUT_C_PROJECT_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <list>
#include <map>
#include <boost/filesystem.hpp>

#include "Module.hpp"
#include "Transaction.hpp"
#include "Frame.hpp"

class OutputCProject {

private:
	std::string output_dir;
	void createMainFile(std::list<Module*>*, std::list<Transaction*>*, std::list<Frame*>*);
	void createMakefile();
	void createTypes(std::ofstream&, Module*);
	void createHeaderFiles(std::list<Module*>*, std::list<Transaction*>*, std::list<Frame*>*);
	void createSystemSourceFiles(std::list<Module*>*, std::list<Transaction*>*, std::list<Frame*>*);
	void createUserSourceFiles(std::list<Module*>*, std::list<Transaction*>*, std::list<Frame*>*);
	
	std::list<Entry*> getExits(Module*, Frame*, std::list<Method*>&);
	
	int numberOfFrames;
	int unique = 0;
	void calculateNumberOfFrames(Frame*);
	void createMainFile_createEntryIds(std::ofstream&, Frame*);
	void createMainFile_createEntryIsSeq(std::ofstream&, Frame*, int);
	void createMainFile_createEntryStructure(std::ofstream&, Frame*, int);
	void createHeaderFiles_createEntryPoints(std::ofstream&, Module*, Frame*, std::list<Method*>&, std::list<Method*>&);
	void createSystemSourceFiles_initiateEntrySCMethods(std::ofstream&, Module*, Frame*, std::list<Method*>&);
	void createSystemSourceFiles_createEntryPoints(std::ofstream&, Module*, Frame*, int, std::string, std::list<Method*>&);
	void createUserSourceFiles_createEntryPoints(std::ofstream&, Module*, Frame*, int, std::string, std::list<Method*>&);
	
public:
	OutputCProject(std::string, std::list<Module*>*, std::list<Transaction*>*, std::list<Frame*>*);

};

#endif