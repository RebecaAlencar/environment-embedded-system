#include <systemc.h>

#include "MultiAccessInterface.hpp"

#ifndef MULTI_ACCESS_CHANNEL_H
#define MULTI_ACCESS_CHANNEL_H

template<class T>
class MultiAccessChannel : public sc_channel, public write_ma<T>, public read_ma<T> {
	private:
		T *data;
		void* *objects;
		int first, elements, capacity;
		sc_event write_event, read_event;
		sc_event _default_event;

	public:
		const sc_event& default_event() const {
			return _default_event;
		}
	
		MultiAccessChannel(sc_module_name nm) : sc_module(nm){
			capacity = 10;
			elements = first = 0;
		
			data = new T[capacity];
			objects = new void*[capacity];
		}
		
		MultiAccessChannel(sc_module_name nm, int capacity) : sc_module(nm){
			if(capacity <= 0) capacity = 10;
		
	        this->capacity = capacity;
	        elements = first = 0;
	
	        data = new T[capacity];
			objects = new void*[capacity];
		}
		
		void write(void* obj, T v){
			if(elements == capacity)
				wait(read_event);
			
			data[ (first+elements)%capacity ] = v;
			objects[ (first+elements)%capacity ] = obj;
		
			elements++;
		
			write_event.notify();
			_default_event.notify();
		}
		void write(T v){
			write(NULL, v);
		}
		bool nb_write(void* obj, T v){
			if(elements == capacity)
				return false;
			
			data[ (first+elements)%capacity ] = v;
			objects[ (first+elements)%capacity ] = obj;
		
			elements++;
		
			write_event.notify();
			_default_event.notify();
			
			return true;
		}
		T read(void* obj, void*& objret){
			while(elements == 0 || (obj != NULL && obj != objects[first]))
				wait(write_event);
		
			T v = data[first];
			objret = objects[first];
		
			elements--;
			first = (first+1)%capacity;
		
			read_event.notify();
			_default_event.notify();
		
			return v;
		}
		T read(void* obj){
			void* tmp;
			return read(obj, tmp);
		}
		T read(){
			return read(NULL);
		}
		bool nb_read(void* obj, void*& objret, T& value){
			if(elements == 0 || (obj != NULL && obj != objects[first]))
				return false;
			
			value = data[first];
			objret = objects[first];
		
			elements--;
			first = (first+1)%capacity;
		
			read_event.notify();
			_default_event.notify();
	
			return true;
		}
};

#endif
