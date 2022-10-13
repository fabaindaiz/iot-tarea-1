#include <math.h>
#include <sensors.h>
#include <stdlib.h>
#include "esp_system.h"
#include "esp_mac.h"
#include "esp_log.h"

/*

Aqui generamos los 5 tipos de protocolos con sus datos.
Las timestamps en realidad siempre mandamos 0, y por comodidad 
guardamos la timestampo del tiempo de llegada en el servidor de la raspberry.


En general los "mensajes" los creamos copiando a la mala (con memcpy) la memoria de los datos en un char*.
No es muy elegante pero funciona.

Al final lo único que se usa fuera de este archivo es:

message: dado un protocolo, crea un mensaje (con header y datos) codificado en un array de bytes (char*).
messageLength: dado un protocolo, entrega el largo del mensaje correspondiente

*/




float floatrand(float min, float max){
    return min + (float)rand()/(float)(RAND_MAX/(max-min));
}

// Accelerometer_Sensor
float* acc_sensor_x(float n){
    float* arr = malloc(2000* sizeof(float));
    for (int i =0; i <2000; i++){
        arr[i] = (float) 2 * sin(2*M_PI * 0.001 * n);
    }
    return arr;
}

float* acc_sensor_y(float n){
    float* arr = malloc(2000* sizeof(float));
    for (int i =0; i <2000; i++){
        arr[i] = (float) 3 * cos(2*M_PI * 0.001 * n);
    }
    return arr;
}

float* acc_sensor_z(float n){
    float* arr = malloc(2000* sizeof(float));
    for (int i =0; i <2000; i++){
        arr[i] = (float) 10 * sin(2*M_PI * 0.001 * n);
    }
    return arr;
}

//THPC_Sensor
float THPC_sensor_temp(){
    float n = floatrand(5, 30);
    return n;
}

unsigned char THPC_sensor_hum(){
    unsigned char n = (unsigned char) (30 + (rand() %51));
    return n;
}

unsigned int THPC_sensor_pres(){
    unsigned int f =(unsigned int) (1000 + (rand() %201));
    return f;
}

float THPC_sensor_co(){
    float f =(float) floatrand(30, 200);
    return f;
}


// Batt_sensor
unsigned char batt_sensor(){
    unsigned char n = (unsigned char) 1 + (rand() % 100);
    return n;
}


// Acelerometer_kpi
float* acc_kpi(){
    float* arr = malloc(7 * sizeof(float));
    arr[0] = (float) floatrand(0.0059, 0.12);   // amp_x
    arr[1] = (float) floatrand(29, 31);         // frec_x
    arr[2] = (float) floatrand(0.0041, 0.11);   // amp_y
    arr[3] = (float) floatrand(59, 61);         // frec_y
    arr[4] = (float) floatrand(0.008, 0.15);    // amp_z
    arr[5] = (float) floatrand(89, 91);         // frec_z
    arr[6] = (float) sqrt(arr[0]*arr[0] + arr[2]*arr[2] + arr[4]*arr[4]); // RMS

    return arr;
} 