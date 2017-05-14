#include "WritePPM.h"

WritePPM::WritePPM(sc_module_name name) : sc_module(name){
	SC_HAS_PROCESS(WritePPM);
	SC_THREAD(run);
}

bool WritePPM::writeFile(FILE* file, int blackwhite) {
	//put your code here
	void* obj;
	Data_WritePPM_writeFile data =  port_writeFile_receive->read(NULL, obj);
	bool ret;
	port_writeFile_send->write(obj,ret);
}
void WritePPM::run(){
	while(true){
		//put your code here
	}
}
