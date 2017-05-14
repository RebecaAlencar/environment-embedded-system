#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <stdio.h>
#include <systemc.h>
#include "Defines.h"
#include "MultiAccessInterface.h"
#include "Data.h"

#ifndef LIMIARIZA_H
#define LIMIARIZA_H

class Limiariza : public sc_module {

	public:

		Limiariza(sc_module_name name);
		void run();
		int limiarizarYCRCB(int* pixelrgb);
		sc_port< read_ma< Data_Limiariza_limiarizarYCRCB > >port_limiarizarYCRCB_receive;
		sc_port< write_ma< int > >port_limiarizarYCRCB_send;
	private:

	protected:

};

#endif
#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <stdio.h>
#include <systemc.h>
#include "Defines.h"
#include "MultiAccessInterface.h"
#include "Data.h"

#ifndef LIMIARIZA_H
#define LIMIARIZA_H

class Limiariza : public sc_module {

	public:

		Limiariza(sc_module_name name);
		void run();
		int limiarizarYCRCB(int* pixelrgb);
		sc_port< read_ma< Data_Limiariza_limiarizarYCRCB > >port_limiarizarYCRCB_receive;
		sc_port< write_ma< int > >port_limiarizarYCRCB_send;
	private:

	protected:

};

#endif
