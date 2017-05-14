#include "ReadPPM.h"

ReadPPM::ReadPPM(sc_module_name name) : sc_module(name){
	SC_HAS_PROCESS(ReadPPM);
	SC_THREAD(run);
}

void ReadPPM::make_pixels() {

//put your code here
}
FILE* ReadPPM::read_file(std::string name) {
	//put your code here
}
void ReadPPM::run(){
	while(true){
		//put your code here
	}
}
void ReadPPM::service_sync_C1_converterYCRCB(int* pRGB, int *ret){
	Data_Converte_converterYCRCB data;
	data.pRGB = pRGB;
	port_C1_converterYCRCB_send->write(this, data);
	*ret = port_C1_converterYCRCB_receive->read(this);
}
void ReadPPM::service_async_C1_converterYCRCB(int* pRGB, std::function<void(int)> callback){
	Data_Converte_converterYCRCB data;
	data.pRGB = pRGB;
	port_C1_converterYCRCB_send->write(this, data);
	callback(port_C1_converterYCRCB_receive->read(this));
}
void ReadPPM::service_C1_converterYCRCB_sync(int* pRGB, int *ret){
	SC_FORK
		sc_spawn(sc_bind(&ReadPPM::service_sync_C1_converterYCRCB, this, pRGB, ret), "service_C1_converterYCRCB_sync") , 
	SC_JOIN
}
void ReadPPM::service_C1_converterYCRCB_async(int* pRGB,  std::function<void(int)> callback){
	sc_spawn(sc_bind(&ReadPPM::service_async_C1_converterYCRCB, this, pRGB, callback), "service_C1_converterYCRCB_async");
}
