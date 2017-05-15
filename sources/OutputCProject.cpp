#include "OutputCProject.hpp"

OutputCProject::OutputCProject(std::string output_dir,
                                std::list<Module*>* modules,
                                std::list<Transaction*>* transactions){
    this->output_dir = output_dir;
    
    createMainFile(modules, transactions);
}

void OutputCProject::createMainFile(std::list<Module*>* modules,
                                    std::list<Transaction*>* transactions){
    std::ofstream output("output/main.cpp");
    
    output << "#include <omp.h>" << std::endl;
    
    output << std::endl;
    
    for (std::list<Module*>::iterator it = modules->begin(); it != modules->end(); ++it)
        output << "#include \"" << ((*it)->getName()).c_str() << ".h\"" << std::endl;
    
    output << std::endl;
    
    output << "int main(int argc, char* argv[]) {" << std::endl;
    
    output << std::endl;
    
    for (std::list<Module*>::iterator it = modules->begin(); it != modules->end(); ++it)
        for (std::list<Element*>::iterator jt = (*it)->getInstances()->begin(); jt != (*it)->getInstances()->end(); ++jt)
            output << "\t" << ((*jt)->getValue()).c_str() << " " << ((*jt)->getName()).c_str() << ";" << std::endl;
            
    output << std::endl;
    
    for (std::list<Transaction*>::iterator it = transactions->begin(); it != transactions->end(); ++it)
        for (std::list<std::string*>::iterator jt = ((*it)->getNameCaller())->begin(); jt != ((*it)->getNameCaller())->end(); ++jt)
		  output << "\t" << **jt << ".setServiceInstance1(&" << (*it)->getNameCallee() << ");" << std::endl;
    
    output << std::endl;

    output << "\t" << "#pragma omp parallel sections" << std::endl; 
    output << "\t" << "{" << std::endl; 
    for (std::list<Module*>::iterator it = modules->begin(); it != modules->end(); ++it)
        for (std::list<Element*>::iterator jt = (*it)->getInstances()->begin(); jt != (*it)->getInstances()->end(); ++jt){
            output << "\t" << "\t" << "#pragma omp section" << std::endl; 
            output << "\t" << "\t" << "{ " << ((*jt)->getName()).c_str() << ".run(); }" << std::endl;
        }
    output << "\t" << "}" << std::endl; 
     
    output << std::endl;
    
    output << "\t" << "return 0;" << std::endl;
    output << "}" << std::endl;
    
    output.close();
}

void OutputCProject::createHeaderFiles(){

}

void OutputCProject::createSourceFiles(){

}