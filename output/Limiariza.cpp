#include "Limiariza.h"

Limiariza::Limiariza(sc_module_name name) : sc_module(name){
	SC_HAS_PROCESS(Limiariza);
	SC_THREAD(run);
}

int Limiariza::limiarizarYCRCB(int* pixelrgb) {
	//put your code here
	void* obj;
	Data_Limiariza_limiarizarYCRCB data =  port_limiarizarYCRCB_receive->read(NULL, obj);
	int ret;
	port_limiarizarYCRCB_send->write(obj,ret);
}
void Limiariza::run(){
	while(true){
		//put your code here
	}
}
