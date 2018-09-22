#include "Frame.hpp"

Frame::Frame(FrameType frameType, int id){
	setFrameType(frameType);
	this->id = id;
}

FrameType Frame::getFrameType(){ return frameType; }
int Frame::getId(){ return id; }
std::list<Transaction*>* Frame::getTransactions(){ return &transactions; };
std::list<Frame*>* Frame::getFrames(){ return &frames; };
std::list<Entry*>* Frame::getEntries(){ return &entries; };


void Frame::setFrameType(FrameType frameType){ this->frameType = frameType; }