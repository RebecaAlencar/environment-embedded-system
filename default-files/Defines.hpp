#include <cstdio>
#include <cstdarg>
#include <functional>
#include <list>
#include <systemc.h>
#include <sysc/communication/sc_mutex.h>
#include <string>
#include <map>
#include <stack>
#include <sstream>

#ifndef DEFINES_H
#define DEFINES_H

#define PARALLEL_START  SC_FORK \
                        sc_spawn(sc_bind( (std::function<void()>) ([&](){}
#define PARALLEL_BLOCK  ))), \
                        sc_spawn(sc_bind( (std::function<void()>) ([&]()
#define PARALLEL_END    ))) \
                        SC_JOIN
 
#define CALLBACK(p) [&](p)

#define LOOP	while(1)

class EXEC {
	private:

	enum FlowState {
		WAITING,
		READY,
		RUNNING
	};
	
    bool flowIsSeq[6];
    int flowChildReady[6];
    FlowState flowState[6];
	bool flowStructure[6][6];
	int frames;
		
	void reset_children(int parent){
		int firstvalid = -1;
		for (int i = 0; i < frames; ++i)
			if (flowStructure[parent][i]){
				reset_children(i);
				
				if (firstvalid == -1) firstvalid = i;
			}
		
		flowState[parent] = READY;
		flowChildReady[parent] = firstvalid;
	}
		
	bool reset(int id){
		// If it is the master, return true (all done)
		if (id == 0){
			return true;	
		}
		
		int parent;
		for (int i = 0; i < frames; ++i) if(flowStructure[i][id]) parent = i;
		
		//std::cout << "  evaluate " << parent << std::endl;
		
		// If is sequential, check to see if needs to move the pointer
		if (flowIsSeq[parent]){
			int nextvalid = -1;
			for (int i = 0; i < frames; ++i)
				if (flowStructure[parent][i]){
					nextvalid = i;
					
					if (i > id)
						break;
				}
			
			if (nextvalid > id){
				// Has a next flow to execute the sequence
				flowChildReady[parent] = nextvalid;
				return false;
			} else {
				// Does not have a valid next flow
				
				// mark parent as WAITING because it is done and reset it
				flowState[parent] = WAITING;
				
				// If all done on parent, restart it
				if(reset(parent)){
					/*int firstvalid = -1;
					for (int i = 0; i < frames; ++i)
						if (flowStructure[parent][i]) if (firstvalid == -1) firstvalid = i;

					flowState[parent] = READY;
					flowChildReady[parent] = firstvalid;*/
					
					reset_children(parent);
					return true;
				}
			}
		} else {
			// check all children for WAITING, that means it is done, mark this WAITING then reset parent
			for (int i = 0; i < frames; ++i) 
				if(flowStructure[parent][i] && flowState[i] != WAITING) return false;
			
			flowState[parent] = WAITING;
			
			// If all done on parent, restart it
			if(reset(parent)){
		/*		int firstvalid = -1;
				for (int i = 0; i < frames; ++i)
					if (flowStructure[parent][i]) if (firstvalid == -1) firstvalid = i;

				flowState[parent] = READY;
				flowChildReady[parent] = firstvalid;*/
				
				reset_children(parent);	
				return true;
			}
		}
		
		return false;
	}
	
	bool parents_ready(int id){	
		// Master always ready
		if(id == 0) return true;
		
		int parent;
		for (int i = 0; i < frames; ++i) if(flowStructure[i][id]) parent = i;
				
		// If not ready, just return false
		if (flowState[parent] != READY) return false;
		
		// If is parallel, all children are ready, then check the parent
		if (!flowIsSeq[parent]) return parents_ready(parent);
		
		// If it is its turn
		if (flowChildReady[parent] == id) return true;
		
		return false;
	}
	
	public:
	
	void init(bool *flowStructure, int frames, bool *flowIsSeq){
		std::memcpy(this->flowStructure, flowStructure, frames*frames*sizeof(bool));
		this->frames = frames;
		
		std::memcpy(this->flowIsSeq, flowIsSeq, frames*sizeof(bool));
		
		for (int i = 0; i < frames; ++i){
			int nextvalid;
			for (nextvalid = 0; nextvalid < frames; ++nextvalid) if (this->flowStructure[i][nextvalid]) break;
			flowChildReady[i] = nextvalid;
			flowState[i] = READY;
		}
	}
	
	bool ENTRY(int id){
		//std::cout << "entry " << id  << " state " << flowState[id]<< std::endl;
		
		FlowState state = flowState[id];
		switch(state){
			case READY:
				if(parents_ready(id)){
					flowState[id] = RUNNING;
					return true;
				} else
					return false;
			default:
				return false;
		}
	}
	
	bool EXIT(int id){
		//std::cout << "exit " << id  << " state " << flowState[id]<< std::endl;
		static int count;
		if(count == 10) while(true);
		count++;
		flowState[id] = WAITING;
		//return reset(id);
		reset(id);
		return true;
	}
};
        
extern EXEC _FLOWEXEC;

#endif