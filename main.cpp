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

Element* findElement (std::list<Element*> *mylist, std::string toFind){
    for (std::list<Element*>::iterator it=mylist->begin(); it != mylist->end(); ++it)
         if((*it)->getName() == toFind){
         	return *it;
         }
    return NULL;
}

void setMethod(std::list<Method*> *mylist, std::string name_toFind){
    for (std::list<Method*>::iterator it=mylist->begin(); it != mylist->end(); ++it){
         if((*it)->getName() == name_toFind){
         		(*it)->setService(1);
         	}
         }
}

Module* findModule (std::list<Module*> *mylist, std::string toFind){
   for (std::list<Module*>::iterator it = mylist->begin(); it != mylist->end(); ++it)
         if((*it)->getName() == toFind){
              return *it;
          }
    return NULL;
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

	parser->loadGrammar(xercesc::XMLString::transcode("config/objectDiagram.xsd"), xercesc::Grammar::SchemaGrammarType);
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
return 0;
}