#include <sensors.h>
#include <sensors.c>
#include <packeting.h>
#include <math.h>
#include <stdlib.h>
#include "esp_system.h"
#include "esp_mac.h"
#include "esp_log.h"


static unsigned char val = (unsigned char) 1;

//Genera el header de un mensaje, con la MAC, el protocolo, status, y el largo del mensaje.
char* header(unsigned int protocol, int transportLayer){
	char* head = malloc(12);
    unsigned short ID = "AB";
    memcpy(&(head[0]), &ID, 2);
	uint8_t* MACaddrs = malloc(6);
	// esp_base_mac_addr_get(MACaddrs);
	esp_efuse_mac_get_default(MACaddrs);
	memcpy((void*) &(head[2]), (void*) MACaddrs, 6);
	head[8]= transportLayer;
	head[9]= protocol;
	unsigned short dataLen = dataLength(protocol);
	memcpy(&(head[10]), &dataLen, 2);
	free(MACaddrs);
	return head;
}

unsigned short lengmsg[6] = {1, 2, 15, 19, 43, 24015};
unsigned short dataLength(unsigned int protocol){
	printf("Datalength: %hu.\n", lengmsg[protocol]);
    return lengmsg[protocol];
}

unsigned short messageLength(unsigned int protocol){
    return 12+dataLength(protocol);
}

char* mensaje (unsigned int protocol, int transportLayer){
	char* mnsj = malloc(messageLength(protocol));
	// mnsj[messageLength(protocol)-1]= '\0';
	char* hdr = header(protocol, transportLayer);
	char* data;
	switch (protocol) {
		case 0:
			data = dataprotocol00();
			break;
		case 1:
			data = dataprotocol1();
			break;
		case 2:
			data = dataprotocol2();
			break;
		case 3:
			data = dataprotocol3();
			break;
        case 4:
			data = dataprotocol4();
			break;
        case 5:
			data = dataprotocol5();
			break;
		default:
			data = dataprotocol00();
			break;
	}
	memcpy((void*) mnsj, (void*) hdr, 12);
	memcpy((void*) &(mnsj[12]), (void*) data, dataLength(protocol));
	free(hdr);
	free(data);
	return mnsj;
}

// Arma un paquete para el protocolo de inicio, que busca solo respuesta
char* dataprotocol00(){
    char* msg = malloc(dataLength(0));
    msg[0] = val;
    return msg;
}

// Arma un paquete para el protocolo 0, con la bateria
char* dataprotocol1(){
    char* msg = malloc(dataLength(1));

	msg[0] = val;

    unsigned char batt = batt_sensor();
	msg[1] = batt;
    return msg;
}

char* dataprotocol2(){
    char* msg = malloc(dataLength(2));
	
	msg[0] = val;

    unsigned char batt = batt_sensor();
	msg[1] = batt;

	float temp = THPC_sensor_temp();
	memcpy((void*) &(msg[2]), (void*) &temp, 4); //msg[2-5]
	
	unsigned int press = THPC_sensor_pres();
	memcpy((void*) &(msg[6]), (void*) &press, 4); //msg[6-9]

	unsigned char hum = THPC_sensor_hum();
	msg[10] = hum;

	float co = THPC_sensor_co();
	memcpy((void*) &(msg[11]), (void*) &co, 4); //msg[11-14]

    return msg;
}

char* dataprotocol3(){
    char* msg = malloc(dataLength(3));

	msg[0] = val;
	
	unsigned char batt = batt_sensor();
	msg[1] = batt;

	float temp = THPC_sensor_temp();
	memcpy((void*) &(msg[2]), (void*) &temp, 4); //msg[2-5]
	
	unsigned int press = THPC_sensor_pres();
	memcpy((void*) &(msg[6]), (void*) &press, 4); //msg[6-9]

	unsigned char hum = THPC_sensor_hum();
	msg[10] = hum;

	float co = THPC_sensor_co();
	memcpy((void*) &(msg[11]), (void*) &co, 4); //msg[11-14]

	float* acc_kpi_ptr = acc_kpi();

	float rms = acc_kpi_ptr[6];
	memcpy((void*) &(msg[15]), (void*) &rms, 4); //msg[15-18]
	free(acc_kpi_ptr);

    return msg;
}

char* dataprotocol4(){
    char* msg = malloc(dataLength(4));
	
	msg[0] = val;

	unsigned char batt = batt_sensor();
	msg[1] = batt;

	float temp = THPC_sensor_temp();
	memcpy((void*) &(msg[2]), (void*) &temp, 4); //msg[2-5]
	
	unsigned int press = THPC_sensor_pres();
	memcpy((void*) &(msg[6]), (void*) &press, 4); //msg[6-9]

	unsigned char hum = THPC_sensor_hum();
	msg[10] = hum;

	float co = THPC_sensor_co();
	memcpy((void*) &(msg[11]), (void*) &co, 4); //msg[11-14]

	float* acc_kpi_ptr = acc_kpi();
	
	float rms = acc_kpi_ptr[6];
	float ampx = acc_kpi_ptr[0];
	float frecx = acc_kpi_ptr[1];
	float ampy = acc_kpi_ptr[2];
	float frecy = acc_kpi_ptr[3];
	float ampz = acc_kpi_ptr[4];
	float frecz = acc_kpi_ptr[5];
	free(acc_kpi_ptr); // liberamos el malloc
	
	
	memcpy((void*) &(msg[15]), (void*) &rms, 4); //msg[15-18]
	memcpy((void*) &(msg[19]), (void*) &ampx, 4); //msg[19-22]

	
	memcpy((void*) &(msg[23]), (void*) &frecx, 4); //msg[23-26]

	
	memcpy((void*) &(msg[27]), (void*) &ampy, 4); //msg[27-30]

	
	memcpy((void*) &(msg[31]), (void*) &frecy, 4); //msg[31-34]

	
	memcpy((void*) &(msg[35]), (void*) &ampz, 4); //msg[35-38]

	
	memcpy((void*) &(msg[39]), (void*) &frecz, 4); //msg[39-42]

	

    return msg;
}

char* dataprotocol5(){
    char* msg = malloc(dataLength(5));
	
	unsigned char batt = batt_sensor();
	float temp = THPC_sensor_temp();
    msg[0] = 1;
	msg[1] = batt;
	memcpy((void*) &(msg[2]), (void*) &temp, 4); //msg[2-5]
	
	unsigned int press = THPC_sensor_pres();
	memcpy((void*) &(msg[6]), (void*) &press, 4); //msg[6-9]

	unsigned char hum = THPC_sensor_hum();
	msg[10] = hum;

	float co = THPC_sensor_co();
	memcpy((void*) &(msg[11]), (void*) &co, 4); //msg[11-14]

	float* acc_x = acc_sensor_x(1); // puntero a array de 2000 floats
	memcpy((void*) &(msg[15]), (void*) acc_x, 8000); //msg[15-8014]
	free(acc_x);

	float* acc_y = acc_sensor_y(1); // puntero a array de 2000 floats
	memcpy((void*) &(msg[8015]), (void*) acc_y, 8000); //msg[8015-16014]
	free(acc_y);

	float* acc_z = acc_sensor_z(1); // puntero a array de 2000 floats
	memcpy((void*) &(msg[16015]), (void*) acc_z, 8000); //msg[16015-24014]
	free(acc_z);

    return msg;
}