#include "TransactionType.hpp"

TransactionType TransactionTypeFromString(std::string name){
	std::transform(name.begin(), name.end(), name.begin(), ::toupper);

	if(name == "ASYNC") return ASYNC;
	else return SYNC;
}