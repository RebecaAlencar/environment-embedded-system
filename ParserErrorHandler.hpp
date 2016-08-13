#ifndef PARSEERRORHANDLER_HPP
#define PARSEERRORHANDLER_HPP

#include <iostream>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>

class ParserErrorHandler : public xercesc::ErrorHandler {
private:
    void reportParseException(const xercesc::SAXParseException& ex);

public:
    void warning(const xercesc::SAXParseException& ex);
    void error(const xercesc::SAXParseException& ex);
    void fatalError(const xercesc::SAXParseException& ex);
    void resetErrors();
};

#endif