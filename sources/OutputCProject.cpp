#include "OutputCProject.hpp"

OutputCProject::OutputCProject(std::string output_dir,
                                std::list<Module*>* modules,
                                std::list<Transaction*>* transactions,
							  	std::list<Frame*>* frames){
	numberOfFrames = frames->size();
	for (std::list<Frame*>::iterator jt = frames->begin(); jt != frames->end(); ++jt){
		calculateNumberOfFrames(*jt);
	}
	
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
    
    std::stringstream user_dir_str;
    user_dir_str << this->output_dir << "/user";
    boost::filesystem::path user_dir(user_dir_str.str().c_str());
    boost::filesystem::create_directory(user_dir);
	
    createMakefile();

    createMainFile(modules, transactions, frames);
    createHeaderFiles(modules, transactions, frames);
    createUserSourceFiles(modules, transactions, frames);
    createSystemSourceFiles(modules, transactions, frames);
}

std::list<Entry*> OutputCProject::getExits(Module* module, Frame* frame, std::list<Method*> &configuratedMethods){
	std::list<Entry*> exits;
	
	for (std::list<Entry*>::iterator it = frame->getEntries()->begin(); it != frame->getEntries()->end(); ++it){
		if (module == (*it)->getExitModule() && std::find(configuratedMethods.begin(), configuratedMethods.end(), (*it)->getMethod()) == configuratedMethods.end()){
			exits.push_back(*it);	
			
			configuratedMethods.push_back((*it)->getMethod());
		}
	}
	
	for (std::list<Frame*>::iterator jt = frame->getFrames()->begin(); jt != frame->getFrames()->end(); ++jt){
		std::list<Entry*> subexits = getExits(module, *jt, configuratedMethods);
		
		exits.insert(exits.end(), subexits.begin(), subexits.end());
	}
	
	return exits;
}

void OutputCProject::calculateNumberOfFrames(Frame* frame){
	numberOfFrames += frame->getFrames()->size();
	for (std::list<Frame*>::iterator jt = frame->getFrames()->begin(); jt != frame->getFrames()->end(); ++jt){
		 calculateNumberOfFrames(*jt);
	}
}

void OutputCProject::createMainFile_createEntryIds(std::ofstream &output, Frame *frame){
	for (std::list<Entry*>::iterator it = frame->getEntries()->begin(); it != frame->getEntries()->end(); ++it){
		output << "\t" << (*it)->getModuleInstance() << ".entry_id_" << (*it)->getMethod()->getName() << " = " << frame->getId() << ";" << std::endl;
		output << "\t" << (*it)->getExitModuleInstance() << ".exit_id_" << (*it)->getMethod()->getName() << " = " << frame->getId() << ";" << std::endl;
	}
	
	for (std::list<Frame*>::iterator jt = frame->getFrames()->begin(); jt != frame->getFrames()->end(); ++jt){
		createMainFile_createEntryIds(output, *jt);
	}
}

void OutputCProject::createMainFile_createEntryIsSeq(std::ofstream &output, Frame *frame, int maxLevel){
	output << (frame->getFrameType() == SEQ) ? "1" : "0";

	if(frame->getId() < maxLevel-1) output << ", ";
	
	for (std::list<Frame*>::iterator jt = frame->getFrames()->begin(); jt != frame->getFrames()->end(); ++jt){
		createMainFile_createEntryIsSeq(output, *jt, maxLevel);
	}
}

void OutputCProject::createMainFile_createEntryStructure(std::ofstream &output, Frame *frame, int maxLevel){
	/*if (frame->getFrameType() == SEQ){
		for (std::list<Frame*>::iterator jt = frame->getFrames()->begin(); jt != frame->getFrames()->end(); ++jt){
			std::cout << "s ";
		}
		std::cout << std::endl;
	} else {
		
		for (std::list<Frame*>::iterator jt = frame->getFrames()->begin(); jt != frame->getFrames()->end(); ++jt){
			std::cout << "p ";
		}
		std::cout << std::endl;
	}
	
	output << "\t" << "structure[" << frame->getId() << "] = {";
	for(int i = 0; i <= maxLevel; ++i){
		bool connects = false;
		for (std::list<Frame*>::iterator jt = frame->getFrames()->begin(); jt != frame->getFrames()->end(); ++jt){
			if ((*jt)->getId() == i){
				connects = true;
				break;
			}
		}
		
		output << (connects) ? "1" : "0";
		
		if(i < maxLevel) output << ", ";
	}
	output << "};" << std::endl;
	
	for (std::list<Frame*>::iterator jt = frame->getFrames()->begin(); jt != frame->getFrames()->end(); ++jt){
		createMainFile_createEntryStructure(output, *jt, maxLevel);
	}*/
		
	output << "\t" << "{";
	for(int i = 0; i < maxLevel; ++i){
		bool connects = false;
		for (std::list<Frame*>::iterator jt = frame->getFrames()->begin(); jt != frame->getFrames()->end(); ++jt){
			if ((*jt)->getId() == i){
				connects = true;
				break;
			}
		}
		
		output << (connects) ? "1" : "0";
		
		if(i < maxLevel-1) output << ", ";
	}
	output << "}" << ((frame->getId() == maxLevel-1)?"":", ") << std::endl;
	
	for (std::list<Frame*>::iterator jt = frame->getFrames()->begin(); jt != frame->getFrames()->end(); ++jt){
		createMainFile_createEntryStructure(output, *jt, maxLevel);
	}
}

void OutputCProject::createMainFile(std::list<Module*>* modules,
                                    std::list<Transaction*>* transactions,
								   	std::list<Frame*>* frames){
    std::stringstream ss;
    ss << this->output_dir << "/main.cpp";
    std::ofstream output(ss.str().c_str());
    
    output << "#include \"MultiAccessInterface.hpp\"" << std::endl;   
    output << "#include \"MultiAccessChannel.hpp\"" << std::endl;    
    output << "#include \"Defines.hpp\"" << std::endl;    
	output << "#define SC_INCLUDE_DYNAMIC_PROCESSES" << std::endl;
    output << "#include <systemc>" << std::endl;
        
    output << std::endl;
    
    for (std::list<Module*>::iterator it = modules->begin(); it != modules->end(); ++it)
        output << "#include \"" << ((*it)->getName()).c_str() << ".hpp\"" << std::endl;
    
    output << std::endl;
    
    output << "#define CHANNELS_SIZE 20" << std::endl;
	
    output << std::endl;
    
    output << "int sc_main(int argc, char* argv[]) {" << std::endl;
    
    output << "\t" << "sc_time CLK_PERIOD(50, SC_NS);" << std::endl; 
    output << "\t" << "sc_clock CLK(\"clk01\", CLK_PERIOD);" << std::endl; 
	
    output << std::endl;
	
	// Entry flow structure
	output << "\t" << "bool flowIsSeq[" << numberOfFrames << "] = {";
	for (std::list<Frame*>::iterator jt = frames->begin(); jt != frames->end(); ++jt){
		Frame* frame = *jt;

		createMainFile_createEntryIsSeq(output, frame, numberOfFrames);
	}	
	output << "};" << std::endl;
	output << "\t" << "bool structure[" << numberOfFrames << "][ " << numberOfFrames << "] = {" << std::endl;
	for (std::list<Frame*>::iterator jt = frames->begin(); jt != frames->end(); ++jt){
		Frame* frame = *jt;

		createMainFile_createEntryStructure(output, frame, numberOfFrames);
	}	
	output << "\t" << "};" << std::endl;
	output << "\t" << "_FLOWEXEC.init((bool*)structure, " << numberOfFrames << ", (bool*)flowIsSeq);" << std::endl;
	// end of entry flow structure
	
    output << std::endl;
    
	int instanceCount = 1;
    for (std::list<Module*>::iterator it = modules->begin(); it != modules->end(); ++it)
        for (std::list<Element*>::iterator jt = (*it)->getInstances()->begin(); jt != (*it)->getInstances()->end(); ++jt){
            output << "\t" << ((*jt)->getValue()).c_str() << " " << ((*jt)->getName()).c_str() << "(\"" << (*jt)->getName() << "\")" << ";" << std::endl;
			output << "\t" << (*jt)->getName() << ".CLK(CLK);" << std::endl;
				
			output << std::endl;
		}
	
	int channelCount = 1;
    for (std::list<Module*>::iterator it = modules->begin(); it != modules->end(); ++it){
        for (std::list<Element*>::iterator jt = (*it)->getInstances()->begin(); jt != (*it)->getInstances()->end(); ++jt)
			for (std::list<Service*>::iterator mt = (*it)->getServices()->begin(); mt != (*it)->getServices()->end(); ++mt){
				output << "\t" << "MultiAccessChannel<Data_" << (*it)->getName() << "_" << (*mt)->getName() << "> channel_" << (*jt)->getName() << "_" << (*mt)->getName() << "_to" << "(\"c" << channelCount++ << "\", CHANNELS_SIZE);" << std::endl;
				output << "\t" << "MultiAccessChannel<" << (*mt)->getType() << "> channel_" << (*jt)->getName() << "_" << (*mt)->getName() << "_from" << "(\"c" << channelCount++ << "\", CHANNELS_SIZE);" << std::endl;

				output << std::endl;

				output << "\t" << (*jt)->getName() << ".port_" << (*mt)->getName() << "_send(" << "channel_" << (*jt)->getName() << "_" << (*mt)->getName() << "_from);" << std::endl;
				output << "\t" << (*jt)->getName() << ".port_" << (*mt)->getName() << "_receive(" << "channel_" << (*jt)->getName() << "_" << (*mt)->getName() << "_to);" << std::endl;

				output << std::endl;
			}
	}
		
    for (std::list<Module*>::iterator it = modules->begin(); it != modules->end(); ++it)
        for (std::list<Element*>::iterator mt = (*it)->getInstances()->begin(); mt != (*it)->getInstances()->end(); ++mt){
            for (std::list<Transaction*>::iterator nt = transactions->begin(); nt != transactions->end(); ++nt){
                if((*nt)->getCaller() == *it){
                    // check if the current instance uses any service
                    bool useInstance = false;
                    for (std::list<std::string*>::iterator ot = (*nt)->getNameCaller()->begin(); ot != (*nt)->getNameCaller()->end(); ++ot){
                        if(!(*ot)->compare((*mt)->getName()))
                            useInstance = true;
						std::cout << **ot << std::endl;
					}
                        
                    if(useInstance){
						output << "\t" << (*mt)->getName() << ".port_" << (*nt)->getCallee()->getName() << "_" << (*nt)->getService()->getName() << "_send(channel_" << (*nt)->getNameCallee()<< "_" << (*nt)->getService()->getName() << "_to);" << std::endl;
						output << "\t" << (*mt)->getName() << ".port_" << (*nt)->getCallee()->getName() << "_" << (*nt)->getService()->getName() << "_receive(channel_" << (*nt)->getNameCallee() << "_" << (*nt)->getService()->getName() << "_from);" << std::endl;	
					}
                }
			}
        }
	
	output << std::endl;
	
	// Entry ids
	for (std::list<Frame*>::iterator jt = frames->begin(); jt != frames->end(); ++jt){
		Frame* frame = *jt;

		createMainFile_createEntryIds(output, frame);
	}
	
	output << std::endl;
    
    output << "\t" << "sc_start();" << std::endl;
	
	output << std::endl;
	
    output << "\t" << "return 0;" << std::endl;
    output << "}" << std::endl;
    
    output.close();
}

void OutputCProject::createMakefile(){
    std::stringstream ss;
    ss << this->output_dir << "/Makefile";
    std::ofstream output(ss.str().c_str());
    
    output << "SYSTEMC=$(SYSTEMC_HOME)" << std::endl;
    output << "OS=linux64" << std::endl;
    output << "LIBS=$(SYSTEMC)/lib-$(OS)/" << std::endl;
    output << "INCLUDE=$(SYSTEMC)/include" << std::endl;
    output << "CFLAGS=-I$(INCLUDE) -L$(LIBS) -lsystemc" << std::endl;
    output << std::endl;
	
    output << "all:" << std::endl;
    output << "\t" << "g++ main.cpp sources/*.cpp user/*.cpp -o a.out -I headers -std=c++11 $(CFLAGS)" << std::endl;
    
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
		output << "} Data_" << module->getName() << "_" << (*jt)->getName() << ";" << std::endl;

		output << std::endl;

		output << "typedef struct {" << std::endl;
		output << "\t" << (*jt)->getType() << " response;" << std::endl;
		output << "\t" << "bool ready;" << std::endl;
		output << "} Data_" << module->getName() << "_" << (*jt)->getName() << "_request;" << std::endl;

		output << std::endl;
    }
}


void OutputCProject::createHeaderFiles_createEntryPoints(std::ofstream &output, Module* module, Frame* frame, std::list<Method*> &configuratedMethods, std::list<Method*> &configuratedExitMethods){
	for(std::list<Entry*>::iterator it = frame->getEntries()->begin(); it != frame->getEntries()->end(); ++it){
		if((*it)->getModule() == module && std::find(configuratedMethods.begin(), configuratedMethods.end(), (*it)->getMethod()) == configuratedMethods.end()){		
			output << "\t\t" << "enum " << (*it)->getMethod()->getName() << "_RUN_STATE {" << std::endl;
			output << "\t\t\t" << "ENTRY," << std::endl;
			output << "\t\t\t" << "START," << std::endl;
			int transactionCount = 1;
			for(std::list<Transaction*>::iterator jt = (*it)->getTransactions()->begin(); jt != (*it)->getTransactions()->end(); ++jt){
				if(std::string::npos == (*jt)->getOrder().find(".")){ // Top level transaction
					output << "\t\t\t" << "TRANSACTION_" << transactionCount << "_REQUEST_PARAMS," << std::endl;
					output << "\t\t\t" << "TRANSACTION_" << transactionCount << "_SEND," << std::endl;
					output << "\t\t\t" << "TRANSACTION_" << transactionCount << "_WAIT," << std::endl;
					output << "\t\t\t" << "TRANSACTION_" << transactionCount << "_RECEIVE," << std::endl;
					++transactionCount;
				}
			}
			output << "\t\t\t" << "FINISH," << std::endl;
			output << "\t\t\t" << "EXIT" << std::endl;
			output << "\t\t" << "};" << std::endl;
			output << "\t\t" << "struct {" << std::endl;
			output << "\t\t\t" << (*it)->getMethod()->getName() << "_RUN_STATE STATE = " << (*it)->getMethod()->getName() << "_RUN_STATE::ENTRY;" << std::endl;
			output << "\t\t\t" << (*it)->getMethod()->getName() << "_RUN_STATE LAST_STATE = " << (*it)->getMethod()->getName() << "_RUN_STATE::ENTRY;" << std::endl;
			transactionCount = 1;
			for(std::list<Transaction*>::iterator jt = (*it)->getTransactions()->begin(); jt != (*it)->getTransactions()->end(); ++jt){
				if(std::string::npos == (*jt)->getOrder().find(".")){ // Top level transaction
					output << "\t\t\t" << "Data_" << (*jt)->getCallee()->getName() << "_" << (*jt)->getService()->getName() << " data_" << transactionCount << ";" << std::endl;
					output << "\t\t\t" << "Data_" << (*jt)->getCallee()->getName() << "_" << (*jt)->getService()->getName() << "_request request_" << transactionCount << ";" << std::endl;
					++transactionCount;
				}
			}
			output << std::endl;
			output << "\t\t" << "} " << (*it)->getMethod()->getName() << "_stateData;" << std::endl;
			
			output << std::endl;
				
			output << "\t\t" << "bool " << (*it)->getMethod()->getName() << "_start();" << std::endl;
			output << "\t\t" << "bool " << (*it)->getMethod()->getName() << "_finish();" << std::endl;

			output << std::endl;
				
			for(std::list<Transaction*>::iterator jt = (*it)->getTransactions()->begin(); jt != (*it)->getTransactions()->end(); ++jt){
				if(std::string::npos == (*jt)->getOrder().find(".")){ // Top level transaction
					output << "\t\t" << "Data_" << (*jt)->getCallee()->getName() << "_" << (*jt)->getService()->getName() << " " << (*it)->getMethod()->getName() << "_" << (*jt)->getCallee()->getName() << "_" << (*jt)->getService()->getName() << "_send();" << std::endl;
					output << "\t\t" << "void " << (*it)->getMethod()->getName() << "_" << (*jt)->getCallee()->getName() << "_" << (*jt)->getService()->getName() << "_receive(" << (*jt)->getService()->getType() << " ret);" << std::endl;
					
					configuratedMethods.push_back((*it)->getMethod());
				}
			}
			
			output << "\t\t" << "int entry_id_" << (*it)->getMethod()->getName() << ";" << std::endl;
		}
		
		
		if((*it)->getExitModule() == module && std::find(configuratedExitMethods.begin(), configuratedExitMethods.end(), (*it)->getMethod()) == configuratedExitMethods.end()){	
			output << "\t\t" << "int exit_id_" << (*it)->getMethod()->getName() << ";" << std::endl;
					
			configuratedExitMethods.push_back((*it)->getMethod());
		}
	}
		
	for (std::list<Frame*>::iterator jt = frame->getFrames()->begin(); jt != frame->getFrames()->end(); ++jt){
		createHeaderFiles_createEntryPoints(output, module, *jt, configuratedMethods, configuratedExitMethods);
	}	
}

void OutputCProject::createHeaderFiles(std::list<Module*>* modules, std::list<Transaction*>* transactions, std::list<Frame*>* frames){
    for (std::list<Module*>::iterator it = modules->begin(); it != modules->end(); ++it){
        std::stringstream ss;
        ss << this->output_dir << "/headers/" << (*it)->getName() << ".hpp";
        std::ofstream output(ss.str().c_str());
        
        std::string moduleName = (*it)->getName();
        transform(moduleName.begin(), moduleName.end(), moduleName.begin(), ::toupper);
        output << "#ifndef " << moduleName << "_H" << std::endl;
        output << "#define " << moduleName << "_H" << std::endl;
        
        output << std::endl;
        
        output << "#include \"Defines.hpp\"" << std::endl;
        output << "#include \"MultiAccessInterface.hpp\"" << std::endl;
        output << "#include \"MultiAccessChannel.hpp\"" << std::endl;
    	output << "#define SC_INCLUDE_DYNAMIC_PROCESSES" << std::endl;
        output << "#include <systemc>" << std::endl;
        
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
        
        output << "class " << (*it)->getName() << " : public sc_module {" << std::endl;
        output << std::endl;
        
        output << "\t" << "public:" << std::endl;
        
        output << "\t\t" << (*it)->getName() << "(sc_module_name name);" << std::endl;
        output << "\t\t" << "sc_in<bool> CLK;" << std::endl;
        
        output << std::endl;
		
		// Methods
        for (std::list<Method*>::iterator jt = (*it)->getMethods()->begin(); jt != (*it)->getMethods()->end(); ++jt){
			if ((*jt)->getModifier() != PUBLIC){
				output << "\t\t" << (*jt)->getType() << " " << (*jt)->getName() << "(";

				bool firstParam = true;
				for (std::list<Parameter*>::iterator kt = (*jt)->getParameters()->begin(); kt != (*jt)->getParameters()->end(); ++kt){
					if(!firstParam)
						output << ", ";

					output << (*kt)->getType() << " " << (*kt)->getName();

					firstParam = false;
				}

				output << ");" << std::endl;

				output << std::endl;
			}
		}
		
		/*
		 * Services nested calls
		 */
		std::map<std::string, Method*> servicesNestedCalls;
		for (std::list<Transaction*>::iterator kt = transactions->begin(); kt != transactions->end(); ++kt){
			if ((*kt)->getCallee() == *it){
				servicesNestedCalls.insert( std::pair<std::string, Method*>( (*kt)->getOrder(), (*kt)->getService() ) );
			}
		}
		
		// Offering services
        for (std::list<Service*>::iterator jt = (*it)->getServices()->begin(); jt != (*it)->getServices()->end(); ++jt){
            output << "\t\t" << "void service_SM_" << (*jt)->getName() << "();" << std::endl;
			
            output << "\t\t" << "void " << (*jt)->getName() << "_start(";
            
            bool firstParam = true;
            for (std::list<Parameter*>::iterator kt = (*jt)->getParameters()->begin(); kt != (*jt)->getParameters()->end(); ++kt){
                if(!firstParam)
                    output << ", ";
                
                output << (*kt)->getType() << " " << (*kt)->getName();
                
                firstParam = false;
            }

            output << ");" << std::endl;
			
            output << "\t\t" << (*jt)->getType() << " " << (*jt)->getName() << "_finish();" << std::endl;
			
			output << std::endl;
			
			
			output << "\t\t" << "enum " << (*jt)->getName() << "_SERVICE_STATE {" << std::endl;
			output << "\t\t\t" << "WAITING_READ," << std::endl;
			output << "\t\t\t" << "SERVICE_START," << std::endl;

			int transactionCount = 1;
			std::list<Method*> configuredServices;
			for (std::list<Transaction*>::iterator kt = transactions->begin(); kt != transactions->end(); ++kt){
				if ((*kt)->getCaller() == *it && std::find(configuredServices.begin(), configuredServices.end(), (*kt)->getService()) == configuredServices.end()){
					std::size_t lastDot = (*kt)->getOrder().find_last_of(".");

					if (std::string::npos != lastDot){
						std::string parentOrder = (*kt)->getOrder().substr(0, lastDot);
						std::map<std::string, Method*>::iterator mt = servicesNestedCalls.find(parentOrder);
						if (servicesNestedCalls.end() != mt && mt->second == *jt){
							output << "\t\t\t" << "TRANSACTION_" << transactionCount << "_REQUEST_PARAMS," << std::endl;
							output << "\t\t\t" << "TRANSACTION_" << transactionCount << "_SEND," << std::endl;
							output << "\t\t\t" << "TRANSACTION_" << transactionCount << "_WAIT," << std::endl;
							output << "\t\t\t" << "TRANSACTION_" << transactionCount << "_RECEIVE," << std::endl;

							++transactionCount;
						}
					}

					configuredServices.push_back((*kt)->getService());
				}
			}

			int totalTransactions = transactionCount - 1;

			output << "\t\t\t" << "SERVICE_FINISH," << std::endl;
			output << "\t\t\t" << "WAITING_WRITE," << std::endl;
			output << "\t\t\t" << "EXIT" << std::endl;			
			output << "\t\t" << "};" << std::endl;

			output << std::endl;
			
			output << "\t\t" << "struct {" << std::endl;
			output << "\t\t\t" << (*jt)->getName() << "_SERVICE_STATE STATE = " << (*jt)->getName() << "_SERVICE_STATE::WAITING_READ;" << std::endl;
			output << "\t\t\t" << (*jt)->getName() << "_SERVICE_STATE LAST_STATE = " << (*jt)->getName() << "_SERVICE_STATE::WAITING_READ;" << std::endl;
			output << "\t\t\t" << "Data_" << (*it)->getName() << "_" << (*jt)->getName() << " data;" << std::endl;
			output << "\t\t\t" << "void* obj;" << std::endl;
			output << "\t\t\t" << (*jt)->getType() << " ret;";
	
			transactionCount = 1;
			configuredServices.clear();
			for (std::list<Transaction*>::iterator kt = transactions->begin(); kt != transactions->end(); ++kt){
				if ((*kt)->getCaller() == *it && std::find(configuredServices.begin(), configuredServices.end(), (*kt)->getService()) == configuredServices.end()){
					std::size_t lastDot = (*kt)->getOrder().find_last_of(".");

					if (std::string::npos != lastDot){
						std::string parentOrder = (*kt)->getOrder().substr(0, lastDot);
						std::map<std::string, Method*>::iterator mt = servicesNestedCalls.find(parentOrder);
						if (servicesNestedCalls.end() != mt && mt->second == *jt){
							output << std::endl;
							output << "\t\t\t" << "Data_" << (*kt)->getCallee()->getName() << "_" << (*kt)->getService()->getName() << " data_" << transactionCount << ";" << std::endl;
							output << "\t\t\t" << "Data_" << (*kt)->getCallee()->getName() << "_" << (*kt)->getService()->getName() << "_request request_" << transactionCount << ";";

							++transactionCount;
						}
					}

					configuredServices.push_back((*kt)->getService());
				}
			}
			output << std::endl;
			
			output << "\t\t" << "} " << (*jt)->getName() << "_stateData;" << std::endl;

			output << std::endl;
			
			// Start of transaction methods
			transactionCount = 1;
			std::list<Method*> configuratedServices;
			for (std::list<Transaction*>::iterator kt = transactions->begin(); kt != transactions->end(); ++kt){
				if ((*kt)->getCaller() == *it && std::find(configuratedServices.begin(), configuratedServices.end(), (*kt)->getService()) == configuratedServices.end()){
					std::size_t lastDot = (*kt)->getOrder().find_last_of(".");
					
					if (std::string::npos != lastDot){
						std::string parentOrder = (*kt)->getOrder().substr(0, lastDot);
						std::map<std::string, Method*>::iterator mt = servicesNestedCalls.find(parentOrder);
						if (servicesNestedCalls.end() != mt && mt->second == *jt){
							// Transaction send method
							output << "\t\t" << "Data_" << (*kt)->getCallee()->getName() << "_" << (*kt)->getService()->getName() << " " << (*jt)->getName() << "_" << (*kt)->getCallee()->getName() << "_" << (*kt)->getService()->getName() << "_send();" << std::endl;
							
							// Transaction receive method
							output << "\t\t" << "void " << (*jt)->getName() << "_" << (*kt)->getCallee()->getName() << "_" << (*kt)->getService()->getName() << "_receive(" << (*kt)->getService()->getType() << " ret);" << std::endl;

							output << std::endl;
							
							++transactionCount;
							
							configuratedServices.push_back((*kt)->getService());
						}
					}
				}
			}
            
            output << "\t\t" << "sc_port< read_ma < Data_" << (*it)->getName() << "_" << (*jt)->getName() << " > > port_" << (*jt)->getName() << "_receive;" << std::endl;
            output << "\t\t" << "sc_port< write_ma < " << (*jt)->getType() << " > > port_" << (*jt)->getName() << "_send;" << std::endl;
			
			output << std::endl;
        }
        
		// Used services
		std::list<Method*> configuratedServices;
		for (std::list<Transaction*>::iterator nt = transactions->begin(); nt != transactions->end(); ++nt)
			if((*nt)->getCaller() == *it && std::find(configuratedServices.begin(), configuratedServices.end(), (*nt)->getService()) == configuratedServices.end()){
				output << "\t" << "\t" << "sc_port< read_ma < " << (*nt)->getService()->getType() << " > > port_" << (*nt)->getCallee()->getName() << "_" << (*nt)->getService()->getName() << "_receive;" << std::endl;
				output << "\t" << "\t" << "sc_port< write_ma < Data_" << (*nt)->getCallee()->getName() << "_" << (*nt)->getService()->getName() << " > > port_" << (*nt)->getCallee()->getName() << "_" << (*nt)->getService()->getName() << "_send;" << std::endl;
				output << "\t" << "\t" << "bool service_" << (*nt)->getCallee()->getName() << "_" << (*nt)->getService()->getName() << "(Data_" << (*nt)->getCallee()->getName() << "_" << (*nt)->getService()->getName() << ", " << "Data_" << (*nt)->getCallee()->getName() << "_" << (*nt)->getService()->getName() << "_request*);" << std::endl;;
				output << "\t" << "\t" << "void service_" << (*nt)->getCallee()->getName() << "_" << (*nt)->getService()->getName() << "_checkRequest(Data_" << (*nt)->getCallee()->getName() << "_" << (*nt)->getService()->getName() << "_request*);" << std::endl;;

				output << std::endl;
				
				configuratedServices.push_back((*nt)->getService());
			}
		
        /*
         * Execution flow - Entry/Exit points
         */
		std::list<Method*> configuratedMethods, configuratedExitMethods;
        for (std::list<Frame*>::iterator jt = frames->begin(); jt != frames->end(); ++jt){
			Frame* frame = *jt;
			
            createHeaderFiles_createEntryPoints(output, *it, frame, configuratedMethods, configuratedExitMethods);
        }
        
        output << std::endl;
        
        output << "};" << std::endl;
        
        output << std::endl;
        
        output << "#endif" << std::endl;
        
        output.close();
    }
}


void OutputCProject::createUserSourceFiles_createEntryPoints(std::ofstream &output, Module* module, Frame* frame, int runLevel, std::string runPreviousSetup, std::list<Method*> &configuratedMethods){
	for(std::list<Entry*>::iterator it = frame->getEntries()->begin(); it != frame->getEntries()->end(); ++it){
		if((*it)->getModule() == module && std::find(configuratedMethods.begin(), configuratedMethods.end(), (*it)->getMethod()) == configuratedMethods.end()){
			output << "// Execution flow " << (*it)->getMethod()->getName() << std::endl;
			output << std::endl;
			
			output << "bool " << module->getName() << "::" << (*it)->getMethod()->getName() << "_start(){" << std::endl;
			output << "\t" << "// Implement this method with your pre-processing" << std::endl;
			output << "\t" << "// Return true to continue this flow" << std::endl;
			output << "\t" << "// Or false to execute the start method again" << std::endl;
			output << "\t" << "return true;" << std::endl;
			output << "}" << std::endl;
			
			output << std::endl;
			
			int transactionCount = 1;
			for(std::list<Transaction*>::iterator jt = (*it)->getTransactions()->begin(); jt != (*it)->getTransactions()->end(); ++jt){
				if(std::string::npos == (*jt)->getOrder().find(".")){ // Top level transaction
					// Transaction send method
					output << "Data_" << (*jt)->getCallee()->getName() << "_" << (*jt)->getService()->getName() << " " << module->getName() << "::" << (*it)->getMethod()->getName() << "_" << (*jt)->getCallee()->getName() << "_" << (*jt)->getService()->getName() << "_send(){" << std::endl;
					output << "\t" << "// Fill up the data parameters to call the service" << std::endl;
					output << "\t" << "Data_" << (*jt)->getCallee()->getName() << "_" << (*jt)->getService()->getName() << " data;" << std::endl;

					for (std::list<Parameter*>::iterator kt = (*jt)->getService()->getParameters()->begin(); kt != (*jt)->getService()->getParameters()->end(); ++kt)
						output << "\t" << "// data." << (*kt)->getName() << " = ?;" << std::endl;

					output << "\t" << "return data;" << std::endl;
					output << "}" << std::endl;

					output << std::endl;

					// Transaction receive method
					output << "void " << module->getName() << "::" << (*it)->getMethod()->getName() << "_" << (*jt)->getCallee()->getName() << "_" << (*jt)->getService()->getName() << "_receive(" << (*jt)->getService()->getType() << " ret){" << std::endl;
					output << "\t" << "// Implement this method" << std::endl;
					output << "}" << std::endl;

					output << std::endl;

					++transactionCount;
				}
			}
			
			output << "bool " << module->getName() << "::" << (*it)->getMethod()->getName() << "_finish(){" << std::endl;
			output << "\t" << "// Implement this method with your post-processing" << std::endl;
			output << "\t" << "// Return true to finish this flow" << std::endl;
			output << "\t" << "// Or false to execute the finish method again" << std::endl;
			output << "\t" << "return true;" << std::endl;
			output << "}" << std::endl;
			
			output << std::endl;
			
			configuratedMethods.push_back((*it)->getMethod());
		}
	}
	
	for (std::list<Frame*>::iterator jt = frame->getFrames()->begin(); jt != frame->getFrames()->end(); ++jt){
		createUserSourceFiles_createEntryPoints(output, module, *jt, runLevel+1, runPreviousSetup, configuratedMethods);
	}
}

void OutputCProject::createUserSourceFiles(std::list<Module*>* modules, std::list<Transaction*>* transactions, std::list<Frame*>* frames){
	for (std::list<Module*>::iterator it = modules->begin(); it != modules->end(); ++it){
        std::stringstream ss;
        ss << this->output_dir << "/user/" << (*it)->getName() << ".cpp";
        std::ofstream output(ss.str().c_str());
        
        output << "#include \"" << (*it)->getName() << ".hpp\"" << std::endl;
        
        output << std::endl;
		
		/*
		 * Services nested calls
		 */
		std::map<std::string, Method*> servicesNestedCalls;
		for (std::list<Transaction*>::iterator kt = transactions->begin(); kt != transactions->end(); ++kt){
			if ((*kt)->getCallee() == *it){
				servicesNestedCalls.insert( std::pair<std::string, Method*>( (*kt)->getOrder(), (*kt)->getService() ) );
			}
		}
        
        /*
         * Services offered - service function
         */
        for (std::list<Service*>::iterator jt = (*it)->getServices()->begin(); jt != (*it)->getServices()->end(); ++jt){
			// Start of service start method
            output << "void " << (*it)->getName() << "::" <<  (*jt)->getName() << "_start(";
            
            bool firstParam = true;
            for (std::list<Parameter*>::iterator kt = (*jt)->getParameters()->begin(); kt != (*jt)->getParameters()->end(); ++kt){
                if(!firstParam)
                    output << ", ";
                
                output << (*kt)->getType() << " " << (*kt)->getName();
                
                firstParam = false;
            }

            output << "){" << std::endl;
            
			output << "\t" << "// Implement this function" << std::endl;
            
            output << "}" << std::endl;
			
        	output << std::endl;
			// End of service start method
			
			// Start of service finish method
            output << (*jt)->getType() << " " << (*it)->getName() << "::" <<  (*jt)->getName() << "_finish(){" << std::endl;
            
			output << "\t" << "// Implement this function" << std::endl;
            
            output << "}" << std::endl;
			
        	output << std::endl;
			// End of service finish method
			
			// Start of transaction methods
			int transactionCount = 1;
			std::list<Method*> configuratedServices;
			for (std::list<Transaction*>::iterator kt = transactions->begin(); kt != transactions->end(); ++kt){
				if ((*kt)->getCaller() == *it && std::find(configuratedServices.begin(), configuratedServices.end(), (*kt)->getService()) == configuratedServices.end()){
					std::size_t lastDot = (*kt)->getOrder().find_last_of(".");
					
					if (std::string::npos != lastDot){
						std::string parentOrder = (*kt)->getOrder().substr(0, lastDot);
						std::map<std::string, Method*>::iterator mt = servicesNestedCalls.find(parentOrder);
						if (servicesNestedCalls.end() != mt && mt->second == *jt){
							// Transaction send method
							output << "Data_" << (*kt)->getCallee()->getName() << "_" << (*kt)->getService()->getName() << " " << (*it)->getName() << "::" << (*jt)->getName() << "_" << (*kt)->getCallee()->getName() << "_" << (*kt)->getService()->getName() << "_send(){" << std::endl;
							output << "\t" << "// Fill up the data parameters to call the service" << std::endl;
							output << "\t" << "Data_" << (*kt)->getCallee()->getName() << "_" << (*kt)->getService()->getName() << " data;" << std::endl;

							for (std::list<Parameter*>::iterator ht = (*kt)->getService()->getParameters()->begin(); ht != (*kt)->getService()->getParameters()->end(); ++ht)
								output << "\t" << "// data." << (*ht)->getName() << " = ?;" << std::endl;

							output << "\t" << "return data;" << std::endl;
							output << "}" << std::endl;

							output << std::endl;
							
							// Transaction receive method
							output << "void " << (*it)->getName() << "::" << (*jt)->getName() << "_" << (*kt)->getCallee()->getName() << "_" << (*kt)->getService()->getName() << "_receive(" << (*kt)->getService()->getType() << " ret){" << std::endl;
							output << "\t" << "// Implement this method" << std::endl;
							output << "}" << std::endl;

							output << std::endl;

							++transactionCount;
							
							configuratedServices.push_back((*kt)->getService());
						}
					}
				}
			}
        }
		
        /*
         * Execution flow - Entry/Exit points
         */
		std::list<Method*> configuratedMethods;
        for (std::list<Frame*>::iterator jt = frames->begin(); jt != frames->end(); ++jt){
			Frame* frame = *jt;
			
            createUserSourceFiles_createEntryPoints(output, *it, frame, 1, "", configuratedMethods);
        }
        
        output << std::endl;
        
        output.close();
    }
}


void OutputCProject::createSystemSourceFiles_initiateEntrySCMethods(std::ofstream &output, Module* module, Frame* frame, std::list<Method*> &configuratedMethods){
	for(std::list<Entry*>::iterator it = frame->getEntries()->begin(); it != frame->getEntries()->end(); ++it){
		if((*it)->getModule() == module && std::find(configuratedMethods.begin(), configuratedMethods.end(), (*it)->getMethod()) == configuratedMethods.end()){
			output << "\t" << "SC_METHOD(" << (*it)->getMethod()->getName() << ");" << std::endl;
			output << "\t" << "sensitive << CLK.pos();" << std::endl;
			
			configuratedMethods.push_back((*it)->getMethod());
		}
	}
	
	for (std::list<Frame*>::iterator jt = frame->getFrames()->begin(); jt != frame->getFrames()->end(); ++jt){
		createSystemSourceFiles_initiateEntrySCMethods(output, module, *jt, configuratedMethods);
	}	
}

void OutputCProject::createSystemSourceFiles_createEntryPoints(std::ofstream &output, Module* module, Frame* frame, int runLevel, std::string runPreviousSetup, std::list<Method*> &configuratedMethods){
	for(std::list<Entry*>::iterator it = frame->getEntries()->begin(); it != frame->getEntries()->end(); ++it){
		if((*it)->getModule() == module && std::find(configuratedMethods.begin(), configuratedMethods.end(), (*it)->getMethod()) == configuratedMethods.end()){
			
			output << "void " << module->getName() << "::" << (*it)->getMethod()->getName() << "(){" << std::endl;
			

			/*output << "\t" << "enum RUN_STATE {" << std::endl;
			output << "\t\t" << "ENTRY," << std::endl;
			int transactionCount = 1;
			for(std::list<Transaction*>::iterator jt = (*it)->getTransactions()->begin(); jt != (*it)->getTransactions()->end(); ++jt){
				if(std::string::npos == (*jt)->getOrder().find(".")){ // Top level transaction
					output << "\t\t" << "TRANSACTION_" << transactionCount << "_REQUEST_PARAMS," << std::endl;
					output << "\t\t" << "TRANSACTION_" << transactionCount << "_SEND," << std::endl;
					output << "\t\t" << "TRANSACTION_" << transactionCount << "_WAIT," << std::endl;
					output << "\t\t" << "TRANSACTION_" << transactionCount << "_RECEIVE," << std::endl;

					++transactionCount;
				}
			}
			output << "\t\t" << "FINISH," << std::endl;
			output << "\t\t" << "EXIT" << std::endl;
			output << "\t" << "};" << std::endl;
			
			int totalTransactions = transactionCount - 1;
			
			output << std::endl;
			
			output << "\t" << "RUN_STATE STATE = ENTRY;" << std::endl;
			output << "\t" << "static RUN_STATE LAST_STATE = ENTRY;" << std::endl;
			
			output << std::endl;
			
			transactionCount = 1;
			for(std::list<Transaction*>::iterator jt = (*it)->getTransactions()->begin(); jt != (*it)->getTransactions()->end(); ++jt){
				if(std::string::npos == (*jt)->getOrder().find(".")){ // Top level transaction
					output << "\t" << "static Data_" << (*jt)->getCallee()->getName() << "_" << (*jt)->getService()->getName() << " data_" << transactionCount << ";" << std::endl;
					output << "\t" << "static Data_" << (*jt)->getCallee()->getName() << "_" << (*jt)->getService()->getName() << "_request request_" << transactionCount << ";" << std::endl;

					output << std::endl;

					++transactionCount;
				}
			}*/
			
			int transactionCount = 1;
			output << "\t" << "if(" << (*it)->getMethod()->getName() << "_stateData.STATE != " << (*it)->getMethod()->getName() << "_RUN_STATE::ENTRY && " << (*it)->getMethod()->getName() << "_stateData.STATE != " << (*it)->getMethod()->getName() << "_RUN_STATE::EXIT){";
			for(std::list<Transaction*>::iterator jt = (*it)->getTransactions()->begin(); jt != (*it)->getTransactions()->end(); ++jt){
				if(std::string::npos == (*jt)->getOrder().find(".")){ // Top level transaction
					output << std::endl;

					output << "\t\t" << "if(" << (*it)->getMethod()->getName() << "_stateData.request_" << transactionCount << ".ready){" << std::endl;
					output << "\t\t\t" << (*it)->getMethod()->getName() << "_stateData.request_" << transactionCount << ".ready = false;" << std::endl;
					output << "\t\t\t" << (*it)->getMethod()->getName() << "_stateData.LAST_STATE = " << (*it)->getMethod()->getName() << "_stateData.STATE;" << std::endl;
					output << "\t\t\t" << (*it)->getMethod()->getName() << "_stateData.STATE = " << (*it)->getMethod()->getName() << "_RUN_STATE::TRANSACTION_" << transactionCount << "_RECEIVE;" << std::endl;
					output << "\t\t" << "}" << std::endl;

					++transactionCount;
				}
			}
			output << "\t" << "}" << std::endl;
			
			output << std::endl;
			
			int totalTransactions = transactionCount - 1;
			
			
			output << "\t" << "switch(" << (*it)->getMethod()->getName() << "_stateData.STATE){" << std::endl;
			output << "\t\t" << "case " << (*it)->getMethod()->getName() << "_RUN_STATE::ENTRY:" << std::endl;
			output << "\t\t\t" << "if(_FLOWEXEC.ENTRY(entry_id_" << (*it)->getMethod()->getName() << ")){" << std::endl;
			output << "\t\t\t\t" << (*it)->getMethod()->getName() << "_stateData.STATE = " << (*it)->getMethod()->getName() << "_RUN_STATE::START;" << std::endl;
			
			transactionCount = 1;
			for(std::list<Transaction*>::iterator jt = (*it)->getTransactions()->begin(); jt != (*it)->getTransactions()->end(); ++jt){
				std::cout << (*jt)->getOrder() << " -> "<< (*jt)->getNameCallee() << std::endl;
				if(std::string::npos == (*jt)->getOrder().find(".")){ // Top level transaction
					output << "\t\t\t\t" << (*it)->getMethod()->getName() << "_stateData.request_" << transactionCount << ".ready = false;" << std::endl;
				}
			}
			output << "\t\t\t" << "}" << std::endl;
			
			output << "\t\t\t" << "break;" << std::endl;
			
			output << "\t\t" << "case " << (*it)->getMethod()->getName() << "_RUN_STATE::START:" << std::endl;
			
			output << "\t\t\t" << "if(" << (*it)->getMethod()->getName() << "_start())" << std::endl;
			if(totalTransactions)
				output << "\t\t\t\t" << (*it)->getMethod()->getName() << "_stateData.STATE = " << (*it)->getMethod()->getName() << "_RUN_STATE::TRANSACTION_1_REQUEST_PARAMS;" << std::endl;
			else
				output << "\t\t\t\t" << (*it)->getMethod()->getName() << "_stateData.STATE = " << (*it)->getMethod()->getName() << "_RUN_STATE::FINISH;" << std::endl;
			
			output << "\t\t\t" << "break;" << std::endl;
			
			transactionCount = 1;
			for(std::list<Transaction*>::iterator jt = (*it)->getTransactions()->begin(); jt != (*it)->getTransactions()->end(); ++jt){
				std::cout << (*jt)->getOrder() << " -> "<< (*jt)->getNameCallee() << std::endl;
				if(std::string::npos == (*jt)->getOrder().find(".")){ // Top level transaction
					output << "\t\t" << "case " << (*it)->getMethod()->getName() << "_RUN_STATE::TRANSACTION_" << transactionCount << "_REQUEST_PARAMS:" << std::endl;
					output << "\t\t\t" << (*it)->getMethod()->getName() << "_stateData.data_" << transactionCount << " = " << (*it)->getMethod()->getName() << "_" << (*jt)->getCallee()->getName() << "_" << (*jt)->getService()->getName() << "_send();" << std::endl;
					output << "\t\t\t" << (*it)->getMethod()->getName() << "_stateData.STATE = " << (*it)->getMethod()->getName() << "_RUN_STATE::TRANSACTION_" << transactionCount << "_SEND;" << std::endl;
					output << "\t\t\t" << "break;" << std::endl;

					output << "\t\t" << "case " << (*it)->getMethod()->getName() << "_RUN_STATE::TRANSACTION_" << transactionCount << "_SEND:" << std::endl;
					output << "\t\t\t" << "if(service_" << (*jt)->getCallee()->getName() << "_" << (*jt)->getService()->getName() << "(" << (*it)->getMethod()->getName() << "_stateData.data_" << transactionCount << ", &" << (*it)->getMethod()->getName() << "_stateData.request_" << transactionCount << "))" << std::endl;
					output << "\t\t\t\t" << (*it)->getMethod()->getName() << "_stateData.STATE = " << (*it)->getMethod()->getName() << "_RUN_STATE::TRANSACTION_" << transactionCount << "_WAIT;" << std::endl;
					output << "\t\t\t" << "break;" << std::endl;

					output << "\t\t" << "case " << (*it)->getMethod()->getName() << "_RUN_STATE::TRANSACTION_" << transactionCount << "_WAIT:" << std::endl;
					if((*jt)->getTransactionType() == ASYNC)
						if(totalTransactions > transactionCount)
							output << "\t\t\t" << (*it)->getMethod()->getName() << "_stateData.STATE = " << (*it)->getMethod()->getName() << "_RUN_STATE::TRANSACTION_" << (transactionCount+1) << "_REQUEST_PARAMS;" << std::endl;
						else
							output << "\t\t\t" << (*it)->getMethod()->getName() << "_stateData.STATE = " << (*it)->getMethod()->getName() << "_RUN_STATE::FINISH;" << std::endl;
					output << "\t\t\t" << "break;" << std::endl;

					output << "\t\t" << "case " << (*it)->getMethod()->getName() << "_RUN_STATE::TRANSACTION_" << transactionCount << "_RECEIVE:" << std::endl;
					output << "\t\t\t" << (*it)->getMethod()->getName() << "_" << (*jt)->getCallee()->getName() << "_" << (*jt)->getService()->getName() << "_receive(" << (*it)->getMethod()->getName() << "_stateData.request_" << transactionCount << ".response);" << std::endl;
					if(totalTransactions > transactionCount)
						output << "\t\t\t" << (*it)->getMethod()->getName() << "_stateData.STATE = (" << (*it)->getMethod()->getName() << "_stateData.LAST_STATE == " << (*it)->getMethod()->getName() << "_RUN_STATE::TRANSACTION_" << transactionCount << "_WAIT) ? " << (*it)->getMethod()->getName() << "_RUN_STATE::TRANSACTION_" << (transactionCount+1) << "_REQUEST_PARAMS : " << (*it)->getMethod()->getName() << "_stateData.LAST_STATE;" << std::endl;
					else
						output << "\t\t\t" << (*it)->getMethod()->getName() << "_stateData.STATE = (" << (*it)->getMethod()->getName() << "_stateData.LAST_STATE == " << (*it)->getMethod()->getName() << "_RUN_STATE::TRANSACTION_" << transactionCount << "_WAIT) ? " << (*it)->getMethod()->getName() << "_RUN_STATE::FINISH : " << (*it)->getMethod()->getName() << "_stateData.LAST_STATE;" << std::endl;
					output << "\t\t\t" << "break;" << std::endl;

					++transactionCount;
				}
			}
			
			output << "\t\t" << "case " << (*it)->getMethod()->getName() << "_RUN_STATE::FINISH:" << std::endl;
			output << "\t\t\t" << "if(" << (*it)->getMethod()->getName() << "_finish())" << std::endl;
			output << "\t\t\t\t" << (*it)->getMethod()->getName() << "_stateData.STATE = " << (*it)->getMethod()->getName() << "_RUN_STATE::EXIT;" << std::endl;
			output << "\t\t\t" << "break;" << std::endl;
			
			output << "\t\t" << "case " << (*it)->getMethod()->getName() << "_RUN_STATE::EXIT:" << std::endl;
			
			output << "\t\t\t" << "if(true";
			std::list<Entry*> exits;
			{
				std::list<Method*> configuredServices;
				std::list<Entry*> subexits = getExits(module, frame, configuredServices);
				exits.insert(exits.end(), subexits.begin(), subexits.end());
			}
			for (std::list<Entry*>::iterator kt = exits.begin(); kt != exits.end(); ++kt){
				output << "	&& _FLOWEXEC.EXIT(exit_id_" << (*it)->getMethod()->getName() << ")";
			}
			output << ")" << std::endl;
			output << "\t\t\t\t" << (*it)->getMethod()->getName() << "_stateData.STATE = " << (*it)->getMethod()->getName() << "_RUN_STATE::ENTRY;" << std::endl;
			output << "\t\t\t" << "break;" << std::endl;
			output << "\t" << "}" << std::endl;
			
			output << "}" << std::endl;
			
			output << std::endl;
			
			configuratedMethods.push_back((*it)->getMethod());
		}
	}
	
	for (std::list<Frame*>::iterator jt = frame->getFrames()->begin(); jt != frame->getFrames()->end(); ++jt){
		createSystemSourceFiles_createEntryPoints(output, module, *jt, runLevel+1, runPreviousSetup, configuratedMethods);
	}
}

void OutputCProject::createSystemSourceFiles(std::list<Module*>* modules, std::list<Transaction*>* transactions, std::list<Frame*>* frames){	
    for (std::list<Module*>::iterator it = modules->begin(); it != modules->end(); ++it){
        std::stringstream ss;
        ss << this->output_dir << "/sources/" << (*it)->getName() << ".cpp";
        std::ofstream output(ss.str().c_str());
        
        output << "#include \"" << (*it)->getName() << ".hpp\"" << std::endl;
        
        output << std::endl;
        
        output << (*it)->getName() << "::" << (*it)->getName() << "(sc_module_name name) : sc_module(name){" << std::endl;
		output << "\t" << "SC_HAS_PROCESS(" << (*it)->getName() << ");" << std::endl;
		
        for (std::list<Service*>::iterator jt = (*it)->getServices()->begin(); jt != (*it)->getServices()->end(); ++jt){
			output << "\t" << "SC_METHOD(service_SM_" << (*jt)->getName() << ");" << std::endl;
			output << "\t" << "sensitive << CLK.pos();" << std::endl;
		}
		
		/*
		 * Services nested calls
		 */
		std::map<std::string, Method*> servicesNestedCalls;
		std::list<Method*> configuredServices;
		for (std::list<Transaction*>::iterator kt = transactions->begin(); kt != transactions->end(); ++kt){
			if ((*kt)->getCallee() == *it && std::find(configuredServices.begin(), configuredServices.end(), (*kt)->getService()) == configuredServices.end()){
				servicesNestedCalls.insert( std::pair<std::string, Method*>( (*kt)->getOrder(), (*kt)->getService() ) );
				
				configuredServices.push_back((*kt)->getService());
			}
		}
		
        /*
         * Execution flow - Entry/Exit points
         */
		{
			std::list<Method*> configuratedMethods;
			for (std::list<Frame*>::iterator jt = frames->begin(); jt != frames->end(); ++jt){
				Frame* frame = *jt;

				createSystemSourceFiles_initiateEntrySCMethods(output, *it, frame, configuratedMethods);
			}
		}
		
		output << "}" << std::endl;
		
        output << std::endl;
                
        /*
         * Services offered - service function
         */
        for (std::list<Service*>::iterator jt = (*it)->getServices()->begin(); jt != (*it)->getServices()->end(); ++jt){
            output << "void " << (*it)->getName() << "::service_SM_" <<  (*jt)->getName() << "(){" << std::endl;
			/*output << "\t" << "enum SERVICE_STATE {" << std::endl;
			output << "\t\t" << "WAITING_READ," << std::endl;
			output << "\t\t" << "SERVICE_START," << std::endl;
			
			int transactionCount = 1;
			std::list<Method*> configuredServices;
			for (std::list<Transaction*>::iterator kt = transactions->begin(); kt != transactions->end(); ++kt){
				if ((*kt)->getCaller() == *it && std::find(configuredServices.begin(), configuredServices.end(), (*kt)->getService()) == configuredServices.end()){
					std::size_t lastDot = (*kt)->getOrder().find_last_of(".");
					
					if (std::string::npos != lastDot){
						std::string parentOrder = (*kt)->getOrder().substr(0, lastDot);
						std::map<std::string, Method*>::iterator mt = servicesNestedCalls.find(parentOrder);
						if (servicesNestedCalls.end() != mt && mt->second == *jt){
							output << "\t\t" << "TRANSACTION_" << transactionCount << "_REQUEST_PARAMS," << std::endl;
							output << "\t\t" << "TRANSACTION_" << transactionCount << "_SEND," << std::endl;
							output << "\t\t" << "TRANSACTION_" << transactionCount << "_WAIT," << std::endl;
							output << "\t\t" << "TRANSACTION_" << transactionCount << "_RECEIVE," << std::endl;
							
							++transactionCount;
						}
					}
					
					configuredServices.push_back((*kt)->getService());
				}
			}
			
			int totalTransactions = transactionCount - 1;
			
			output << "\t\t" << "SERVICE_FINISH," << std::endl;
			output << "\t\t" << "WAITING_WRITE" << std::endl;
			output << "\t" << "};" << std::endl;
			
			output << std::endl;
			
			output << "\t" << "static SERVICE_STATE STATE = WAITING_READ;" << std::endl;
			output << "\t" << "static SERVICE_STATE LAST_STATE = WAITING_READ;" << std::endl;
			output << "\t" << "static Data_" << (*it)->getName() << "_" << (*jt)->getName() << " data;" << std::endl;
			output << "\t" << "static void* obj;" << std::endl;
			output << "\t" << "static " << (*jt)->getType() << " ret;" << std::endl;
			
			output << std::endl;
			
			
			// Start of transaction data
			
			transactionCount = 1;
			configuredServices.clear();
			for (std::list<Transaction*>::iterator kt = transactions->begin(); kt != transactions->end(); ++kt){
				if ((*kt)->getCaller() == *it && std::find(configuredServices.begin(), configuredServices.end(), (*kt)->getService()) == configuredServices.end()){
					std::size_t lastDot = (*kt)->getOrder().find_last_of(".");
					
					if (std::string::npos != lastDot){
						std::string parentOrder = (*kt)->getOrder().substr(0, lastDot);
						std::map<std::string, Method*>::iterator mt = servicesNestedCalls.find(parentOrder);
						if (servicesNestedCalls.end() != mt && mt->second == *jt){
							output << "\t" << "static Data_" << (*kt)->getCallee()->getName() << "_" << (*kt)->getService()->getName() << " data_" << transactionCount << ";" << std::endl;
							output << "\t" << "static Data_" << (*kt)->getCallee()->getName() << "_" << (*kt)->getService()->getName() << "_request request_" << transactionCount << ";" << std::endl;
				
							output << std::endl;
							
							++transactionCount;
						}
					}
					
					configuredServices.push_back((*kt)->getService());
				}
			}*/
			
			int transactionCount = 1;
			configuredServices.clear();
			output << "\t" << "if(" << (*jt)->getName() << "_stateData.STATE != " << (*jt)->getName() << "_SERVICE_STATE::WAITING_READ && " << (*jt)->getName() << "_stateData.STATE != " << (*jt)->getName() << "_SERVICE_STATE::WAITING_WRITE){";
			for (std::list<Transaction*>::iterator kt = transactions->begin(); kt != transactions->end(); ++kt){
				if ((*kt)->getCaller() == *it && std::find(configuredServices.begin(), configuredServices.end(), (*kt)->getService()) == configuredServices.end()){
					std::size_t lastDot = (*kt)->getOrder().find_last_of(".");
					
					if (std::string::npos != lastDot){
						std::string parentOrder = (*kt)->getOrder().substr(0, lastDot);
						std::map<std::string, Method*>::iterator mt = servicesNestedCalls.find(parentOrder);
						if (servicesNestedCalls.end() != mt && mt->second == *jt){
							output << std::endl;

							output << "\t\t" << "if(" << (*jt)->getName() << "_stateData.request_" << transactionCount << ".ready){" << std::endl;
							output << "\t\t\t" << "" << (*jt)->getName() << "_stateData.request_" << transactionCount << ".ready = false;" << std::endl;
							output << "\t\t\t" << "" << (*jt)->getName() << "_stateData.LAST_STATE = " << (*jt)->getName() << "_stateData.STATE;" << std::endl;
							output << "\t\t\t" << "" << (*jt)->getName() << "_stateData.STATE = " << (*jt)->getName() << "_SERVICE_STATE::TRANSACTION_" << transactionCount << "_RECEIVE;" << std::endl;
							output << "\t\t" << "}" << std::endl;
							
							++transactionCount;
						}
					}
					
					configuredServices.push_back((*kt)->getService());
				}
			}
			output << "\t" << "}" << std::endl;
			
			int totalTransactions = transactionCount - 1;
			
			// End of transaction data
			
			output << std::endl;
				
			output << "\t" << "switch(" << (*jt)->getName() << "_stateData.STATE){" << std::endl;
			
			output << "\t\t" << "case " << (*jt)->getName() << "_SERVICE_STATE::WAITING_READ:" << std::endl;
			output << "\t\t\t" << "if(port_" << (*jt)->getName() << "_receive->nb_read(NULL, " << (*jt)->getName() << "_stateData.obj, " << (*jt)->getName() << "_stateData.data)){" << std::endl;
			output << "\t\t\t\t" << (*jt)->getName() << "_stateData.STATE = " << (*jt)->getName() << "_SERVICE_STATE::SERVICE_START;" << std::endl;
			
			transactionCount = 1;
			configuredServices.clear();
			for (std::list<Transaction*>::iterator kt = transactions->begin(); kt != transactions->end(); ++kt){
				if ((*kt)->getCaller() == *it && std::find(configuredServices.begin(), configuredServices.end(), (*kt)->getService()) == configuredServices.end()){
					std::size_t lastDot = (*kt)->getOrder().find_last_of(".");
					
					if (std::string::npos != lastDot){
						std::string parentOrder = (*kt)->getOrder().substr(0, lastDot);
						std::map<std::string, Method*>::iterator mt = servicesNestedCalls.find(parentOrder);
						if (servicesNestedCalls.end() != mt && mt->second == *jt){
							output << "\t\t\t\t" << (*jt)->getName() << "_stateData.request_" << transactionCount << ".ready = false;" << std::endl;

							++transactionCount;
						}
					}
					
					configuredServices.push_back((*kt)->getService());
				} 
			}
			
			
			output << "\t\t\t" << "}" << std::endl;
			output << "\t\t\t" << "break;" << std::endl;
			
			output << "\t\t" << "case " << (*jt)->getName() << "_SERVICE_STATE::SERVICE_START:" << std::endl;
			output << "\t\t\t" << (*jt)->getName() << "_start(";
			bool firstParam = true;
            for (std::list<Parameter*>::iterator kt = (*jt)->getParameters()->begin(); kt != (*jt)->getParameters()->end(); ++kt){
                if(!firstParam)
                    output << ", ";
                
                output << (*jt)->getName() << "_stateData.data." << (*kt)->getName();
                
                firstParam = false;
            }
			output << ");" << std::endl;
			if (totalTransactions > 0)
			{
				output << "\t\t\t" << (*jt)->getName() << "_stateData.STATE = " << (*jt)->getName() << "_SERVICE_STATE::TRANSACTION_1_REQUEST_PARAMS;" << std::endl;
			}
			else
			{
				output << "\t\t\t" << (*jt)->getName() << "_stateData.STATE = " << (*jt)->getName() << "_SERVICE_STATE::SERVICE_FINISH;" << std::endl;
			}
			output << "\t\t\t" << "break;" << std::endl;
			
			// Start of transactions states
			transactionCount = 1;
			configuredServices.clear();
			for (std::list<Transaction*>::iterator kt = transactions->begin(); kt != transactions->end(); ++kt){
				if ((*kt)->getCaller() == *it && std::find(configuredServices.begin(), configuredServices.end(), (*kt)->getService()) == configuredServices.end()){
					std::size_t lastDot = (*kt)->getOrder().find_last_of(".");
					
					if (std::string::npos != lastDot){
						std::string parentOrder = (*kt)->getOrder().substr(0, lastDot);
						std::map<std::string, Method*>::iterator mt = servicesNestedCalls.find(parentOrder);
						if (servicesNestedCalls.end() != mt && mt->second == *jt){
							output << "\t\t" << "case " << (*jt)->getName() << "_SERVICE_STATE::TRANSACTION_" << transactionCount << "_REQUEST_PARAMS:" << std::endl;
							output << "\t\t\t" << (*jt)->getName() << "_stateData.data_" << transactionCount << " = " << (*jt)->getName() << "_" << (*kt)->getCallee()->getName() << "_" << (*kt)->getService()->getName() << "_send();" << std::endl;
							output << "\t\t\t" << (*jt)->getName() << "_stateData.STATE = " << (*jt)->getName() << "_SERVICE_STATE::TRANSACTION_" << transactionCount << "_SEND;" << std::endl;
							output << "\t\t\t" << "break;" << std::endl;

							output << "\t\t" << "case " << (*jt)->getName() << "_SERVICE_STATE::TRANSACTION_" << transactionCount << "_SEND:" << std::endl;
							output << "\t\t\t" << "if(service_" << (*kt)->getCallee()->getName() << "_" << (*kt)->getService()->getName() << "(" << (*jt)->getName() << "_stateData.data_" << transactionCount << ", &" << (*jt)->getName() << "_stateData.request_" << transactionCount << "))" << std::endl;
							output << "\t\t\t\t" << (*jt)->getName() << "_stateData.STATE = " << (*jt)->getName() << "_SERVICE_STATE::TRANSACTION_" << transactionCount << "_WAIT;" << std::endl;
							output << "\t\t\t" << "break;" << std::endl;

							output << "\t\t" << "case " << (*jt)->getName() << "_SERVICE_STATE::TRANSACTION_" << transactionCount << "_WAIT:" << std::endl;
							if((*kt)->getTransactionType() == ASYNC)
								if(totalTransactions > transactionCount)
									output << "\t\t\t" << (*jt)->getName() << "_stateData.STATE = " << (*jt)->getName() << "_SERVICE_STATE::TRANSACTION_" << (transactionCount+1) << "_REQUEST_PARAMS;" << std::endl;
								else
									output << "\t\t\t" << (*jt)->getName() << "_stateData.STATE = " << (*jt)->getName() << "_SERVICE_STATE::SERVICE_FINISH;" << std::endl;
							output << "\t\t\t" << "break;" << std::endl;

							output << "\t\t" << "case " << (*jt)->getName() << "_SERVICE_STATE::TRANSACTION_" << transactionCount << "_RECEIVE:" << std::endl;
							output << "\t\t\t" << (*jt)->getName() << "_" << (*kt)->getCallee()->getName() << "_" << (*kt)->getService()->getName() << "_receive(" << (*jt)->getName() << "_stateData.request_" << transactionCount << ".response);" << std::endl;
							if(totalTransactions > transactionCount)
								output << "\t\t\t" << (*jt)->getName() << "_stateData.STATE = (" << (*jt)->getName() << "_stateData.LAST_STATE == " << (*jt)->getName() << "_SERVICE_STATE::TRANSACTION_" << transactionCount << "_WAIT) ? " << (*jt)->getName() << "_SERVICE_STATE::TRANSACTION_" << (transactionCount+1) << "_REQUEST_PARAMS : " << (*jt)->getName() << "_stateData.LAST_STATE;" << std::endl;
							else
								output << "\t\t\t" << (*jt)->getName() << "_stateData.STATE = (" << (*jt)->getName() << "_stateData.LAST_STATE == " << (*jt)->getName() << "_SERVICE_STATE::TRANSACTION_" << transactionCount << "_WAIT) ? " << (*jt)->getName() << "_SERVICE_STATE::SERVICE_FINISH : " << (*jt)->getName() << "_stateData.LAST_STATE;" << std::endl;
							output << "\t\t\t" << "break;" << std::endl;

							++transactionCount;
						}
					}
					
					configuredServices.push_back((*kt)->getService());
				} 
			}
			// End of transactions states
			
			output << "\t\t" << "case " << (*jt)->getName() << "_SERVICE_STATE::SERVICE_FINISH:" << std::endl;
			output << "\t\t\t" << (*jt)->getName() << "_stateData.ret = " << (*jt)->getName() << "_finish();" << std::endl;
			output << "\t\t\t" << (*jt)->getName() << "_stateData.STATE = " << (*jt)->getName() << "_SERVICE_STATE::WAITING_WRITE;" << std::endl;
			output << "\t\t\t" << "break;" << std::endl;
			
			output << "\t\t" << "case " << (*jt)->getName() << "_SERVICE_STATE::WAITING_WRITE:" << std::endl;
			output << "\t\t\t" << "if(port_" << (*jt)->getName() << "_send->nb_write(" << (*jt)->getName() << "_stateData.obj, " << (*jt)->getName() << "_stateData.ret))" << std::endl;
			output << "\t\t\t\t" << (*jt)->getName() << "_stateData.STATE = " << (*jt)->getName() << "_SERVICE_STATE::EXIT;" << std::endl;
			output << "\t\t\t" << "break;" << std::endl;
			
			output << "\t\t" << "case " << (*jt)->getName() << "_SERVICE_STATE::EXIT:" << std::endl;
			
			output << "\t\t\t" << "if(true";
			std::list<Entry*> exits;
			{
				configuredServices.clear();
				for (std::list<Frame*>::iterator jt = frames->begin(); jt != frames->end(); ++jt){
					std::list<Entry*> subexits = getExits(*it, *jt, configuredServices);
					
					exits.insert(exits.end(), subexits.begin(), subexits.end());
				}
			}
			for (std::list<Entry*>::iterator kt = exits.begin(); kt != exits.end(); ++kt){
				output << "	&& _FLOWEXEC.EXIT(exit_id_" << (*kt)->getMethod()->getName() << ")";
			}
			output << ")" << std::endl;
			output << "\t\t\t\t" << (*jt)->getName() << "_stateData.STATE = " << (*jt)->getName() << "_SERVICE_STATE::WAITING_READ;" << std::endl;
			output << "\t\t\t" << "break;" << std::endl;
			output << "\t" << "}" << std::endl;
			
            output << "}" << std::endl;
			
        	output << std::endl;
        }
                
		// Used services
		configuredServices.clear();
		for (std::list<Element*>::iterator mt = (*it)->getInstances()->begin(); mt != (*it)->getInstances()->end(); ++mt)
			for (std::list<Transaction*>::iterator nt = transactions->begin(); nt != transactions->end(); ++nt)
				if((*nt)->getCaller() == *it && std::find(configuredServices.begin(), configuredServices.end(), (*nt)->getService()) == configuredServices.end()){
					// check if the current instance uses any service
					bool useInstance = false;
					for (std::list<std::string*>::iterator ot = (*nt)->getNameCaller()->begin(); ot != (*nt)->getNameCaller()->end(); ++ot)
						if(!(*ot)->compare((*mt)->getName()))
							useInstance = true;

					if(useInstance){
						output << "bool " << (*it)->getName() << "::service_" << (*nt)->getCallee()->getName() << "_" << (*nt)->getService()->getName() << "(Data_" << (*nt)->getCallee()->getName() << "_" << (*nt)->getService()->getName() << " data, " << "Data_" << (*nt)->getCallee()->getName() << "_" << (*nt)->getService()->getName() << "_request *request){" << std::endl;
						
						output << "\t" << "request->ready = false;" << std::endl;
						output << std::endl;
						
						output << "\t" << "if(port_" << (*nt)->getCallee()->getName() << "_" << (*nt)->getService()->getName() << "_send->nb_write(this, data)){" << std::endl;
						output << "\t\t" << "sc_spawn(sc_bind(&" << (*it)->getName() << "::service_" << (*nt)->getCallee()->getName() << "_" << (*nt)->getService()->getName() << "_checkRequest, this, request));" << std::endl;
						output << "\t\t" << "return true;" << std::endl;
						output << "\t" << "} else " << std::endl;
						output << "\t\t" << "return false; " << std::endl;
						
						output << "}" << std::endl;
						
						
						output << std::endl;
						
						
						output << "void " << (*it)->getName() << "::service_" << (*nt)->getCallee()->getName() << "_" << (*nt)->getService()->getName() << "_checkRequest(Data_" << (*nt)->getCallee()->getName() << "_" << (*nt)->getService()->getName() << "_request *request){" << std::endl;
						
						output << "\t" << "request->response = port_" << (*nt)->getCallee()->getName() << "_" << (*nt)->getService()->getName() << "_receive->read(this);" << std::endl;
						output << "\t" << "request->ready = true;" << std::endl;
						
						output << "}" << std::endl;
						
        				output << std::endl;
						
						configuredServices.push_back((*nt)->getService());
					}
				}
	
        /*
         * Execution flow - Entry/Exit points
         */
		{
			std::list<Method*> configuratedMethods;
			for (std::list<Frame*>::iterator jt = frames->begin(); jt != frames->end(); ++jt){
				Frame* frame = *jt;

				createSystemSourceFiles_createEntryPoints(output, *it, frame, 1, "", configuratedMethods);
			}
		}
        
        output << std::endl;
        
        output.close();
    }
}
