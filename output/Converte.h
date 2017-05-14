#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <stdio.h>
#include <systemc.h>
#include "Defines.h"
#include "MultiAccessInterface.h"
#include "Data.h"

#ifndef CONVERTE_H
#define CONVERTE_H

class Converte : public sc_module {

	public:

		Converte(sc_module_name name);
		void run();
		int converterYCRCB(int* pRGB);
		sc_port< read_ma< Data_Converte_converterYCRCB > >port_converterYCRCB_receive;
		sc_port< write_ma< int > >port_converterYCRCB_send;
		sc_port< read_ma< int > >port_L1_limiarizarYCRCB_receive;
		sc_port< write_ma< Data_Limiariza_limiarizarYCRCB > >port_L1_limiarizarYCRCB_send;
		void service_sync_L1_limiarizarYCRCB(int* pixelrgb, int *ret);
		void service_async_L1_limiarizarYCRCB(int* pixelrgb, std::function<void(int)> callback);
		void service_L1_limiarizarYCRCB_sync(int* pixelrgb, int *ret);
		void service_L1_limiarizarYCRCB_async(int* pixelrgb,  std::function<void(int)> callback);

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

#ifndef CONVERTE_H
#define CONVERTE_H

class Converte : public sc_module {

	public:

		Converte(sc_module_name name);
		void run();
		int converterYCRCB(int* pRGB);
		sc_port< read_ma< Data_Converte_converterYCRCB > >port_converterYCRCB_receive;
		sc_port< write_ma< int > >port_converterYCRCB_send;
		sc_port< read_ma< int > >port_L1_limiarizarYCRCB_receive;
		sc_port< write_ma< Data_Limiariza_limiarizarYCRCB > >port_L1_limiarizarYCRCB_send;
		void service_sync_L1_limiarizarYCRCB(int* pixelrgb, int *ret);
		void service_async_L1_limiarizarYCRCB(int* pixelrgb, std::function<void(int)> callback);
		void service_L1_limiarizarYCRCB_sync(int* pixelrgb, int *ret);
		void service_L1_limiarizarYCRCB_async(int* pixelrgb,  std::function<void(int)> callback);

	private:

	protected:

};

#endif
