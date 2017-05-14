#include <cstdio>
#include <iostream>
#include <string>
#include <list>
#include <algorithm>   
#include <iterator> 
#include <iostream>
#include <fstream>
#include <vector>
#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <string>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>

#include "Module.hpp"
#include "Attribute.hpp"
#include "Method.hpp"
#include "Parameter.hpp"
#include "Service.hpp"
#include "Transaction.hpp"
#include "Element.hpp"
#include "Restriction.hpp"
#include "Dependency.hpp"
#include "Frame.hpp"
#include "Device.hpp"
#include "Artifact.hpp"
#include "Component.hpp"
#include "ExecutionEnvironment.hpp"
#include "ParserErrorHandler.hpp"


///Trecho do SYSTEM C 

void writeMain(std::list<Module*> *mylistModule, std::list<Transaction*> *mylistTransaction){
	std::ofstream* main_cpp = new std::ofstream("output/main.cpp", std::ios::app);;
	for (std::list<Module*>::iterator modules = mylistModule->begin(); modules != mylistModule->end(); ++modules){
		*main_cpp<<"#include \""<<((*modules)->getName()).c_str()<<".h\""<<std::endl;

	}
	*main_cpp<<std::endl;
	*main_cpp<<"#include \"MultiAccessChannel.h\""<<std::endl;

	*main_cpp<<std::endl;
	*main_cpp<<"#include <iostream>"<<std::endl;
	*main_cpp<<"#include <systemc>"<<std::endl;
	*main_cpp<<std::endl;

	*main_cpp<<"#define CHANNELS_SIZE 20"<<std::endl;
	*main_cpp<<std::endl;
	*main_cpp<<"int sc_main(int argc, char* argv[]) {"<<std::endl;
	*main_cpp<<std::endl;
	int c_number = 1;
	//channel

	for (std::list<Module*>::iterator modules = mylistModule->begin(); modules != mylistModule->end(); ++modules){
		for (std::list<Element*>::iterator elements = ((*modules)->getInstances())->begin();elements != ((*modules)->getInstances())->end();++elements)
		{
			*main_cpp<<"	"<<(*modules)->getName()<<" "<<(*elements)->getName()<<"(\""<<(*elements)->getName()<<"\");"<<std::endl;
		}
	}
	*main_cpp<<std::endl;

	for (std::list<Transaction*>::iterator transactions = mylistTransaction->begin(); transactions != mylistTransaction->end(); ++transactions){

			std::string name_callee = (*transactions)->getCallee()->getName();
			std::string name_service = (*transactions)->getService()->getName();
			*main_cpp<<"	MultiAccessChannel<";
			*main_cpp<<"Data_"<<name_callee<<"_"<<name_service<<" > channel_";
			*main_cpp<<name_callee<<"_"<<name_service<<"_to(\"c"<<c_number<<"\", CHANNELS_SIZE);"<<std::endl; 
			c_number++;


			//channel from
			std::string type_channel = (*transactions)->getService()->getType();
			*main_cpp<<"	MultiAccessChannel";
			*main_cpp<<"< "<<type_channel<<" > channel_"<<name_callee;
			*main_cpp<<"_"<<name_service<<"_"<<"from(\"c"<<c_number<<"\", CHANNELS_SIZE);"<<std::endl;
			*main_cpp<<std::endl;

			std::list<std::string*>::iterator nameCaller = ((*transactions)->getNameCaller())->begin();
			for (; nameCaller != ((*transactions)->getNameCaller())->end(); ++nameCaller){

				*main_cpp<<"	"<<(*nameCaller)->c_str()<<".port_"<<(*transactions)->getNameCallee()<<"_"<<name_service;
				*main_cpp<<"_send(channel_"<<name_callee<<"_"<<name_service<<"_"<<"to);"<<std::endl;	

				*main_cpp<<"	"<<(*nameCaller)->c_str()<<".port_"<<(*transactions)->getNameCallee()<<"_"<<name_service;
				*main_cpp<<"_receive(channel_"<<name_callee<<"_"<<name_service<<"_"<<"from);"<<std::endl;
				
				*main_cpp <<std::endl;
			}

			*main_cpp<<"	"<<(*transactions)->getNameCallee()<<".port_"<<name_service;
			*main_cpp<<"_send(channel_"<<name_callee<<"_"<<name_service<<"_"<<"from);"<<std::endl;	

			*main_cpp<<"	"<<(*transactions)->getNameCallee()<<".port_"<<name_service;
			*main_cpp<<"_receive(channel_"<<name_callee<<"_"<<name_service<<"_"<<"to);"<<std::endl;
			*main_cpp<<std::endl;
		
	}

	*main_cpp<<"	sc_start();"<<std::endl;
	*main_cpp<<"}"<<std::endl;
	(*main_cpp).close();
}


void write_methods_h(std::list<Method*> *ListMethod , std::ofstream *myfile_h ){
	for (std::list<Method*>::iterator methods = ListMethod->begin(); methods != ListMethod->end(); ++methods){
		std::string type = (((*methods)->getType()).c_str());
		*myfile_h <<"		"<<type.c_str()<<" "<<(*methods)->getName()<<"(";
		std::list<Parameter*>::iterator parameters = ((*methods)->getParameters())->begin();
		int sz = ((*methods)->getParameters())->size();
		if(sz == 0){
			*myfile_h<<");"<<std::endl;
		}
		for (parameters; parameters != ((*methods)->getParameters())->end(); ++parameters){
			sz = --sz;
			*myfile_h <<(*parameters)->getType().c_str()<<" "<<(*parameters)->getName().c_str();
			if(sz > 0){
				*myfile_h<<", ";
			}else{
				*myfile_h<<");"<<std::endl;
			}
		}
	}
	*myfile_h<<std::endl;	
}

int checkFrame(std::list<Frame*> *mylistFrame, std::string name,int ret){
	for (std::list<Frame*>::iterator it=mylistFrame->begin(); it != mylistFrame->end(); ++it){
  		for (std::list<Transaction*>::iterator it2=((*it)->getTransactions())->begin(); it2 != ((*it)->getTransactions())->end(); ++it2){
  			if((*it2)->getCaller()->getName() == name){
  				ret++;
  				return ret;
  			}
  		}
  		ret = checkFrame((*it)->getFrames(),name, ret);
  	}
  	return ret;

}

void writeFrame (std::list<Frame*> *mylistFrame, std::ofstream *myfile,std::string name, int start,int qtd){
	int done = 0;
    for (std::list<Frame*>::iterator it=mylistFrame->begin(); it != mylistFrame->end(); ++it){
    	done = 0;
    	if(checkFrame((*it)->getFrames(),name, 0)>0){
    		done++;
			if((*it)->getFrameType() == 2){
				*myfile <<"		LOOP{"<<std::endl;
				qtd++;
			}else if((*it)->getFrameType()==1){
				*myfile <<"		PARALLEL_START"<<std::endl;
				*myfile <<"		PARALLEL_BLOCK {"<<std::endl;
				start++;
				qtd++;
			}
		}
		int aux=0;
  		for (std::list<Transaction*>::iterator it2=((*it)->getTransactions())->begin(); it2 != ((*it)->getTransactions())->end(); ++it2){
  				//se for chamada interna
  				std::string type_service;
				std::vector<std::string>  name_parameters; 
				std::vector<std::string>  type_parameters;
				std::list<Parameter*>::iterator parameters;
				std::vector<std::string>::iterator aux_name;
				std::vector<std::string>::iterator aux_type;
				std::string name_ret="";
				int sz;
				std:: string type_ret;
				if((*it2)->getCaller()->getName() == name ){
					if(done ==0){
						if((*it)->getFrameType() == 2){
							*myfile <<"		LOOP{"<<std::endl;
						}else if((*it)->getFrameType()==1){
							aux++;
							*myfile <<"		PARALLEL_START"<<std::endl;
							*myfile <<"		PARALLEL_BLOCK {"<<std::endl;
						}
					}
					if(((*it2)->getTransactionType()) == 0){
						type_service = "sync";
						std::string nameCallee = (*it2)->getNameCallee();
						parameters = (((*it2)->getService())->getParameters())->begin();
						sz = (((*it2)->getService())->getParameters())->size();
						for (parameters; parameters != (((*it2)->getService())->getParameters())->end(); ++parameters){
							type_parameters.push_back((*parameters)->getType().c_str());
							name_parameters.push_back((*parameters)->getName().c_str());
						}
						type_ret = (((*it2)->getService())->getType());
						aux_name = (name_parameters).begin();
						aux_type = (type_parameters).begin();
						for (aux_name; aux_name != (name_parameters).end(); ++aux_name){
							*myfile <<"		"<<(*aux_type)<<" "<<(*aux_name)<<";"<<std::endl;
							++aux_type;
						}
						*myfile<<"		"<<(*it2)->getService()->getType()<<" * data;"<<std::endl;
						*myfile <<"		service_"<<nameCallee<<"_"<<(*it2)->getService()->getName()<<"_"<<type_service<<"(";
	  					aux_name = (name_parameters).begin();
	  					sz = (name_parameters).size();
						for (aux_name; aux_name != (name_parameters).end(); ++aux_name){
							if(sz == 1){
								*myfile <<(*aux_name)<<", data);"<<std::endl;
							}else{
								*myfile <<(*aux_name)<<", ";
							}
							sz--;
						}
						name_parameters.clear();
						type_parameters.clear();
						}else{
							type_service = "async";
							std::string nameCallee = (*it2)->getNameCallee();
							parameters = (((*it2)->getService())->getParameters())->begin();
							sz = (((*it2)->getService())->getParameters())->size();
							for (parameters; parameters != (((*it2)->getService())->getParameters())->end(); ++parameters){
								type_parameters.push_back((*parameters)->getType().c_str());
								name_parameters.push_back((*parameters)->getName().c_str());
							}
							type_ret = (((*it2)->getService())->getType());
							aux_name = (name_parameters).begin();
							aux_type = (type_parameters).begin();
							for (aux_name; aux_name != (name_parameters).end(); ++aux_name){
								*myfile <<"		"<<(*aux_type)<<" "<<(*aux_name)<<";"<<std::endl;
								++aux_type;
							}
							*myfile <<"		service_"<<nameCallee<<"_"<<(*it2)->getService()->getName()<<"_"<<type_service<<"(";
							aux_name = (name_parameters).begin();
	  						sz = (name_parameters).size();
							for (aux_name; aux_name != (name_parameters).end(); ++aux_name){
								*myfile <<(*aux_name)<<", ";
							}
							name_parameters.clear();
							type_parameters.clear();
							*myfile<<"CALLBACK(";
							*myfile<<(*it2)->getService()->getType()<<" data";
							*myfile<<"){"<<std::endl;
							*myfile<<"		//put here your code"<<std::endl;
							*myfile<<"		});"<<std::endl;
					}
					if(done==0)
						*myfile<<"		}"<<std::endl;
  				}
  		}
  		if(aux>0){
  			*myfile<<"		PARALLEL_END"<<std::endl;
  		}
  		writeFrame((*it)->getFrames(),myfile,name,start,0);
  		while(qtd-1>0){
  			if(start>0){
  				*myfile<<"		}"<<std::endl;
  				*myfile<<"		PARALLEL_END"<<std::endl;
  				start=0;
  			}
  			else{
  				*myfile<<"		}"<<std::endl;
  			}
  			qtd--;
  		}
  	}
}


void writeClass (std::list<Module*> *mylistModule, std::list<Transaction*> *mylistTransaction, std::list<std::string*> *ListNewType, std::list<Frame*> *mylistFrame){
	std::vector<std::ofstream*> myFiles;
	std::vector<std::ofstream*> myFiles_h;
	std::ofstream* data_h;
	int n_files = 0;
	int data_n =0;
	for (std::list<Module*>::iterator modules = mylistModule->begin(); modules != mylistModule->end(); ++modules){
		//creating .cpp
    	std::string name = (((*modules)->getName()).c_str());
    	std::string out = "output/";
    	strcat((char*)name.c_str(),".cpp");
    	strcat((char*)out.c_str(),(char*)name.c_str());
    	myFiles.push_back(new std::ofstream(out.c_str(), std::ios::app));
    	n_files++;
    	std::ofstream *myfile = myFiles[n_files-1];
    	std::string out_h = "output/";
		name = (((*modules)->getName()).c_str());
		strcat((char*)name.c_str(),".h");
		strcat((char*)out_h.c_str(),(char*)name.c_str());
		myFiles_h.push_back(new std::ofstream(out_h.c_str(), std::ios::app));
    	std::ofstream *myfile_h = myFiles_h[n_files-1];

    	//verificar se existe algum serviço com essa classe
		std::list<Transaction*> ListIsCallee; 
		std::list<Transaction*> ListIsCaller;
		std::list<Transaction*>::iterator transactions = ((mylistTransaction))->begin();
		int iscallee,iscaller = 0;
		for(transactions; transactions != ((mylistTransaction))->end(); ++transactions){
			if((*transactions)->getCaller()->getName() == name){
				iscaller++;
				ListIsCaller.push_back((*transactions));
			}else if((*transactions)->getCallee()->getName() == name){
				iscallee++;
				ListIsCallee.push_back((*transactions));
			}
		}
    	*myfile_h << "#define SC_INCLUDE_DYNAMIC_PROCESSES"<<std::endl;
    	*myfile_h << "#include <stdio.h>"<<std::endl;
    	*myfile_h << "#include <systemc.h>"<<std::endl;
    	*myfile_h << "#include \"Defines.h\""<<std::endl;
    	*myfile_h << "#include \"MultiAccessInterface.h\""<<std::endl;
    	if(!ListIsCallee.empty() || !ListIsCaller.empty()){
			*myfile_h << "#include \"Data.h\""<<std::endl;
			data_n++;
			if(data_n == 1){
				data_h = new std::ofstream("out/Data.h", std::ios::app);
		    	//writing the data.h file
				*data_h << "#define SC_INCLUDE_DYNAMIC_PROCESSES"<<std::endl;
    			*data_h << "#include <stdio.h>"<<std::endl;
    			*data_h << "#include <systemc.h>"<<std::endl;
    			*data_h << "#ifndef DATA_H"<<std::endl;
    			*data_h << "#define DATA_H"<<std::endl;
    			*data_h <<std::endl;
    			*data_h <<std::endl;
			}
		}
    	*myfile_h << ""<<std::endl;
		std::string name_boost = name;	
		boost::to_upper(name_boost);
		*myfile_h << "#ifndef "<<name_boost<<"_H"<<std::endl;
		*myfile_h << "#define "<<name_boost<<"_H"<<std::endl;
		*myfile_h <<""<<std::endl;
		*myfile_h <<"class " <<name<<" : public sc_module {"<<std::endl;
		*myfile_h <<std::endl;

		*myfile_h <<"	public:"<<std::endl;
		*myfile_h <<std::endl;
		*myfile_h <<"		"<<name<<"(sc_module_name name);"<<std::endl;
		*myfile_h <<"		"<<"void run();"<<std::endl;

    	name = (((*modules)->getName()).c_str());

    	//writing the file
		*myfile << "#include "<< "\""<<name <<".h"<<"\"" << ""<<std::endl;
		//constructor
		*myfile <<std::endl;
		*myfile <<name <<"::"<<name<<"(sc_module_name name) : sc_module(name){"<<std::endl;
		*myfile <<"	SC_HAS_PROCESS("<<name<<");"<<std::endl;
		*myfile <<"	SC_THREAD(run);"<<std::endl;
		*myfile <<"}"<<std::endl;
		*myfile <<std::endl;

		std::string name_ret="";
		std::list<Method*> ListMethodPrivate; 
		std::list<Method*> ListMethodProtected;
		for (std::list<Method*>::iterator methods = ((*modules)->getMethods())->begin(); methods != ((*modules)->getMethods())->end(); ++methods){
				std::string type = (((*methods)->getType()).c_str());

				//method public
				if((((*methods)->getModifier())) == 2){
					*myfile_h <<"		"<<type.c_str()<<" "<<(*methods)->getName()<<"(";
				}else if((((*methods)->getModifier())) == 1){
					ListMethodPrivate.push_back((*methods));
				}else{
					ListMethodProtected.push_back((*methods));
				}
				*myfile <<type.c_str()<<" "<<name<<"::"<<(*methods)->getName()<<"(";
				std::list<Parameter*>::iterator parameters = ((*methods)->getParameters())->begin();
				int sz = ((*methods)->getParameters())->size();
				if(sz == 0){
					*myfile<<") {"<<std::endl;
					*myfile<<""<<std::endl;
					*myfile<<"//put your code here"<<std::endl;
					if( (*methods)->getService() == 1) {
						*data_h<<"typedef struct {"<<std::endl;
						*data_h<<std::endl;
						*data_h<<"} Data_"<<name<<"_"<<(*methods)->getName()<<";"<<std::endl;
						*data_h<<std::endl;
						//verificar para o caso de não ter nenhum atributo no serviço
						*myfile<<"	void* obj;"<<std::endl;
						*myfile<<"	Data_"<<name<<"_"<<(*methods)->getName()<<" data = ";
						*myfile<<"	port_"<<(*methods)->getName()<<"_receive->read(NULL, obj);"<<std::endl;
						*myfile<<"	"<<type<<" ret;"<<std::endl;
						*myfile<<"	port_"<<(*methods)->getName()<<"_send->write(obj, ret);"<<std::endl;
						if((((*methods)->getModifier())) == 2){
							*myfile_h <<");"<<std::endl;
							*myfile_h<<"		sc_port< read_ma< Data_"<<name<<"_"<<(*methods)->getName()<<" > >port_"<<(*methods)->getName()<<"_receive;"<<std::endl;
							*myfile_h<<"		sc_port< write_ma< "<<type<<" > >port_"<<(*methods)->getName()<<"_send;"<<std::endl;
						}
					}
					else{
						*myfile_h <<");"<<std::endl;
					}
					*myfile<<"}"<<std::endl;
				}else{
					if( (*methods)->getService() == 1) {
						*data_h<<"typedef struct {"<<std::endl;
					}
					for (parameters; parameters != ((*methods)->getParameters())->end(); ++parameters){
						sz = sz-1;
						if((((*methods)->getModifier())) == 2){
							*myfile_h <<(*parameters)->getType().c_str()<<" "<<(*parameters)->getName().c_str();
						}
						if( (*methods)->getService() == 1) {
							*data_h<<"	"<<(*parameters)->getType().c_str()<<" "<<(*parameters)->getName().c_str()<<";"<<std::endl;
						}
						*myfile <<(*parameters)->getType().c_str()<<" "<<(*parameters)->getName().c_str();
						if(sz > 0){
							*myfile<<", ";
							if((((*methods)->getModifier())) == 2){
								*myfile_h<<",";
							}
						}else{

							*myfile<<") {"<<std::endl;
							*myfile<<"	//put your code here"<<std::endl;
							//verifica se esse metodo eh um servico
							if( (*methods)->getService() == 1) {
								*data_h<<std::endl;
								*data_h<<"} Data_"<<name<<"_"<<(*methods)->getName()<<";"<<std::endl;
								*data_h<<std::endl;
								*myfile<<"	void* obj;"<<std::endl;
								*myfile<<"	Data_"<<name<<"_"<<(*methods)->getName()<<" data = ";
								*myfile<<" port_"<<(*methods)->getName()<<"_receive->read(NULL, obj);"<<std::endl;
								*myfile<<"	"<<type<<" ret;"<<std::endl;
								*myfile<<"	port_"<<(*methods)->getName()<<"_send->write(obj,ret);"<<std::endl;
								if((((*methods)->getModifier())) == 2){
									*myfile_h <<");"<<std::endl;
									*myfile_h<<"		sc_port< read_ma< Data_"<<name<<"_"<<(*methods)->getName()<<" > >port_"<<(*methods)->getName()<<"_receive;"<<std::endl;
									*myfile_h<<"		sc_port< write_ma< "<<type<<" > >port_"<<(*methods)->getName()<<"_send;"<<std::endl;
								}
							}else{
								if((((*methods)->getModifier())) == 2){
									*myfile_h <<");"<<std::endl;
								}
							}
							*myfile<<"}"<<std::endl;
						}
					}
				}
			}
		//se ele for caller tem que colocar as funcoes sync e assync no run
		//se ele for callee tem que colocar o write
		*myfile <<"void"<<" "<<name<<"::"<<"run"<<"(){"<<std::endl;
		*myfile <<"	while(true){"<<std::endl;
		*myfile <<"		//put your code here"<<std::endl;
		///TENTANDO COLOCAR OS LOOPS, PAR, SEQ...
		//writeFrame(mylistFrame,myfile,name,0,0);
		std::list<Transaction*>::iterator callers = (ListIsCaller).begin();
		std::list<Transaction*>::iterator callees = (ListIsCallee).begin();
		std::string callee;
		std::string name_service;
		std::string type_service;
		std::vector<std::string>  name_parameters; 
		std::vector<std::string>  type_parameters;
		std::list<Parameter*>::iterator parameters;
		std::vector<std::string>::iterator aux_name;
		std::vector<std::string>::iterator aux_type;
		//std::string type_parameters;
		int aux = 0;

		//VERIFICAÇÃO PARA O CASO DO MODULO FORNERCER ALGUM SERVIÇO
		if(iscallee>0){
			if(iscaller == 0){ //if the class isn't caller too, close the class 
				*myfile <<"	}"<<std::endl;
				*myfile <<"}"<<std::endl;
			}
		}

		//VERIFICAÇÃO CASO O MODULO CHAME ALGUMA FUNÇÃO DE OUTRO METODO
		if(iscaller>0){
			int sz =0;
			std:: string type_ret;
			type_ret = (((*callers)->getService())->getType());
			callers =(ListIsCaller).begin();
			int close_run =0;
			for(callers; callers != (ListIsCaller).end(); ++callers){
				callee = ((*callers)->getNameCallee());
				name_service = (((*callers)->getService())->getName());
				type_ret = (((*callers)->getService())->getType());
				if(((*callers)->getTransactionType()) == 0){
					type_service = "sync";
				}else{
					type_service = "assync";
				}
				parameters = (((*callers)->getService())->getParameters())->begin();
				sz = (((*callers)->getService())->getParameters())->size();
				for (parameters; parameters != (((*callers)->getService())->getParameters())->end(); ++parameters){
					type_parameters.push_back((*parameters)->getType().c_str());
					name_parameters.push_back((*parameters)->getName().c_str());
				}
				if(close_run ==0){
					*myfile <<"	}"<<std::endl;
					*myfile <<"}"<<std::endl; //close the run
					close_run++;
				}	
				aux_name = (name_parameters).begin();
				aux_type = (type_parameters).begin();
				*myfile <<"void "<<name<<"::service_sync_"<<callee<<"_"<<name_service<<"(";
				sz = (name_parameters).size();
				aux_type = (type_parameters).begin();
				for (aux_name; aux_name != (name_parameters).end(); ++aux_name){
					sz--;
					if(sz == 0){
						*myfile <<(*aux_type)<<" "<<(*aux_name)<<", "<<type_ret<<" "<<"*ret){"<<std::endl;
					}else{
						*myfile <<(*aux_type)<<" "<<(*aux_name)<<", ";
					}
					++aux_type;	
				}

				*myfile_h <<"		sc_port< read_ma< " <<((*callers)->getService())->getType()<<" > >"<<"port_"<<callee<<"_"<<name_service<<"_receive;"<<std::endl;
				
				*myfile <<"	Data_"<<(((*callers)->getCallee())->getName()).c_str()<<"_"<<name_service<<" data;"<<std::endl;
				aux_name = (name_parameters).begin();
				aux_type = (type_parameters).begin();
				aux_type = (type_parameters).begin();
				for (aux_name; aux_name != (name_parameters).end(); ++aux_name){
					*myfile <<"	data.";
					*myfile <<(*aux_name) <<" = "<<(*aux_name) <<";"<<std::endl;
				}
				*myfile_h <<"		sc_port< write_ma< Data_"<<(((*callers)->getCallee())->getName()).c_str()<<"_"<<name_service<<" > >";
				*myfile_h <<"port_"<<callee<<"_"<<name_service<<"_send;"<<std::endl;
				*myfile <<"	port_"<<callee<<"_"<<name_service<<"_send->write(this, data);"<<std::endl;
				aux_name = (name_parameters).begin();

				*myfile <<"	*ret = port_"<<callee<<"_"<<name_service<<"_receive->read(this);"<<std::endl;
				*myfile <<"}"<<std::endl;

				*myfile <<"void "<<name<<"::service_async_"<<callee<<"_"<<name_service<<"(";
				sz = (name_parameters).size();
				aux_type = (type_parameters).begin();
				for (aux_name; aux_name != (name_parameters).end(); ++aux_name){
					sz--;
					if(sz == 0){
						*myfile <<(*aux_type)<<" "<<(*aux_name)<<", std::function<void("<<type_ret<<")> callback){"<<std::endl;
					}else{
						*myfile <<(*aux_type)<<" "<<(*aux_name)<<", ";
					}
					++aux_type;	
				}
				
				*myfile <<"	Data_"<<(((*callers)->getCallee())->getName()).c_str()<<"_"<<name_service<<" data;"<<std::endl;
				aux_name = (name_parameters).begin();
				aux_type = (type_parameters).begin();
				aux_type = (type_parameters).begin();
				for (aux_name; aux_name != (name_parameters).end(); ++aux_name){
					*myfile <<"	data.";
					*myfile <<(*aux_name) <<" = "<<(*aux_name) <<";"<<std::endl;
				}
				*myfile <<"	port_"<<callee<<"_"<<name_service<<"_send->write(this, data);"<<std::endl;
				aux_name = (name_parameters).begin();

				*myfile <<"	callback(port_"<<callee<<"_"<<name_service<<"_receive->read(this));"<<std::endl;
				*myfile <<"}"<<std::endl;
				for (std::list<Attribute*>::iterator atts = ((*modules)->getAttributes())->begin(); atts != ((*modules)->getAttributes())->end(); ++atts){
					if((*atts)->getModifier() == 2){
						*myfile_h <<"		"<<(*atts)->getType()<<" "<<(*atts)->getName()<<";"<<std::endl;
					}
				}
				*myfile_h <<"		void service_sync_"<<callee<<"_"<<name_service<<"(";
				sz = (name_parameters).size();
				aux_type = (type_parameters).begin();
				for (aux_name; aux_name != (name_parameters).end(); ++aux_name){
					sz--;
					if(sz == 0){
						*myfile_h <<(*aux_type)<<" "<<(*aux_name)<<", "<<type_ret<<" *ret);"<<std::endl;
					}else{
						*myfile_h <<(*aux_type)<<" "<<(*aux_name)<<", ";
					}
					++aux_type;	
				}
				*myfile_h <<"		void service_async_"<<callee<<"_"<<name_service<<"(";
				sz = (name_parameters).size();
				aux_type = (type_parameters).begin();
				aux_name = (name_parameters).begin();
				for (aux_name; aux_name != (name_parameters).end(); ++aux_name){
					sz--;
					if(sz == 0){
						*myfile_h <<(*aux_type)<<" "<<(*aux_name)<<", std::function<void("<<type_ret<<")> callback);"<<std::endl;
					}else{
						*myfile_h <<(*aux_type)<<" "<<(*aux_name)<<", ";
					}
					++aux_type;	
				}
				*myfile_h <<"		void service_"<<callee<<"_"<<name_service<<"_"<<"sync"<<"(";
				*myfile <<"void "<<name<<"::service_"<<callee<<"_"<<name_service<<"_"<<"sync"<<"(";
				aux_name = (name_parameters).begin();
				sz = (name_parameters).size();
				aux_type = (type_parameters).begin();
				for (aux_name; aux_name != (name_parameters).end(); ++aux_name){
					sz--;
					if(sz == 0){
						*myfile_h <<(*aux_type)<<" "<<(*aux_name)<<", "<<type_ret<<" *ret);"<<std::endl;
						*myfile <<(*aux_type)<<" "<<(*aux_name)<<", "<<type_ret<<" *ret){"<<std::endl;
					}else{
						*myfile_h <<(*aux_type)<<" "<<(*aux_name)<<", ";
						*myfile <<(*aux_type)<<" "<<(*aux_name)<<", ";
					}
					++aux_type;	
				}
				*myfile <<"	SC_FORK"<<std::endl;
				*myfile <<"		sc_spawn(sc_bind(&"<<name<<"::"<<"service_sync_"<<callee<<"_"<<name_service<<", this";
				aux_name = (name_parameters).begin();
				sz = (name_parameters).size();
				for (aux_name; aux_name != (name_parameters).end(); ++aux_name){
					sz--;
					if(sz == 0){
							*myfile <<", "<<(*aux_name)<<", ret), "<<"\""<<"service_"<<callee<<"_"<<name_service<<"_"<<"sync"<<"\""<<") , "<<std::endl;
					}else{
							*myfile <<", "<<(*aux_name);
					}
					++aux_type;	
				}
				*myfile <<"	SC_JOIN"<<std::endl;
				*myfile <<"}"<<std::endl;

				*myfile_h <<"		void service_"<<callee<<"_"<<name_service<<"_"<<"async"<<"(";
				*myfile <<"void "<<name<<"::service_"<<callee<<"_"<<name_service<<"_"<<"async"<<"(";
				aux_name = (name_parameters).begin();
				sz = (name_parameters).size();
				aux_type = (type_parameters).begin();
				for (aux_name; aux_name != (name_parameters).end(); ++aux_name){
					sz--;
					if(sz == 0){
						*myfile_h <<(*aux_type)<<" "<<(*aux_name)<<",  std::function<void("<<type_ret<<")> callback);"<<std::endl;
						*myfile_h<<std::endl;
						*myfile <<(*aux_type)<<" "<<(*aux_name)<<",  std::function<void("<<type_ret<<")> callback){"<<std::endl;
					}else{
						*myfile_h <<(*aux_type)<<" "<<(*aux_name)<<", ";
						*myfile <<(*aux_type)<<" "<<(*aux_name)<<", ";
					}
					++aux_type;	
				}
				*myfile <<"	sc_spawn(sc_bind(&"<<name<<"::"<<"service_async_"<<callee<<"_"<<name_service<<", this";
				aux_name = (name_parameters).begin();
				sz = (name_parameters).size();
				for (aux_name; aux_name != (name_parameters).end(); ++aux_name){
					sz--;
					if(sz == 0){
							*myfile <<", "<<(*aux_name)<<", callback), "<<"\""<<"service_"<<callee<<"_"<<name_service<<"_"<<"async"<<"\""<<");"<<std::endl;
					}else{
							*myfile <<", "<<(*aux_name);
					}
					++aux_type;	
				}
				*myfile <<"}"<<std::endl;
				name_parameters.clear();
				type_parameters.clear();
			}
		}// Acaba a verificações para callers
		type_parameters.clear();
		name_parameters.clear();
		*myfile_h <<"	private:"<<std::endl;
		for (std::list<Attribute*>::iterator atts = ((*modules)->getAttributes())->begin(); atts != ((*modules)->getAttributes())->end(); ++atts){
			if((*atts)->getModifier() == 0){
				*myfile_h <<"		"<<(*atts)->getType()<<" "<<(*atts)->getName()<<";"<<std::endl;
			}
		}
		write_methods_h(&ListMethodPrivate,myfile_h);
		*myfile_h <<"	protected:"<<std::endl;
		for (std::list<Attribute*>::iterator atts = ((*modules)->getAttributes())->begin(); atts != ((*modules)->getAttributes())->end(); ++atts){
			if((*atts)->getModifier() == 1){
				*myfile_h <<"		"<<(*atts)->getType()<<" "<<(*atts)->getName()<<";"<<std::endl;
			}
		}				
		write_methods_h(&ListMethodProtected,myfile_h);		
		*myfile_h << "};"<<std::endl;
		*myfile_h <<std::endl;
		*myfile_h << "#endif"<<std::endl;
		(*myfile).close();
		(*myfile_h).close();
	}
	*data_h<<"#endif"<<std::endl;
	(*data_h).close();

}
				

//////FIM DO Trecho do SYSTEM C 
void addNewType(std::list<std::string*> *ListNewType, std::string type){
	int exists = 0;
	for (std::list<std::string*>::iterator types = (ListNewType)->begin(); types != (ListNewType)->end(); ++types){
		if((*types)->c_str() == type){
			exists =1;
		}
	}
	if (!exists)
		(*ListNewType).push_back(new std::string(type));
}

Module* findModule (std::list<Module*> *mylist, std::string toFind){
   for (std::list<Module*>::iterator it = mylist->begin(); it != mylist->end(); ++it)
         if((*it)->getName() == toFind){
              return *it;
          }
    return NULL;
}

Service* findService (std::list<Service*> *mylist, std::string toFind){
    for (std::list<Service*>::iterator it=mylist->begin(); it != mylist->end(); ++it){
         if((*it)->getName() == toFind)
              return *it;
      }
    return NULL;
}

Element* findElement (std::list<Element*> *mylist, std::string toFind){
    for (std::list<Element*>::iterator it=mylist->begin(); it != mylist->end(); ++it)
         if((*it)->getName() == toFind){
         	return *it;
         }
    return NULL;
}

bool findDependency (std::list<Dependency*> *mylist, std::string callee , std::string caller, std::string transaction){
    for (std::list<Dependency*>::iterator it=mylist->begin(); it != mylist->end(); ++it){
        if(((*it)->getCaller()->getName() == caller)&& ((*it)->getCallee()->getName() == callee) && ((*it)->getTransaction()->getName() == transaction)){
         	return 1;
         }
     }
    return 0;
}

void setMethod(std::list<Method*> *mylist, std::string name_toFind){
    for (std::list<Method*>::iterator it=mylist->begin(); it != mylist->end(); ++it){
         if((*it)->getName() == name_toFind){
         		(*it)->setService(1);
         	}
         }
}

Method* findMethod(std::list<Method*> *mylist, std::string name_toFind){
    for (std::list<Method*>::iterator it=mylist->begin(); it != mylist->end(); ++it){
         if((*it)->getName() == name_toFind){
         		return (*it);
         	}
         }
    return NULL;
}

Transaction* findTransaction (std::list<Transaction*> *mylist, std::string caller , std::string callee , std::string service){
    for (std::list<Transaction*>::iterator it=mylist->begin(); it != mylist->end(); ++it){
        if(((*it)->getCallee()->getName() == callee) && ((*it)->getService()->getName() == service) && (caller == (*it)->getCaller()->getName())){
         	return *it;
         }
     }
    return NULL;
}

void findFrame (std::list<Frame*> *mylist){
    for (std::list<Frame*>::iterator it=mylist->begin(); it != mylist->end(); ++it){
      		findFrame((*it)->getFrames());
    }
}

////READ SEQUENCE
int function(xercesc::DOMNode* current_node, Frame* frameFather,  std::list<Transaction*>* ListTransaction, std::list<Dependency*>* ListDependency, std::list<Module*>* ListModule,  std::list<Element*>* ListElement){
	std::string thisNodeName = xercesc::XMLString::transcode(current_node->getNodeName());
	xercesc::DOMNamedNodeMap* atts = current_node->getAttributes();
	FrameType frameType;
	std::string string_type;
	for(XMLSize_t i = 0; i < atts->getLength(); i++){
		xercesc::DOMNode* currentNamedNode = atts->item(i);
		std::string strNodeMapName(xercesc::XMLString::transcode(currentNamedNode->getNodeName()));
		std::string strNodeMapValue(xercesc::XMLString::transcode(currentNamedNode->getNodeValue()));

		if(strNodeMapName == "type"){
			string_type= strNodeMapValue;
			frameType = FrameTypeFromString(string_type);
		}
	}
	if(string_type != "seq" && string_type != "par" && string_type!="loop"){
		printf("Erro[13]\nType %s invalid\n",string_type.c_str());
	}
	Frame * frame = new Frame(frameType);
	frameFather->getFrames()->push_back(frame);
	xercesc::DOMNodeList* childList = current_node->getChildNodes();
	for(int i = 0; i < childList->getLength(); i++){
		xercesc::DOMNode* node = childList->item(i);
		thisNodeName = xercesc::XMLString::transcode(node->getNodeName());
		if(thisNodeName == "transaction"){
			xercesc::DOMNamedNodeMap* atts = node->getAttributes();
		  	std::string caller;
		  	std::string callee;
		  	std::string service_callee;
		  	std::string order;
		  	xercesc::DOMNode* node_aux;
		  	//node_aux= node;
			TransactionType type;
			std::string string_type;
			for(XMLSize_t i = 0; i < atts->getLength(); i++){
				  xercesc::DOMNode* currentNamedNode = atts->item(i);
				  std::string strNodeMapName(xercesc::XMLString::transcode(currentNamedNode->getNodeName()));
				  std::string strNodeMapValue(xercesc::XMLString::transcode(currentNamedNode->getNodeValue()));		  
				  if(strNodeMapName == "type"){
				  	string_type = strNodeMapValue;
				  	type = TransactionTypeFromString(string_type);
				  }
			}
			if(string_type!="sync" && string_type!="async"){
				printf("Erro[15]\nType %s invalid\n",string_type.c_str());
				return 0;
			}
			xercesc::DOMNodeList* childListTransaction = node->getChildNodes();
			for(int i = 0; i < childListTransaction->getLength(); i++){
	 			xercesc::DOMNode* node_aux = childListTransaction->item(i);
	 			thisNodeName = xercesc::XMLString::transcode(node_aux->getNodeName());
	 			if(thisNodeName == "callee"){
	 				callee = xercesc::XMLString::transcode(node_aux->getFirstChild()->getNodeValue());
	 			}else if(thisNodeName == "caller"){
				 	caller = xercesc::XMLString::transcode(node_aux->getFirstChild()->getNodeValue());
				}	else if(thisNodeName == "service"){
				 	service_callee = xercesc::XMLString::transcode(node_aux->getFirstChild()->getNodeValue());
				}else if(thisNodeName == "order"){
				 	order = xercesc::XMLString::transcode(node_aux->getFirstChild()->getNodeValue()); 
				}
	 		}
	 		Element * element1 = NULL;
			element1 = findElement(ListElement,callee);
			if(element1==NULL){
				printf("Erro[17]\nElement callee doesnt exist\n");
				return 0;
			}
			Element * element2 = NULL;
			element2 = findElement(ListElement,caller);
			if(element1==NULL){
				printf("Erro[18]\nElement caller doesnt exist\n");
				return 0;
			}
			Transaction * transaction = NULL;
			Service* service_aux= NULL;
			Module * module_caller = findModule(ListModule,element2->getValue());
			Module *module_callee = findModule(ListModule,element1->getValue());
			//Verificando se existe o metodo
			service_aux = findService(module_callee->getServices(), service_callee);
			if(service_aux == NULL){
				printf("Erro[19]\nService doesnt exist\n");
				return 0;
			}
			// if(!findDependency(ListDependency, caller, callee, service_callee)){
			// 	printf("Erro[x]\nDependency doesnt exist");
			// 	return 0;
			// }else{
				transaction  = findTransaction(ListTransaction,(element2->getValue()),(element1->getValue()),service_callee);
				if(transaction == NULL){
				transaction = new Transaction((element2->getValue()),module_caller,module_callee,service_aux,"NULL",SYNC);
				ListTransaction->push_back(transaction);
				transaction->setOrder(order);
				// }
				//only nedded save the first ocorrency that transactions 
				//transaction->setOrder(order);
	     		transaction->setTransactionType(type);
	     		std::string* name_caller = new std::string(caller);
				transaction->getNameCaller()->push_back(name_caller);
				transaction->setNameCallee(callee);
				frame->getTransactions()->push_back(transaction);
			}
		//Fim do IF
		}
		else if(thisNodeName =="frame"){
			function(node, frame, ListTransaction, ListDependency, ListModule, ListElement);
		}
	}

}

int main(){
	try { xercesc::XMLPlatformUtils::Initialize(); }
	catch (const xercesc::XMLException& toCatch) {
		char* message = xercesc::XMLString::transcode(toCatch.getMessage());
		std::cout << "Error during initialization! :\n"
			<< message << "\n";
		xercesc::XMLString::release(&message);
		return 1;
	}
//CLASS
	xercesc::XercesDOMParser* parser = new xercesc::XercesDOMParser();
	ParserErrorHandler parserErrorHandler;
	parser->setErrorHandler(&parserErrorHandler);
	parser->setValidationScheme(xercesc::XercesDOMParser::Val_Auto);
	parser->setDoNamespaces(true);
	parser->setDoSchema(true);
	parser->setValidationConstraintFatal(true);
	parser->loadGrammar(xercesc::XMLString::transcode("configs/classDiagram.xsd"), xercesc::Grammar::SchemaGrammarType);
	parser->parse(xercesc::XMLString::transcode("input/classDiagram.xml"));
	if (parser->getErrorCount() == 0)
        std::cout << "CLASS XML file validated against the schema successfully\n" << std::endl;
    else
        std::cout << "XML file doesn't conform to the schema\n" << std::endl;

	xercesc::DOMElement* docRootNode;
	xercesc::DOMDocument* doc;
	xercesc::DOMNodeIterator* walker;
	doc = parser->getDocument();
	docRootNode = doc->getDocumentElement();

	try { walker = doc->createNodeIterator(docRootNode,xercesc::DOMNodeFilter::SHOW_ELEMENT,NULL,true); } catch(const xercesc::XMLException& e){
		std::cout << "Erro" << std::endl;
		return 1;
	}

	xercesc::DOMNode* current_node = NULL;
	std::string thisNodeName;
	std::string parentNodeName;

	Module* module = NULL;
	Attribute* attribute = NULL;
	Method * method = NULL;
	Parameter * parameter = NULL;
	Service * service = NULL;
	bool parentMethod = false;
	Transaction * transaction = NULL;
	std::list<Module*> ListModule;  
	bool newModule = false;
	std::list<std::string*>ListNewType;
	for(current_node = walker->nextNode(); current_node != 0; current_node = walker->nextNode()){

		thisNodeName = xercesc::XMLString::transcode(current_node->getNodeName());
		parentNodeName = xercesc::XMLString::transcode(current_node->getParentNode()->getNodeName());
		if(parentNodeName=="method"){
			parentMethod = true;
		}
		else if(parentNodeName=="service"){
			parentMethod = false;
		}
		if(thisNodeName == "module"){
			newModule = true;
			module = NULL;
			xercesc::DOMNamedNodeMap* atts = current_node->getAttributes();

			std::string name;
			std::string type;
			ModuleType moduleType;


			for(XMLSize_t i = 0; i < atts->getLength(); i++){
				xercesc::DOMNode* currentNamedNode = atts->item(i);

				std::string strNodeMapName(xercesc::XMLString::transcode(currentNamedNode->getNodeName()));
				std::string strNodeMapValue(xercesc::XMLString::transcode(currentNamedNode->getNodeValue()));

				if(strNodeMapName == "name")
					name = strNodeMapValue;
				else if(strNodeMapName == "type"){
					type= strNodeMapValue;
					moduleType = ModuleTypeFromString(strNodeMapValue);
				}
			}
			if((type != "hw") && (type!= "sw"))	{
				printf("Erro[1]\nType %s invalid\n",type.c_str());
				return 0;
			}
			module = new Module(name, moduleType);
			
			ListModule.push_back(module);		
		} else if(thisNodeName == "attribute"){
			attribute=NULL;

			xercesc::DOMNamedNodeMap* atts = current_node->getAttributes();

			std::string name;
			std::string type;
			std::string string_modifier;
			Modifier modifier;

			for(XMLSize_t i = 0; i < atts->getLength(); i++){
				xercesc::DOMNode* currentNamedNode = atts->item(i);

				std::string strNodeMapName(xercesc::XMLString::transcode(currentNamedNode->getNodeName()));
				std::string strNodeMapValue(xercesc::XMLString::transcode(currentNamedNode->getNodeValue()));

				if(strNodeMapName == "name")
					name = strNodeMapValue;
				else if(strNodeMapName == "type")
					type = strNodeMapValue;
				else if(strNodeMapName == "modifier"){
					string_modifier = strNodeMapValue;
					modifier = ModifierFromString(string_modifier);
				}
			}
			if(string_modifier!="private" && string_modifier!="protected" &&  string_modifier!="public"){
				printf("Erro[2]\nType %s invalid\n",string_modifier.c_str());
				return 0;
			}

			if(type == "string")
				type = "std::string";
			attribute = new Attribute(name,type,modifier);
			
			module->getAttributes()->push_back(attribute);		
		} else if(thisNodeName=="method"){
			method = NULL;
			xercesc::DOMNamedNodeMap* atts = current_node->getAttributes();
		  
			std::string name;
			std::string type;
			std::string string_modifier;
			Modifier modifier;

			for(XMLSize_t i = 0; i < atts->getLength(); i++){
				xercesc::DOMNode* currentNamedNode = atts->item(i);

				std::string strNodeMapName(xercesc::XMLString::transcode(currentNamedNode->getNodeName()));
				std::string strNodeMapValue(xercesc::XMLString::transcode(currentNamedNode->getNodeValue()));

				if(strNodeMapName == "name")
					name = strNodeMapValue;
				else if(strNodeMapName == "type")
					type = strNodeMapValue;
				else if(strNodeMapName == "modifier"){
					string_modifier=strNodeMapValue;
					modifier=ModifierFromString(string_modifier);
				}
			}
			if(string_modifier!="private" && string_modifier!="protected" && string_modifier!="public"){
				printf("Erro[3]\nType %s invalid\n",string_modifier.c_str());
				return 0;
			}
			if(type == "string")
				type = "std::string";

			method = new Method(name,type,modifier);
			method->setService(0);
			module->getMethods()->push_back(method);
		}else if(thisNodeName=="parameter"){
			parameter = NULL;
			xercesc::DOMNamedNodeMap* atts = current_node->getAttributes();
		  
		  	std::string name;
		  	std::string type;

			for(XMLSize_t i = 0; i < atts->getLength(); i++){
				xercesc::DOMNode* currentNamedNode = atts->item(i);

				std::string strNodeMapName(xercesc::XMLString::transcode(currentNamedNode->getNodeName()));
				std::string strNodeMapValue(xercesc::XMLString::transcode(currentNamedNode->getNodeValue()));

				if(strNodeMapName == "name")
					name = strNodeMapValue;
				else if(strNodeMapName == "type")
					type = strNodeMapValue;
			}

			if(type == "string")
				type = "std::string";

			parameter = new Parameter(name,type);
			if(parentMethod){
				method->getParameters()->push_back(parameter);
			}
			else{
				service->getParameters()->push_back(parameter);
			}
		}
		else if(thisNodeName=="service"){
			service = NULL;
			xercesc::DOMNamedNodeMap* atts = current_node->getAttributes();
		  
		  	std::string name;
		  	std::string type;

			for(XMLSize_t i = 0; i < atts->getLength(); i++){
				xercesc::DOMNode* currentNamedNode = atts->item(i);

				std::string strNodeMapName(xercesc::XMLString::transcode(currentNamedNode->getNodeName()));
				std::string strNodeMapValue(xercesc::XMLString::transcode(currentNamedNode->getNodeValue()));

				if(strNodeMapName == "name")
					name = strNodeMapValue;
				else if(strNodeMapName == "type")
					type = strNodeMapValue;
			}

			if(type == "string")
				type = "std::string";

			service = new Service(name,type);
			setMethod(module->getMethods(),name);
			module->getServices()->push_back(service);

		}
		else if(thisNodeName == "transaction"){
			transaction = NULL;
			xercesc::DOMNamedNodeMap* atts = current_node->getAttributes();
		  
		  	std::string caller;
		  	std::string callee;
		  	std::string service_callee;

			for(current_node = walker->nextNode(); current_node != 0; current_node = walker->nextNode()){
				thisNodeName = xercesc::XMLString::transcode(current_node->getNodeName());
				parentNodeName = xercesc::XMLString::transcode(current_node->getParentNode()->getNodeName());
				

				if(thisNodeName == "callee"){
				 	callee = xercesc::XMLString::transcode(current_node->getFirstChild()->getNodeValue());
				}
				else if(thisNodeName == "caller"){
				 	caller = xercesc::XMLString::transcode(current_node->getFirstChild()->getNodeValue());
				}
				else if(thisNodeName == "service"){
				 	service_callee = xercesc::XMLString::transcode(current_node->getFirstChild()->getNodeValue()); 
				}
			}
			if(callee == caller){
				printf("Erro[4]\nCaller and Callee are the same\n");
				return 0;
			}
			Module * module_aux = NULL;
			module_aux = findModule(&ListModule,callee);
			if(module_aux == NULL){
				printf("Erro[5]\nModule callee doesnt exist\n");
				return 0;
			}
			module_aux = findModule(&ListModule,caller);
			if(module_aux==NULL){
				printf("Erro[6]\nModule caller doesnt exist\n");
				return 0;
			}
		}

	}


//OBJECT DIAGRAM

	parser->loadGrammar(xercesc::XMLString::transcode("configs/objectDiagram.xsd"), xercesc::Grammar::SchemaGrammarType);
	parser->parse(xercesc::XMLString::transcode("input/objectDiagram.xml"));

	if (parser->getErrorCount() == 0)
        std::cout << "OBJECT XML file validated against the schema successfully\n" << std::endl;
    else
        std::cout << "XML file doesn't conform to the schema\n" << std::endl;

	doc = parser->getDocument();
	docRootNode = doc->getDocumentElement();

	try { walker = doc->createNodeIterator(docRootNode,xercesc::DOMNodeFilter::SHOW_ELEMENT,NULL,true); } catch(const xercesc::XMLException& e){
		std::cout << "Erro" << std::endl;
		return 1;
	}

	current_node = NULL;
	Element * element = NULL;
	Restriction * restriction = NULL;
	Dependency * dependency = NULL;
	std::list<Dependency*>  ListDependency;
	std::list<Element*> ListElement;  
	for(current_node = walker->nextNode(); current_node != 0; current_node = walker->nextNode()){

		thisNodeName = xercesc::XMLString::transcode(current_node->getNodeName());
		parentNodeName = xercesc::XMLString::transcode(current_node->getParentNode()->getNodeName());
		if(thisNodeName == "element"){
			xercesc::DOMNamedNodeMap* atts = current_node->getAttributes();

			std::string name;
			std::string type;

			for(XMLSize_t i = 0; i < atts->getLength(); i++){
				xercesc::DOMNode* currentNamedNode = atts->item(i);

				std::string strNodeMapName(xercesc::XMLString::transcode(currentNamedNode->getNodeName()));
				std::string strNodeMapValue(xercesc::XMLString::transcode(currentNamedNode->getNodeValue()));

				if(strNodeMapName == "name")
					name = strNodeMapValue;
				else if(strNodeMapName == "type")
					type = strNodeMapValue;
			}
			Module*module_instance = NULL;
			element = new Element(name, type);
			ListElement.push_back(element);
			if(type != "transaction"){
				module_instance = findModule(&ListModule,type);
				if(module_instance == NULL){
					printf("Erro[8]\n Object Type Invalid %s\n",type.c_str());
					return 0;
				}
				else{
					module_instance->getInstances()->push_back(element);
				}
			}

		} else if(thisNodeName == "restriction"){
			restriction=NULL;

			xercesc::DOMNamedNodeMap* atts = current_node->getAttributes();

			std::string name;
			std::string value;

			for(XMLSize_t i = 0; i < atts->getLength(); i++){
				xercesc::DOMNode* currentNamedNode = atts->item(i);

				std::string strNodeMapName(xercesc::XMLString::transcode(currentNamedNode->getNodeName()));
				std::string strNodeMapValue(xercesc::XMLString::transcode(currentNamedNode->getNodeValue()));

				if(strNodeMapName == "name")
					name = strNodeMapValue;
				else if(strNodeMapName == "value")
					value = strNodeMapValue;
			}
		
			restriction = new Restriction(name,value);
			element->getRestrictions()->push_back(restriction);
		}
		else if(thisNodeName == "dependency"){
			xercesc::DOMNamedNodeMap* atts = current_node->getAttributes();
		 	dependency = NULL;
		  	std::string caller;
		  	std::string callee;
		  	std::string transaction;
			xercesc::DOMNode* node_aux;
		  	node_aux = walker->nextNode();
		  	for(int i=0;i<3;i++){
				thisNodeName = xercesc::XMLString::transcode(node_aux->getNodeName());
				parentNodeName = xercesc::XMLString::transcode(node_aux->getParentNode()->getNodeName());
				
				if(thisNodeName == "callee"){
				 	callee = xercesc::XMLString::transcode(node_aux->getFirstChild()->getNodeValue());
				 	node_aux = walker->nextNode();
				}
				else if(thisNodeName == "caller"){
				 	caller = xercesc::XMLString::transcode(node_aux->getFirstChild()->getNodeValue());
				 	node_aux = walker->nextNode();
				}
				else if(thisNodeName == "transaction"){
				 	transaction = xercesc::XMLString::transcode(node_aux->getFirstChild()->getNodeValue()); 
				}
			}
			if(callee == caller){
				printf("Erro[9]\nCaller and Callee are the same\n");
				return 0;
			}
			Element * element1=NULL;
			element1 = findElement(&ListElement,callee);
			if(element1==NULL){
				printf("Erro[10]\nElement callee doesnt exist\n");
				return 0;
			}

			Element * element2=NULL;
			element2 = findElement(&ListElement,caller);
			if(element1==NULL){
				printf("Erro[11]\nElement caller doesnt exist\n");
				return 0;
			}
			dependency = new Dependency(element1,element2,findElement(&ListElement,transaction));
			if(dependency == NULL){
				printf("Erro[12]\nTransaction doesnt exist\n");
				return 0;
			}else{
				ListDependency.push_back(dependency);
			}
		}

	}

//SEQUENCE DIAGRAM
	parser->loadGrammar(xercesc::XMLString::transcode("configs/sequenceDiagram.xsd"), xercesc::Grammar::SchemaGrammarType);
	parser->parse(xercesc::XMLString::transcode("input/sequenceDiagram.xml"));

	if (parser->getErrorCount() == 0)
        std::cout << "SEQUENCE XML file validated against the schema successfully\n" << std::endl;
    else
        std::cout << "XML file doesn't conform to the schema\n" << std::endl;

	doc = parser->getDocument();
	docRootNode = doc->getDocumentElement();

	try { walker = doc->createNodeIterator(docRootNode,xercesc::DOMNodeFilter::SHOW_ELEMENT,NULL,false); } catch(const xercesc::XMLException& e){
		std::cout << "Erro" << std::endl;
		return 1;
	}
	Transaction * aux = NULL;
	current_node = NULL;
	Frame * frame = NULL;
	Frame * frameMaster = new Frame(FrameTypeFromString("0")); //frameType==0 (seq)
	std::list<Frame*> ListFrame;  
	std::list<Transaction*> ListTransaction; 
	for(current_node = walker->nextNode(); current_node != 0; current_node = walker->nextNode()){
		thisNodeName = xercesc::XMLString::transcode(current_node->getNodeName());
		if(thisNodeName == "transactions"){
			xercesc::DOMNodeList* childList = current_node->getChildNodes();
			for(int i = 0; i < childList->getLength(); i++){
			 	xercesc::DOMNode* node = childList->item(i);
			 	thisNodeName = xercesc::XMLString::transcode(node->getNodeName());
				if(thisNodeName == "frame"){
			  		xercesc::DOMNamedNodeMap* atts = node->getAttributes();
					FrameType frameType;
					std::string string_type;
					for(XMLSize_t i = 0; i < atts->getLength(); i++){
						xercesc::DOMNode* node = atts->item(i);
						std::string strNodeMapName(xercesc::XMLString::transcode(node->getNodeName()));
						std::string strNodeMapValue(xercesc::XMLString::transcode(node->getNodeValue()));

						if(strNodeMapName == "type"){
							string_type= strNodeMapValue;
							frameType = FrameTypeFromString(string_type);
						}
					}
					if(string_type != "seq" && string_type != "par" && string_type!="loop"){
						printf("Erro[13]\nType %s invalid\n",string_type.c_str());
						return 0;
					}
					frame = new Frame(frameType);
					frameMaster->getFrames()->push_back(frame);
					ListFrame.push_back(frameMaster);	
			 		xercesc::DOMNodeList* childList = node->getChildNodes();
			 		//Verifica os filhos do frame
			 		//Transacao ou outro Frame
			 		for(int i = 0; i < childList->getLength(); i++){
			 			xercesc::DOMNode* node = childList->item(i);
			 			thisNodeName = xercesc::XMLString::transcode(node->getNodeName());
			 			if(thisNodeName == "transaction"){
			 				xercesc::DOMNamedNodeMap* atts = node->getAttributes();
						  	std::string caller;
						  	std::string callee;
						  	std::string service_callee;
						  	std::string order;
						  	xercesc::DOMNode* node_aux;
						  	//node_aux= node;
							TransactionType type;
							std::string string_type;
							for(XMLSize_t i = 0; i < atts->getLength(); i++){
								  xercesc::DOMNode* currentNamedNode = atts->item(i);
								  std::string strNodeMapName(xercesc::XMLString::transcode(currentNamedNode->getNodeName()));
								  std::string strNodeMapValue(xercesc::XMLString::transcode(currentNamedNode->getNodeValue()));		  
								  if(strNodeMapName == "type"){
								  	string_type = strNodeMapValue;
								  	type = TransactionTypeFromString(string_type);
								  }
							}
							if(string_type!="sync" && string_type!="async"){
								printf("Erro[15]\nType %s invalid\n",string_type.c_str());
								return 0;
							}
							xercesc::DOMNodeList* childListTransaction = node->getChildNodes();
							for(int i = 0; i < childListTransaction->getLength(); i++){
					 			xercesc::DOMNode* node_aux = childListTransaction->item(i);
					 			thisNodeName = xercesc::XMLString::transcode(node_aux->getNodeName());
					 			if(thisNodeName == "callee"){
					 				callee = xercesc::XMLString::transcode(node_aux->getFirstChild()->getNodeValue());	 
					 			}else if(thisNodeName == "caller"){
								 	caller = xercesc::XMLString::transcode(node_aux->getFirstChild()->getNodeValue());
								}	else if(thisNodeName == "service"){
								 	service_callee = xercesc::XMLString::transcode(node_aux->getFirstChild()->getNodeValue());
								}else if(thisNodeName == "order"){
								 	order = xercesc::XMLString::transcode(node_aux->getFirstChild()->getNodeValue()); 
								}
					 		}
					 		Element * element1 = NULL;
							element1 = findElement(&ListElement,callee);
							if(element1==NULL){
								printf("Erro[17]\nElement callee doesnt exist\n");
								return 0;
							}
							Element * element2 = NULL;
							element2 = findElement(&ListElement,caller);
							if(element1==NULL){
								printf("Erro[18]\nElement caller doesnt exist\n");
								return 0;
							}
							Transaction * transaction = NULL;
							Service* service_aux= NULL;
							Module * module_caller = findModule(&ListModule,element2->getValue());
							Module *module_callee = findModule(&ListModule,element1->getValue());
							service_aux = findService(module_callee->getServices(), service_callee);
							if(service_aux == NULL){
								printf("Erro[19]\nService doesnt exist\n");
								return 0;
							}
							if(!findDependency(&ListDependency, caller, callee, service_callee)){
								printf("Erro[x]\nDependency doesnt exist");
								return 0;
							}else{
								transaction  = findTransaction(&ListTransaction,(element2->getValue()),(element1->getValue()),service_callee);
								if(transaction == NULL){
									transaction = new Transaction((element2->getValue()),module_caller,module_callee,service_aux,"NULL",SYNC);
									ListTransaction.push_back(transaction);
									transaction->setOrder(order);
								}
							}
			         		transaction->setTransactionType(type);
			         		std::string* name_caller = new std::string(caller);
							transaction->getNameCaller()->push_back(name_caller);
							transaction->setNameCallee(callee);
							frame->getTransactions()->push_back(transaction);
			 			}else if(thisNodeName =="frame"){
			 				//se for um frame
			 				function(node, frame,&ListTransaction, &ListDependency, &ListModule, &ListElement);
			 			}
			 		}
			 	//Transaction outside frame
		 		}else if(thisNodeName=="transaction"){
		 			xercesc::DOMNamedNodeMap* atts = node->getAttributes();
				  	std::string caller;
				  	std::string callee;
				  	std::string service_callee;
				  	std::string order;
				  	xercesc::DOMNode* node_aux;
				  	//node_aux= node;
					TransactionType type;
					std::string string_type;
					for(XMLSize_t i = 0; i < atts->getLength(); i++){
						  xercesc::DOMNode* currentNamedNode = atts->item(i);
						  std::string strNodeMapName(xercesc::XMLString::transcode(currentNamedNode->getNodeName()));
						  std::string strNodeMapValue(xercesc::XMLString::transcode(currentNamedNode->getNodeValue()));		  
						  if(strNodeMapName == "type"){
						  	string_type = strNodeMapValue;
						  	type = TransactionTypeFromString(string_type);
						  }
					}
					if(string_type!="sync" && string_type!="async"){
						printf("Erro[15]\nType %s invalid\n",string_type.c_str());
						return 0;
					}
					xercesc::DOMNodeList* childListTransaction = node->getChildNodes();
					for(int i = 0; i < childListTransaction->getLength(); i++){
			 			xercesc::DOMNode* node_aux = childListTransaction->item(i);
			 			thisNodeName = xercesc::XMLString::transcode(node_aux->getNodeName());
			 			if(thisNodeName == "callee"){
			 				callee = xercesc::XMLString::transcode(node_aux->getFirstChild()->getNodeValue());
			 			}else if(thisNodeName == "caller"){
						 	caller = xercesc::XMLString::transcode(node_aux->getFirstChild()->getNodeValue());
						}	else if(thisNodeName == "service"){
						 	service_callee = xercesc::XMLString::transcode(node_aux->getFirstChild()->getNodeValue());
						}else if(thisNodeName == "order"){
						 	order = xercesc::XMLString::transcode(node_aux->getFirstChild()->getNodeValue()); 
						}
			 		}
			 		Element * element1 = NULL;
					element1 = findElement(&ListElement,callee);
					if(element1==NULL){
						printf("Erro[17]\nElement callee doesnt exist\n");
						return 0;
					}
					Element * element2 = NULL;
					element2 = findElement(&ListElement,caller);
					if(element1==NULL){
						printf("Erro[18]\nElement caller doesnt exist\n");
						return 0;
					}
					Transaction * transaction = NULL;
					Service* service_aux= NULL;
					Module * module_caller = findModule(&ListModule,element2->getValue());
					Module *module_callee = findModule(&ListModule,element1->getValue());
					service_aux = findService(module_callee->getServices(), service_callee);
					if(service_aux == NULL){
						printf("Erro[19]\nService doesnt exist\n");
						return 0;
					}
					if(!findDependency(&ListDependency, caller, callee, service_callee)){
						printf("Erro[x]\nDependency doesnt exist");
						return 0;
					}else{
						transaction  = findTransaction(&ListTransaction,(element2->getValue()),(element1->getValue()),service_callee);
						if(transaction == NULL){
							transaction = new Transaction((element2->getValue()),module_caller,module_callee,service_aux,"NULL",SYNC);
							ListTransaction.push_back(transaction);
							transaction->setOrder(order);
						}
					}
					//only nedded save the first ocorrency that transactions 
	         		transaction->setTransactionType(type);
	         		std::string* name_caller = new std::string(caller);
					transaction->getNameCaller()->push_back(name_caller);
					transaction->setNameCallee(callee);
					frameMaster->getTransactions()->push_back(transaction);
		 		}
			}
		}
		current_node =0;
	}

// //DEPLOY DIAGRAM
// 	parser->loadGrammar(xercesc::XMLString::transcode("configs/deployDiagram.xsd"), xercesc::Grammar::SchemaGrammarType);
// 	parser->parse(xercesc::XMLString::transcode("deployDiagram.xml"));

// 	if (parser->getErrorCount() == 0)
//         std::cout << "DEPLOY XML file validated against the schema successfully\n" << std::endl;
//     else
//         std::cout << "XML file doesn't conform to the schema\n" << std::endl;
    
// 	doc = parser->getDocument();
// 	docRootNode = doc->getDocumentElement();

// 	try { walker = doc->createNodeIterator(docRootNode,xercesc::DOMNodeFilter::SHOW_ELEMENT,NULL,true); } catch(const xercesc::XMLException& e){
// 		std::cout << "Erro" << std::endl;
// 		return 1;
// 	}

// 	current_node = NULL;
// 	Device *device = NULL;
// 	Component *component =NULL;
// 	ExecutionEnvironment *executionEnvironment= NULL;
// 	std::list<Device*> ListDevice;  
// 	std::string NameExecutionEnviroment;
// 	int qtd=0;
// 	for(current_node = walker->nextNode(); current_node != 0; current_node = walker->nextNode()){

// 		thisNodeName = xercesc::XMLString::transcode(current_node->getNodeName());
// 		parentNodeName = xercesc::XMLString::transcode(current_node->getParentNode()->getNodeName());

// 		if(thisNodeName == "device"){
// 			xercesc::DOMNamedNodeMap* atts = current_node->getAttributes();
// 			std::string name;

// 			for(XMLSize_t i = 0; i < atts->getLength(); i++){
// 				xercesc::DOMNode* currentNamedNode = atts->item(i);

// 				std::string strNodeMapName(xercesc::XMLString::transcode(currentNamedNode->getNodeName()));
// 				std::string strNodeMapValue(xercesc::XMLString::transcode(currentNamedNode->getNodeValue()));

// 				if(strNodeMapName == "name")
// 					name = strNodeMapValue;
// 			}
// 			device = new Device(name);
// 			ListDevice.push_back(device);		
// 		} else if(thisNodeName == "executionEnvironment"){
// 			if(qtd==0)
// 				qtd++;
// 			xercesc::DOMNamedNodeMap* atts = current_node->getAttributes();
		  
// 		  	std::string name;

// 			for(XMLSize_t i = 0; i < atts->getLength(); i++){
// 				xercesc::DOMNode* currentNamedNode = atts->item(i);

// 				std::string strNodeMapName(xercesc::XMLString::transcode(currentNamedNode->getNodeName()));
// 				std::string strNodeMapValue(xercesc::XMLString::transcode(currentNamedNode->getNodeValue()));

// 				if(strNodeMapName == "name")
// 					name = strNodeMapValue;
// 			}
// 			NameExecutionEnviroment = name;
// 		}else if(thisNodeName == "component"){

// 			xercesc::DOMNamedNodeMap* atts = current_node->getAttributes();
		  
// 		  	std::string name;

// 			for(XMLSize_t i = 0; i < atts->getLength(); i++){
// 				xercesc::DOMNode* currentNamedNode = atts->item(i);

// 				std::string strNodeMapName(xercesc::XMLString::transcode(currentNamedNode->getNodeName()));
// 				std::string strNodeMapValue(xercesc::XMLString::transcode(currentNamedNode->getNodeValue()));

// 				if(strNodeMapName == "name")
// 					name = strNodeMapValue;
// 			}
// 			Module * aux=NULL;
// 			aux = findModule(&ListModule,name);
// 			if(aux == NULL){
// 				printf("Erro[21]\n aushudh Component must be a Module\n");
// 			}
// 			component = new Component(name);
// 		}else if(thisNodeName == "artifact"){

// 			xercesc::DOMNamedNodeMap* atts = current_node->getAttributes();
		  
// 		  	Artifact artifact;

// 			for(XMLSize_t i = 0; i < atts->getLength(); i++){
// 				xercesc::DOMNode* currentNamedNode = atts->item(i);

// 				std::string strNodeMapName(xercesc::XMLString::transcode(currentNamedNode->getNodeName()));
// 				std::string strNodeMapValue(xercesc::XMLString::transcode(currentNamedNode->getNodeValue()));

// 				if(strNodeMapName == "name")
// 						artifact = ArtifactFromString(strNodeMapValue);
// 			}
// 			executionEnvironment = new ExecutionEnvironment(NameExecutionEnviroment,artifact);
// 			executionEnvironment->getComponents()->push_back(component);
// 			device->getExecutionsEnvironment()->push_back(executionEnvironment);
// 		  }
// 	}

// 	if(qtd==0){
// 		printf("Erro[22]\nExecutionEnvironment must occur at least once\n");
// 	}
	//SystemC Start
  	writeClass(&ListModule, &ListTransaction, &ListNewType, frameMaster->getFrames());
  	writeMain(&ListModule, &ListTransaction);

}