#include <systemc.h>

#ifndef MULTI_ACCESS_INTERFACE_H
#define MULTI_ACCESS_INTERFACE_H

template<class T>
class write_ma : virtual public sc_interface {
	public:
		virtual void write(void*, T) = 0;
		virtual void write(T) = 0;
		virtual bool nb_write(void*, T) = 0;
};

template<class T>
class read_ma : virtual public sc_interface {
	public:
		virtual T read(void*, void*&) = 0;
		virtual T read(void*) = 0;
		virtual T read() = 0;
		virtual bool nb_read(void*, void*&, T&) = 0;
};

#endif
