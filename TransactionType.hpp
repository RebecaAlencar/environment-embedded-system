#ifndef TRANSACTIONTYPETYPE_HPP
#define TRANSACTIONTYPETYPE_HPP

#include <string>
#include <algorithm>

enum TransactionType {
	SYNC,
	ASYNC
};

TransactionType TransactionTypeFromString(std::string);

#endif