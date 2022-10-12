#include <sensors.c>
#include <math.h>
#include <stdlib.h>
#include "esp_system.h"
#include "esp_mac.h"
#include "esp_log.h"

//Genera el header de un mensaje, con la MAC, el protocolo, status, y el largo del mensaje.
char* header(char protocol, char transportLayer){
	char* head = malloc(12);

    char ID = 'D1'
    memcpy((void*) &(head[0]), (void*) MACaddrs, 2);
	uint8_t* MACaddrs = malloc(6);
	esp_efuse_mac_get_default(MACaddrs); //conseguir MAC
	memcpy((void*) &(head[2]), (void*) MACaddrs, 6);
    head[8]= transportLayer;
	head[9]= protocol;
	unsigned short dataLen = dataLength(protocol);
	memcpy((void*) &(head[10]), (void*) &dataLen, 2);
	free(MACaddrs);
	return head;
}

unsigned short lengmsg[6] = {1, 6, 16, 20, 44, 24016}; // cambiar para que cumpla con cada uno de los protocolos
unsigned short dataLength(char protocol){
    return lengmsg[ (unsigned int) protocol]-1;
}

unsigned short messageLength(char protocol){
    return 1+12+dataLength(protocol);
}

char* mensaje (char protocol, char transportLayer){
	char* mnsj = malloc(messageLength(protocol));
	mnsj[messageLength(protocol)-1]= '\0';
	char* hdr = header(protocol, transportLayer);
	char* data;
	switch (protocol) {
		case 0:
			data = dataprotocol00(); 
			break;
		case 1:
			data = dataprotocol0();
			break;
		case 2:
			data = dataprotocol1();
			break;
		case 3:
			data = dataprotocol2();
			break;
        case 4:
			data = dataprotocol3();
			break;
		default:
			data = dataprotocol0();
			break;
	}
	memcpy((void*) mnsj, (void*) hdr, 12);
	memcpy((void*) &(mnsj[12]), (void*) data, dataLength(protocol));
	free(hdr);
	free(data);
	return mnsj;
}

// Aca hay que hacer el caso especifico par cada protocolo
// NOTA: generar el timestamp desde el raspberry

// Arma un paquete para el protocolo de inicio, que busca solo respuesta
char* dataprotocol00(){
    char* msg = malloc(dataLength(0));
    msg[0] = 1; // quizas aca cambiar este por un 0, para que diferenciar el msg de config
    return msg;
}

// Arma un paquete para el protocolo 0, con la bateria
char* dataprotocol0(){
    
    char* msg = malloc(dataLength(1));
    char batt = batt_sensor();
    msg[0] = 1;
	msg[1] = batt;
    // long t = 0;
    // memcpy((void*) &(msg[1]), (void*) &t, 4);
    return msg;
}


char* dataprotocol1(){
    char* msg = malloc(dataLength(2));
	
    char batt = batt_sensor();
	char temp = THPC_sensor_temp();
    msg[0] = 1;
	msg[1] = batt;
	// msg[2-5] es el timestamp
	msg[6] = temp;

    float press = THPC_sensor_pres();
    memcpy((void*) &(msg[7]), (void*) &press, 4); //msg[7-10]

    char hum = THPC_sensor_hum();
    msg[11] = hum;

    float co = THPC_sensor_co();
    memcpy((void*) &(msg[12]), (void*) &co, 4); //msg[12-15]

    return msg;
}

char* dataprotocol2(){
    char* msg = malloc(dataLength(3));
	
    char batt = batt_sensor();
	char temp = THPC_sensor_temp();
    msg[0] = 1;
	msg[1] = batt;
	// msg[2-5] es el timestamp
	msg[6] = temp;

    float press = THPC_sensor_pres();
    memcpy((void*) &(msg[7]), (void*) &press, 4); //msg[7-10]

    char hum = THPC_sensor_hum();
    msg[11] = hum;

    float co = THPC_sensor_co();
    memcpy((void*) &(msg[12]), (void*) &co, 4); //msg[12-15]

	float* acc_kpi = acc_kpi();
	float rms = acc_kpi[6];
	memcpy((void*) &(msg[16]), (void*) &rms, 4); //msg[16-19]
	free(acc_kpi);

    return msg;
}

char* dataprotocol3(){
    char* msg = malloc(dataLength(4));
	
    char batt = batt_sensor();
	char temp = THPC_sensor_temp();
    msg[0] = 1;
	msg[1] = batt;
	// msg[2-5] es el timestamp
	msg[6] = temp;

    float press = THPC_sensor_pres();
    memcpy((void*) &(msg[7]), (void*) &press, 4); //msg[7-10]

    char hum = THPC_sensor_hum();
    msg[11] = hum;

    float co = THPC_sensor_co();
    memcpy((void*) &(msg[12]), (void*) &co, 4); //msg[12-15]

	float* acc_kpi = acc_kpi();

	float rms = acc_kpi[6];
	memcpy((void*) &(msg[16]), (void*) &rms, 4); //msg[16-19]

	float ampx = acc_kpi[0];
	memcpy((void*) &(msg[17]), (void*) &ampx, 4); //msg[20-23]

	float frecx = acc_kpi[1];
	memcpy((void*) &(msg[24]), (void*) &frecx, 4); //msg[24-27]

	float ampy = acc_kpi[2];
	memcpy((void*) &(msg[28]), (void*) &ampy, 4); //msg[28-31]

	float frecy = acc_kpi[3];
	memcpy((void*) &(msg[32]), (void*) &frecy, 4); //msg[32-35]

	float ampz = acc_kpi[4];
	memcpy((void*) &(msg[36]), (void*) &ampz, 4); //msg[36-39]

	float frecz = acc_kpi[5];
	memcpy((void*) &(msg[40]), (void*) &frecz, 4); //msg[40-43]

	free(acc_kpi); // liberamos el malloc

    return msg;
}

char* dataprotocol4(){
    char* msg = malloc(dataLength(5));
	
    char batt = batt_sensor();
	char temp = THPC_sensor_temp();
    msg[0] = 1;
	msg[1] = batt;
	// msg[2-5] es el timestamp
	msg[6] = temp;

    float press = THPC_sensor_pres();
    memcpy((void*) &(msg[7]), (void*) &press, 4); //msg[7-10]

    char hum = THPC_sensor_hum();
    msg[11] = hum;

    float co = THPC_sensor_co();
    memcpy((void*) &(msg[12]), (void*) &co, 4); //msg[12-15]

	float* acc_x = acc_sensor_x(1); // puntero a array de 2000 floats
	memcpy((void*) &(msg[16]), (void*) acc_x, 8000); //msg[16-8015]
	free(acc_x);

	float* acc_y = acc_sensor_y(1); // puntero a array de 2000 floats
	memcpy((void*) &(msg[8016]), (void*) acc_y, 8000); //msg[8016-16015]
	free(acc_y);

	float* acc_z = acc_sensor_z(1); // puntero a array de 2000 floats
	memcpy((void*) &(msg[16016]), (void*) acc_z, 8000); //msg[16016-24015]
	free(acc_z);

    return msg;
}

