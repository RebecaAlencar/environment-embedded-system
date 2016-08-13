#ifndef FRAME_HPP
#define FRAME_HPP

#include <string>
#include <list>
#include "FrameType.hpp"
#include "Transaction.hpp"

class Frame {

private:
	FrameType frameType;
	
	std::list<Transaction*> transactions;
	std::list<Frame*> frames;

public:
	Frame(FrameType);

	FrameType getFrameType();
	std::list<Transaction*>* getTransactions();
	std::list<Frame*>* getFrames();

	void setFrameType(FrameType);
};

#endif