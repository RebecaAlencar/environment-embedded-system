#include "ParserErrorHandler.hpp"

void ParserErrorHandler::reportParseException(const xercesc::SAXParseException& ex){
	char* msg = xercesc::XMLString::transcode(ex.getMessage());
	std::cerr << "at line " << ex.getColumnNumber() << " column " << ex.getLineNumber() << ", " << msg << std::endl;
	xercesc::XMLString::release(&msg);
}

void ParserErrorHandler::warning(const xercesc::SAXParseException& ex){
	std::cerr << "Warning: ";
	reportParseException(ex);
}

void ParserErrorHandler::error(const xercesc::SAXParseException& ex){
	std::cerr << "Error: ";
	reportParseException(ex);
}

void ParserErrorHandler::fatalError(const xercesc::SAXParseException& ex){
	std::cerr << "Fatal Error: ";
	reportParseException(ex);
}

void ParserErrorHandler::resetErrors(){
}