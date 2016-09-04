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
	parser->loadGrammar(xercesc::XMLString::transcode("classDiagram.xsd"), xercesc::Grammar::SchemaGrammarType);
	parser->parse(xercesc::XMLString::transcode("classDiagram.xml"));
	if (parser->getErrorCount() == 0)
		std::cout << "CLASS XML file validated against the schema successfully\n" << std::endl;
	else
		std::cout << "XML file doesn't conform to the schema\n" << std::endl;

return 0;
}