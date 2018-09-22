#ifndef FRAME_HPP
#define FRAME_HPP

#include <string>
#include <list>
#include "FrameType.hpp"
#include "Transaction.hpp"
#include "Entry.hpp"
//#include "Exit.hpp"

class Frame {

private:
	FrameType frameType;
	
	int id;
	std::list<Transaction*> transactions;
	std::list<Frame*> frames;
	std::list<Entry*> entries;

public:
	Frame(FrameType, int);

	FrameType getFrameType();
	int getId();
	std::list<Transaction*>* getTransactions();
	std::list<Frame*>* getFrames();
	std::list<Entry*>* getEntries();
	//std::list<Exit*>* getExits();

	void setFrameType(FrameType);
};

#endif