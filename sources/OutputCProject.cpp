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
    
    createConfigFile();
    createMainFile(modules, transactions);
    createHeaderFiles(modules, transactions);
    createSourceFiles(modules, transactions);
}

void OutputCProject::createMainFile(std::list<Module*>* modules,
                                    std::list<Transaction*>* transactions){
    std::stringstream ss;
    ss << this->output_dir << "/main.cpp";
    std::ofstream output(ss.str().c_str());
    
    output << "#include <omp.h>" << std::endl;
    
    output << std::endl;
    
    output << "#include \"config.hpp\"" << std::endl;
    
    output << std::endl;
    
    for (std::list<Module*>::iterator it = modules->begin(); it != modules->end(); ++it)
        output << "#include \"" << ((*it)->getName()).c_str() << ".hpp\"" << std::endl;
    
    output << std::endl;
    
    output << "int main(int argc, char* argv[]) {" << std::endl;
    
    output << std::endl;
    
    for (std::list<Module*>::iterator it = modules->begin(); it != modules->end(); ++it)
        for (std::list<Element*>::iterator jt = (*it)->getInstances()->begin(); jt != (*it)->getInstances()->end(); ++jt)
            output << "\t" << ((*jt)->getValue()).c_str() << " " << ((*jt)->getName()).c_str() << ";" << std::endl;
            
    output << std::endl;

    output << "\t" << "#pragma omp parallel sections" << std::endl; 
    output << "\t" << "{" << std::endl; 
    for (std::list<Module*>::iterator it = modules->begin(); it != modules->end(); ++it)
        for (std::list<Element*>::iterator mt = (*it)->getInstances()->begin(); mt != (*it)->getInstances()->end(); ++mt){
            output << "\t" << "\t" << "#pragma omp section" << std::endl; 
            output << "\t" << "\t" << "{ ";
            
            output << (*it)->getName() << "::run_" << (*mt)->getName() << "(&" << (*mt)->getName();
                     
            for (std::list<Transaction*>::iterator nt = transactions->begin(); nt != transactions->end(); ++nt)
                if((*nt)->getCaller() == *it){
                    // check if the current instance uses any service
                    bool useInstance = false;
                    for (std::list<std::string*>::iterator ot = (*nt)->getNameCaller()->begin(); ot != (*nt)->getNameCaller()->end(); ++ot)
                        if(!(*ot)->compare((*mt)->getName()))
                            useInstance = true;
                        
                    if(useInstance)
                        output << ", &" << (*nt)->getNameCallee();   
                }
                
            output << "); }" << std::endl;
        }
    output << "\t" << "}" << std::endl; 
     
    output << std::endl;
    
    output << "\t" << "return 0;" << std::endl;
    output << "}" << std::endl;
    
    output.close();
}

void OutputCProject::createConfigFile(){
    std::stringstream ss;
    ss << this->output_dir << "/headers/config.hpp";
    std::ofstream output(ss.str().c_str());
    
    output << "#include <cstdio>" << std::endl;
    output << "#include <cstdlib>" << std::endl;
    
    output << std::endl;
    
    output.close();
}

void OutputCProject::createTypes(std::ofstream &output, Module *module){
    /*
     * Create data types for encapsulating service call
     */
    for (std::list<Service*>::iterator jt = module->getServices()->begin(); jt != module->getServices()->end(); ++jt){
        output << "typedef struct {" << std::endl;
        
        for (std::list<Parameter*>::iterator kt = (*jt)->getParameters()->begin(); kt != (*jt)->getParameters()->end(); ++kt)
            output << "\t" << (*kt)->getType() << " " << (*kt)->getName() << ";" << std::endl;
            
        output << "\t" << (*jt)->getType() << "* ret;" << std::endl;
        output << "\t" << "bool* finished;" << std::endl;

        output << "} " << "Data_" << module->getName() << "_" << (*jt)->getName() << ";" << std::endl;
    }
}

void OutputCProject::createHeaderFiles(std::list<Module*>* modules, std::list<Transaction*>* transactions){
    for (std::list<Module*>::iterator it = modules->begin(); it != modules->end(); ++it){
        std::stringstream ss;
        ss << this->output_dir << "/headers/" << (*it)->getName() << ".hpp";
        std::ofstream output(ss.str().c_str());
        
        std::string moduleName = (*it)->getName();
        transform(moduleName.begin(), moduleName.end(), moduleName.begin(), ::toupper);
        output << "#ifndef " << moduleName << "_H" << std::endl;
        output << "#define " << moduleName << "_H" << std::endl;
        
        output << std::endl;
        
        output << "#include \"config.hpp\"" << std::endl;
        output << "#include <queue>" << std::endl;
        
        output << std::endl;
        
        // Include dependencies
        for (std::list<Element*>::iterator mt = (*it)->getInstances()->begin(); mt != (*it)->getInstances()->end(); ++mt){
            for (std::list<Transaction*>::iterator nt = transactions->begin(); nt != transactions->end(); ++nt)
                if((*nt)->getCaller() == *it){
                    // check if the current instance uses any service
                    bool useInstance = false;
                    for (std::list<std::string*>::iterator ot = (*nt)->getNameCaller()->begin(); ot != (*nt)->getNameCaller()->end(); ++ot)
                        if(!(*ot)->compare((*mt)->getName()))
                            useInstance = true;
                        
                    if(useInstance)
                        output << "#include \"" << (*nt)->getCallee()->getName() << ".hpp\"" << std::endl;   
                }
        }
        output << std::endl;
        
        this->createTypes(output, *it);
        
        output << std::endl;
        
        output << "class " << (*it)->getName() << " {" << std::endl;
        output << std::endl;
        
        output << "\t" << "public:" << std::endl;
        
        output << "\t\t" << (*it)->getName() << "();" << std::endl;
        
        for (std::list<Service*>::iterator jt = (*it)->getServices()->begin(); jt != (*it)->getServices()->end(); ++jt){
            // Schedule function
            output << "\t\t" << "void " << (*jt)->getName() << "_schedule(";
            
            for (std::list<Parameter*>::iterator kt = (*jt)->getParameters()->begin(); kt != (*jt)->getParameters()->end(); ++kt)
                output << (*kt)->getType() << " " << (*kt)->getName() << ", ";

            output << (*jt)->getType() << " *ret, bool *finished);" << std::endl;
            
            // Sync function
            output << "\t\t" << (*jt)->getType() << " " << (*jt)->getName() << "_sync(";
            
            bool first = true;
            for (std::list<Parameter*>::iterator kt = (*jt)->getParameters()->begin(); kt != (*jt)->getParameters()->end(); ++kt){
                if(!first)
                    output << ", ";
                output << (*kt)->getType() << " " << (*kt)->getName();
                
                first = false;
            }

            output << ");" << std::endl;
            
            // Async function
            output << "\t\t" << "void " << (*jt)->getName() << "_async(";
            
            for (std::list<Parameter*>::iterator kt = (*jt)->getParameters()->begin(); kt != (*jt)->getParameters()->end(); ++kt)
                output << (*kt)->getType() << " " << (*kt)->getName() << ", ";

            output << (*jt)->getType() << " *ret);" << std::endl;
            
            output << std::endl;
        }
        
        output << std::endl;
        
        // Run methods for each instance
        for (std::list<Element*>::iterator mt = (*it)->getInstances()->begin(); mt != (*it)->getInstances()->end(); ++mt){
            output << "\t\t" << "static void run_" << (*mt)->getName() << "(" << (*it)->getName() << "* instance";
            
            for (std::list<Transaction*>::iterator nt = transactions->begin(); nt != transactions->end(); ++nt)
                if((*nt)->getCaller() == *it){
                    // check if the current instance uses any service
                    bool useInstance = false;
                    for (std::list<std::string*>::iterator ot = (*nt)->getNameCaller()->begin(); ot != (*nt)->getNameCaller()->end(); ++ot)
                        if(!(*ot)->compare((*mt)->getName()))
                            useInstance = true;
                        
                    if(useInstance)
                        output << ", " << (*nt)->getCallee()->getName() << "* " << (*nt)->getNameCallee();   
                }
                
            output << ");" << std::endl;
        }

        output << std::endl;
        
        output << "\t" << "private:" << std::endl;
        
        output << "\t\t" << "void run();" << std::endl;
        
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
            
            output << "\t\t" << "void " << (*jt)->getName() << "_run();" << std::endl;
            
            output << "\t\t" << "std::queue<Data_" << (*it)->getName() << "_" << (*jt)->getName() << "> q_" << (*jt)->getName() << ";" << std::endl;
        }
        
        // print services instances when it is using a service
        
        output << std::endl;
        
        output << "};" << std::endl;
        
        output << std::endl;
        
        output << "#endif" << std::endl;
        
        output.close();
    }
}

void OutputCProject::createSourceFiles(std::list<Module*>* modules, std::list<Transaction*>* transactions){
    for (std::list<Module*>::iterator it = modules->begin(); it != modules->end(); ++it){
        std::stringstream ss;
        ss << this->output_dir << "/sources/" << (*it)->getName() << ".cpp";
        std::ofstream output(ss.str().c_str());
        
        output << "#include \"" << (*it)->getName() << ".hpp\"" << std::endl;
        
        output << std::endl;
        
        output << (*it)->getName() << "::" << (*it)->getName() << "(){}" << std::endl;
        output << std::endl;
        
        output << "void " << (*it)->getName() << "::run(){" << std::endl;
        
        //output << "\t" << "while(true){" << std::endl;
        
        /*
         * Service call
         */
        /*for (std::list<Service*>::iterator jt = (*it)->getServices()->begin(); jt != (*it)->getServices()->end(); ++jt){
            output << "\t\t" << (*jt)->getName() << "_run();" << std::endl;
        }*/

        //output << "\t" << "}" << std::endl;
        
        output << "}" << std::endl;
        
        output << std::endl;
        
        // Run methods for each instance
        for (std::list<Element*>::iterator mt = (*it)->getInstances()->begin(); mt != (*it)->getInstances()->end(); ++mt){
            output << "void " << (*it)->getName() << "::run_" << (*mt)->getName() << "(" << (*it)->getName() << "* instance";
            
            for (std::list<Transaction*>::iterator nt = transactions->begin(); nt != transactions->end(); ++nt)
                if((*nt)->getCaller() == *it){
                    // check if the current instance uses any service
                    bool useInstance = false;
                    for (std::list<std::string*>::iterator ot = (*nt)->getNameCaller()->begin(); ot != (*nt)->getNameCaller()->end(); ++ot)
                        if(!(*ot)->compare((*mt)->getName()))
                            useInstance = true;
                        
                    if(useInstance)
                        output << ", " << (*nt)->getCallee()->getName() << "* " << (*nt)->getNameCallee();   
                }
                
            output << "){" << std::endl;
            
            output << "\t" << "while(true){" << std::endl;
            
            for (std::list<Transaction*>::iterator nt = transactions->begin(); nt != transactions->end(); ++nt)
                if((*nt)->getCaller() == *it){
                    // check if the current instance uses any service
                    bool useInstance = false;
                    for (std::list<std::string*>::iterator ot = (*nt)->getNameCaller()->begin(); ot != (*nt)->getNameCaller()->end(); ++ot)
                        if(!(*ot)->compare((*mt)->getName()))
                            useInstance = true;
                        
                    if(useInstance){
                        if((*nt)->getTransactionType() == SYNC)
                            output << "//\t\t" << (*nt)->getNameCallee() << "->" << (*nt)->getService()->getName() << "_sync();" << std::endl;  
                        else if((*nt)->getTransactionType() == ASYNC)
                            output << "//\t\t" << (*nt)->getNameCallee() << "->" << (*nt)->getService()->getName() << "_async();" << std::endl; 
                    } 
                }
            
            output << "\t" << "}" << std::endl;
            
            output << "}" << std::endl;
        }
        
        /*
         * Services offered - service function
         */
        for (std::list<Service*>::iterator jt = (*it)->getServices()->begin(); jt != (*it)->getServices()->end(); ++jt){
            output << (*jt)->getType() << " " << (*it)->getName() << "::" <<  (*jt)->getName() << "(";
            
            bool firstParam = true;
            for (std::list<Parameter*>::iterator kt = (*jt)->getParameters()->begin(); kt != (*jt)->getParameters()->end(); ++kt){
                if(!firstParam)
                    output << ", ";
                
                output << (*kt)->getType() << " " << (*kt)->getName();
                
                firstParam = false;
            }

            output << "){" << std::endl;
            
            // service function
            
            output << "}" << std::endl;
        }
        
        output << std::endl;
        
        /*
         * Services offered - schedule, sync, async & run
         */
        for (std::list<Service*>::iterator jt = (*it)->getServices()->begin(); jt != (*it)->getServices()->end(); ++jt){
            /*
             * Schedule function
             */
            output << "void " << (*it)->getName() << "::" << (*jt)->getName() << "_schedule(";
            
            for (std::list<Parameter*>::iterator kt = (*jt)->getParameters()->begin(); kt != (*jt)->getParameters()->end(); ++kt)
                output << (*kt)->getType() << " " << (*kt)->getName() << ", ";

            output << (*jt)->getType() << " *ret, bool *finished){" << std::endl;
            
            output << "\t" << "Data_" << (*it)->getName() << "_" << (*jt)->getName() << " data;" << std::endl;
            
            
            for (std::list<Parameter*>::iterator kt = (*jt)->getParameters()->begin(); kt != (*jt)->getParameters()->end(); ++kt)
                output << "\t" << "data." << (*kt)->getName() << " = " << (*kt)->getName() << ";" << std::endl;
                
            output << "\t" << "data.ret = ret;" << std::endl;
            output << "\t" << "data.finished = finished;" << std::endl;
            
            output << std::endl;
          
        	output << "\t" << "#pragma omp critical" << std::endl;
        	output << "\t" << "q_" << (*jt)->getName() << ".push(data);" << std::endl;
            
            output << "}" << std::endl;
            
            output << std::endl;
            
            /*
             * Sync function
             */
            output << (*jt)->getType() << " " << (*it)->getName() << "::" << (*jt)->getName() << "_sync(";
            
            bool first = true;
            for (std::list<Parameter*>::iterator kt = (*jt)->getParameters()->begin(); kt != (*jt)->getParameters()->end(); ++kt){
                if(!first)
                    output << ",";
                output << (*kt)->getType() << " " << (*kt)->getName();
                first = false;
            }

            output << "){" << std::endl;

            output << "\t" << (*jt)->getType() << " ret;" << std::endl;
            output << "\t" << "bool finished;" << std::endl;
            
            output << std::endl;
            
            output << "\t" << "this->" << (*jt)->getName() << "_schedule(";
            for (std::list<Parameter*>::iterator kt = (*jt)->getParameters()->begin(); kt != (*jt)->getParameters()->end(); ++kt){
                output << (*kt)->getName() << ", ";
            }
            
            output << "&ret, &finished);" << std::endl;
            
            output << std::endl;
            
            output << "\t" << "return ret;" << std::endl;
            
            output << std::endl;
          
            output << "}" << std::endl;
            
            output << std::endl;
            
            /*
             * Async function
             */
            output << "void " << (*it)->getName() << "::" << (*jt)->getName() << "_async(";
            
            for (std::list<Parameter*>::iterator kt = (*jt)->getParameters()->begin(); kt != (*jt)->getParameters()->end(); ++kt)
                output << (*kt)->getType() << " " << (*kt)->getName() << ", ";

            output << (*jt)->getType() << "* ret){" << std::endl;

            output << "\t" << "bool finished;" << std::endl;
            
            output << std::endl;
            
            output << "\t" << "this->" << (*jt)->getName() << "_schedule(";
            for (std::list<Parameter*>::iterator kt = (*jt)->getParameters()->begin(); kt != (*jt)->getParameters()->end(); ++kt){
                output << (*kt)->getName() << ", ";
            }
            
            output << "ret, &finished);";
            
            output << std::endl;
          
            output << "}" << std::endl;
            
            output << std::endl;
            
            /*
             * Run function
             */
            
            output << "void " << (*it)->getName() << "::" << (*jt)->getName() << "_run(){" << std::endl;
             
            output << "\t" << "if(!" << "q_" << (*jt)->getName() << ".empty()){" << std::endl;
				
			output << "\t\t" << "Data_" << (*it)->getName() << "_" << (*jt)->getName() << " data = q_" << (*jt)->getName() << ".front();" << std::endl;
      
            output << std::endl;
      
			output << "\t\t" << "#pragma omp critical" << std::endl;
			output << "\t\t" << "q_" << (*jt)->getName() << ".pop();" << std::endl;
			
			output << std::endl;

			output << "\t\t" << "*(data.ret) = " << (*jt)->getName() << "(";
			
			bool firstParam = true;
            for (std::list<Parameter*>::iterator kt = (*jt)->getParameters()->begin(); kt != (*jt)->getParameters()->end(); ++kt){
                if(!firstParam)
                    output << ", ";
                
                output << "data." << (*kt)->getName();
                
                firstParam = false;
            }
            
            output << ");" << std::endl;
			
			output << std::endl;
      
			output << "\t\t" << "*(data.finished) = true;" << std::endl;
            output << "\t" << "}" << std::endl;
            
            output << "}" << std::endl;
            
            output << std::endl;
        }
        
        // print services data
        
        output << std::endl;
        
        output.close();
    }
}