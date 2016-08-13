#include "Frame.hpp"

Frame::Frame(FrameType frameType){
	setFrameType(frameType);
}

FrameType Frame::getFrameType(){ return frameType; }
std::list<Transaction*>* Frame::getTransactions(){ return &transactions; };
std::list<Frame*>* Frame::getFrames(){return &frames;};


void Frame::setFrameType(FrameType frameType){ this->frameType = frameType; }