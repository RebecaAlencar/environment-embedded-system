#include "Converte.h"

Converte::Converte(sc_module_name name) : sc_module(name){
	SC_HAS_PROCESS(Converte);
	SC_THREAD(run);
}

int Converte::converterYCRCB(int* pRGB) {
	//put your code here
	void* obj;
	Data_Converte_converterYCRCB data =  port_converterYCRCB_receive->read(NULL, obj);
	int ret;
	port_converterYCRCB_send->write(obj,ret);
}
void Converte::run(){
	while(true){
		//put your code here
	}
}
void Converte::service_sync_L1_limiarizarYCRCB(int* pixelrgb, int *ret){
	Data_Limiariza_limiarizarYCRCB data;
	data.pixelrgb = pixelrgb;
	port_L1_limiarizarYCRCB_send->write(this, data);
	*ret = port_L1_limiarizarYCRCB_receive->read(this);
}
void Converte::service_async_L1_limiarizarYCRCB(int* pixelrgb, std::function<void(int)> callback){
	Data_Limiariza_limiarizarYCRCB data;
	data.pixelrgb = pixelrgb;
	port_L1_limiarizarYCRCB_send->write(this, data);
	callback(port_L1_limiarizarYCRCB_receive->read(this));
}
void Converte::service_L1_limiarizarYCRCB_sync(int* pixelrgb, int *ret){
	SC_FORK
		sc_spawn(sc_bind(&Converte::service_sync_L1_limiarizarYCRCB, this, pixelrgb, ret), "service_L1_limiarizarYCRCB_sync") , 
	SC_JOIN
}
void Converte::service_L1_limiarizarYCRCB_async(int* pixelrgb,  std::function<void(int)> callback){
	sc_spawn(sc_bind(&Converte::service_async_L1_limiarizarYCRCB, this, pixelrgb, callback), "service_L1_limiarizarYCRCB_async");
}
#include "Converte.h"

Converte::Converte(sc_module_name name) : sc_module(name){
	SC_HAS_PROCESS(Converte);
	SC_THREAD(run);
}

int Converte::converterYCRCB(int* pRGB) {
	//put your code here
	void* obj;
	Data_Converte_converterYCRCB data =  port_converterYCRCB_receive->read(NULL, obj);
	int ret;
	port_converterYCRCB_send->write(obj,ret);
}
void Converte::run(){
	while(true){
		//put your code here
	}
}
void Converte::service_sync_L1_limiarizarYCRCB(int* pixelrgb, int *ret){
	Data_Limiariza_limiarizarYCRCB data;
	data.pixelrgb = pixelrgb;
	port_L1_limiarizarYCRCB_send->write(this, data);
	*ret = port_L1_limiarizarYCRCB_receive->read(this);
}
void Converte::service_async_L1_limiarizarYCRCB(int* pixelrgb, std::function<void(int)> callback){
	Data_Limiariza_limiarizarYCRCB data;
	data.pixelrgb = pixelrgb;
	port_L1_limiarizarYCRCB_send->write(this, data);
	callback(port_L1_limiarizarYCRCB_receive->read(this));
}
void Converte::service_L1_limiarizarYCRCB_sync(int* pixelrgb, int *ret){
	SC_FORK
		sc_spawn(sc_bind(&Converte::service_sync_L1_limiarizarYCRCB, this, pixelrgb, ret), "service_L1_limiarizarYCRCB_sync") , 
	SC_JOIN
}
void Converte::service_L1_limiarizarYCRCB_async(int* pixelrgb,  std::function<void(int)> callback){
	sc_spawn(sc_bind(&Converte::service_async_L1_limiarizarYCRCB, this, pixelrgb, callback), "service_L1_limiarizarYCRCB_async");
}
