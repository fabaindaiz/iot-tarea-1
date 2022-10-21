# Tarea1



## Estructura del Proyecto

Proyecto  
|  
|--- Raspberry  
|--- ESP32-|---main

Se permite cambiar de protocolo en el servidor manteniendo pulsado un numero del 0 al 5 y cambiar de capa de transporte pulsado la 'T' para TCP y la 'U' para UDP.

## Instrucciones de uso

### Server raspberry
    - La raspberry debe tener la zona wifi encendida configurada correctamente
    - Se debe ubicar el terminal de la raspberry en la carpeta "Server Raspberry"
    - Se debe ejecutar el comando "sudo python3 ServerRaspberry.py"
    - Con esto se inicia el servidor en la raspberry

### Cliente ESP
    - Se debe iniciar el terminal del "edf.py" en la carpeta "Conexiones ESP32"
    - Se debe configurar el ESP desde menuconfig con la zona wifi y el puerto 5010
    - Se debe buildear el proyecto y flashearlo a la ESP normalmente
    - Con esto se inicia el cliente en el ESP32