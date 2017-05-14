#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <stdio.h>
#include <systemc.h>
#include "Defines.h"
#include "MultiAccessInterface.h"

#ifndef WRITEPPM_H
#define WRITEPPM_H

class WritePPM : public sc_module {

	public:

		WritePPM(sc_module_name name);
		void run();
		bool writeFile(FILE* file,int blackwhite);
		sc_port< read_ma< Data_WritePPM_writeFile > >port_writeFile_receive;
		sc_port< write_ma< bool > >port_writeFile_send;
	private:
		int columns;
		int rows;

	protected:

};

#endif
#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <stdio.h>
#include <systemc.h>
#include "Defines.h"
#include "MultiAccessInterface.h"

#ifndef WRITEPPM_H
#define WRITEPPM_H

class WritePPM : public sc_module {

	public:

		WritePPM(sc_module_name name);
		void run();
		bool writeFile(FILE* file,int blackwhite);
		sc_port< read_ma< Data_WritePPM_writeFile > >port_writeFile_receive;
		sc_port< write_ma< bool > >port_writeFile_send;
	private:
		int columns;
		int rows;

	protected:

};

#endif
