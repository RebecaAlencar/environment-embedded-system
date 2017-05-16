#include "OutputCProject.hpp"

OutputCProject::OutputCProject(std::string output_dir,
                                std::list<Module*>* modules,
                                std::list<Transaction*>* transactions){
    this->output_dir = output_dir;
    
    boost::filesystem::path dir(output_dir);
    boost::filesystem::create_directory(dir);
    
    std::stringstream header_dir_str;
    header_dir_str << this->output_dir << "/headers";
    boost::filesystem::path headers_dir(header_dir_str.str().c_str());
    boost::filesystem::create_directory(headers_dir);
    
    std::stringstream source_dir_str;
    source_dir_str << this->output_dir << "/sources";
    boost::filesystem::path sources_dir(source_dir_str.str().c_str());
    boost::filesystem::create_directory(sources_dir);
    
    createMainFile(modules, transactions);
    createHeaderFiles(modules);
    createSourceFiles(modules);
}

void OutputCProject::createMainFile(std::list<Module*>* modules,
                                    std::list<Transaction*>* transactions){
    std::stringstream ss;
    ss << this->output_dir << "/main.cpp";
    std::ofstream output(ss.str().c_str());
    
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

void OutputCProject::createHeaderFiles(std::list<Module*>* modules){
    for (std::list<Module*>::iterator it = modules->begin(); it != modules->end(); ++it){
        std::stringstream ss;
        ss << this->output_dir << "/headers/" << (*it)->getName() << ".h";
        std::ofstream output(ss.str().c_str());
        
        std::string moduleName = (*it)->getName();
        transform(moduleName.begin(), moduleName.end(), moduleName.begin(), ::toupper);
        output << "#ifndef " << moduleName << "_H" << std::endl;
        output << "#define " << moduleName << "_H" << std::endl;
        
        output << std::endl;
        
        output << "#include <queue>" << std::endl;
        
        output << std::endl;
        
        output << "class " << (*it)->getName() << " {" << std::endl;
        output << std::endl;
        
        output << "\t" << "public:" << std::endl;
        
        output << "\t\t" << (*it)->getName() << "();" << std::endl;
        output << "\t\t" << "void run();" << std::endl;
        
        for (std::list<Service*>::iterator jt = (*it)->getServices()->begin(); jt != (*it)->getServices()->end(); ++jt){
            output << "\t\t" << "void " << (*jt)->getName() << "_schedule(";
            
            for (std::list<Parameter*>::iterator kt = (*jt)->getParameters()->begin(); kt != (*jt)->getParameters()->end(); ++kt)
                output << (*kt)->getType() << " " << (*kt)->getName() << ", ";

            output << "bool *ret, bool *finished);" << std::endl;
        }
        
        // print services data when it is using a service
        
        output << std::endl;
        
        output << "\t" << "private:" << std::endl;
        
        for (std::list<Service*>::iterator jt = (*it)->getServices()->begin(); jt != (*it)->getServices()->end(); ++jt){
            output << "\t\t" << (*jt)->getType() << " " << (*jt)->getName() << "(";
            
            bool firstParam = true;
            for (std::list<Parameter*>::iterator kt = (*jt)->getParameters()->begin(); kt != (*jt)->getParameters()->end(); ++kt){
                if(!firstParam)
                    output << ", ";
                
                output << (*kt)->getType() << " " << (*kt)->getName();
                
                firstParam = false;
            }

            output << ");" << std::endl;
            
            output << "\t\t" << "std::queue<Data_" << (*it)->getName() << "_" << (*jt)->getName() << "> q_" << (*jt)->getName() << ";" << std::endl;
        }
        
        // print services instances when it is using a service
        
        output << std::endl;
        
        output << "}" << std::endl;
        
        output << std::endl;
        
        output << "#endif" << std::endl;
        
        output.close();
    }
}

void OutputCProject::createSourceFiles(std::list<Module*>* modules){
    for (std::list<Module*>::iterator it = modules->begin(); it != modules->end(); ++it){
        std::stringstream ss;
        ss << this->output_dir << "/sources/" << (*it)->getName() << ".c";
        std::ofstream output(ss.str().c_str());
        
        output << "#include \"" << (*it)->getName() << ".h\"" << std::endl;
        
        output << std::endl;
        
        output << (*it)->getName() << "::" << (*it)->getName() << "(){}" << std::endl;
        output << std::endl;
        
        output << "void " << (*it)->getName() << "::run(){" << std::endl;
        
        // print flow of run
        
        output << "}" << std::endl;
        
        // print services data
        
        output << std::endl;
        
        output.close();
    }
}