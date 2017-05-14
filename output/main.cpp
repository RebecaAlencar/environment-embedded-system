#include "ReadPPM.h"
#include "Converte.h"
#include "Limiariza.h"
#include "WritePPM.h"

#include "MultiAccessChannel.h"

#include <iostream>
#include <systemc>

#define CHANNELS_SIZE 20

int sc_main(int argc, char* argv[]) {

	ReadPPM R1("R1");
	Converte C1("C1");
	Limiariza L1("L1");

	MultiAccessChannel<Data_Converte_converterYCRCB > channel_Converte_converterYCRCB_to("c1", CHANNELS_SIZE);
	MultiAccessChannel< int > channel_Converte_converterYCRCB_from("c2", CHANNELS_SIZE);

	R1.port_C1_converterYCRCB_send(channel_Converte_converterYCRCB_to);
	R1.port_C1_converterYCRCB_receive(channel_Converte_converterYCRCB_from);

	C1.port_converterYCRCB_send(channel_Converte_converterYCRCB_from);
	C1.port_converterYCRCB_receive(channel_Converte_converterYCRCB_to);

	MultiAccessChannel<Data_Limiariza_limiarizarYCRCB > channel_Limiariza_limiarizarYCRCB_to("c2", CHANNELS_SIZE);
	MultiAccessChannel< int > channel_Limiariza_limiarizarYCRCB_from("c3", CHANNELS_SIZE);

	C1.port_L1_limiarizarYCRCB_send(channel_Limiariza_limiarizarYCRCB_to);
	C1.port_L1_limiarizarYCRCB_receive(channel_Limiariza_limiarizarYCRCB_from);

	L1.port_limiarizarYCRCB_send(channel_Limiariza_limiarizarYCRCB_from);
	L1.port_limiarizarYCRCB_receive(channel_Limiariza_limiarizarYCRCB_to);

	sc_start();
}
#include "ReadPPM.h"
#include "Converte.h"
#include "Limiariza.h"
#include "WritePPM.h"

#include "MultiAccessChannel.h"

#include <iostream>
#include <systemc>

#define CHANNELS_SIZE 20

int sc_main(int argc, char* argv[]) {

	ReadPPM R1("R1");
	Converte C1("C1");
	Limiariza L1("L1");

	MultiAccessChannel<Data_Converte_converterYCRCB > channel_Converte_converterYCRCB_to("c1", CHANNELS_SIZE);
	MultiAccessChannel< int > channel_Converte_converterYCRCB_from("c2", CHANNELS_SIZE);

	R1.port_C1_converterYCRCB_send(channel_Converte_converterYCRCB_to);
	R1.port_C1_converterYCRCB_receive(channel_Converte_converterYCRCB_from);

	C1.port_converterYCRCB_send(channel_Converte_converterYCRCB_from);
	C1.port_converterYCRCB_receive(channel_Converte_converterYCRCB_to);

	MultiAccessChannel<Data_Limiariza_limiarizarYCRCB > channel_Limiariza_limiarizarYCRCB_to("c2", CHANNELS_SIZE);
	MultiAccessChannel< int > channel_Limiariza_limiarizarYCRCB_from("c3", CHANNELS_SIZE);

	C1.port_L1_limiarizarYCRCB_send(channel_Limiariza_limiarizarYCRCB_to);
	C1.port_L1_limiarizarYCRCB_receive(channel_Limiariza_limiarizarYCRCB_from);

	L1.port_limiarizarYCRCB_send(channel_Limiariza_limiarizarYCRCB_from);
	L1.port_limiarizarYCRCB_receive(channel_Limiariza_limiarizarYCRCB_to);

	sc_start();
}
