#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <stdio.h>
#include <systemc.h>
#include "Defines.h"
#include "MultiAccessInterface.h"
#include "Data.h"

#ifndef READPPM_H
#define READPPM_H

class ReadPPM : public sc_module {

	public:

		ReadPPM(sc_module_name name);
		void run();
		void make_pixels();
		sc_port< read_ma< int > >port_C1_converterYCRCB_receive;
		sc_port< write_ma< Data_Converte_converterYCRCB > >port_C1_converterYCRCB_send;
		void service_sync_C1_converterYCRCB(int* pRGB, int *ret);
		void service_async_C1_converterYCRCB(int* pRGB, std::function<void(int)> callback);
		void service_C1_converterYCRCB_sync(int* pRGB, int *ret);
		void service_C1_converterYCRCB_async(int* pRGB,  std::function<void(int)> callback);

	private:
		int columns;
		int rows;
		int rgb_max;

	protected:
		FILE* read_file(std::string name);

};

#endif
#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <stdio.h>
#include <systemc.h>
#include "Defines.h"
#include "MultiAccessInterface.h"
#include "Data.h"

#ifndef READPPM_H
#define READPPM_H

class ReadPPM : public sc_module {

	public:

		ReadPPM(sc_module_name name);
		void run();
		void make_pixels();
		sc_port< read_ma< int > >port_C1_converterYCRCB_receive;
		sc_port< write_ma< Data_Converte_converterYCRCB > >port_C1_converterYCRCB_send;
		void service_sync_C1_converterYCRCB(int* pRGB, int *ret);
		void service_async_C1_converterYCRCB(int* pRGB, std::function<void(int)> callback);
		void service_C1_converterYCRCB_sync(int* pRGB, int *ret);
		void service_C1_converterYCRCB_async(int* pRGB,  std::function<void(int)> callback);

	private:
		int columns;
		int rows;
		int rgb_max;

	protected:
		FILE* read_file(std::string name);

};

#endif
