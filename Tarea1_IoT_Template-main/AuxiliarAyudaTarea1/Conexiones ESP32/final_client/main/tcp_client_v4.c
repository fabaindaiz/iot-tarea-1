/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "sdkconfig.h"
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>            // struct addrinfo
#include <arpa/inet.h>
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_sleep.h"

#include <tcp_client_v4.h>
#include <packeting.c>
#if defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
#include "addr_from_stdin.h"
#endif

#if defined(CONFIG_EXAMPLE_IPV4)
#define HOST_IP_ADDR CONFIG_EXAMPLE_IPV4_ADDR
#elif defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
#define HOST_IP_ADDR ""
#endif

#define PORT CONFIG_EXAMPLE_PORT
#define PACK_LEN 1000

char rx_buffer[128];
char host_ip[] = HOST_IP_ADDR;
int addr_family = 0;
int ip_protocol = 0;


static const char *TAG = "example";
char *payload;

unsigned char protocol = 0;
unsigned char transport = 0;

unsigned char change = 1;
unsigned char status;
unsigned char action = 0;




void tcp_client(void)
{

    //int ip_protocol = 0;


    while (1) {



        if (transport == 0) {
            TCPConnection();
        }
        if (transport == 1) {
            xTaskCreate(udp_client_task, "udp_client", 4096, NULL, 5, NULL);
        }

    }
}

int TCP_send_frag(int sock, char transport, char protocolo)
{
    //Parte el mensaje (payload) en trozos de 1000 btyes y los manda por separado, esperando un OK con cada trozo
    printf("Sending!\n");
    char *payload = mensaje(protocolo, transport);
    int payloadLen = messageLength(protocolo) - 1;
    

    for (int i = 0; i < payloadLen; i += PACK_LEN)
    {

        // Generamos el siguiente trozo
        int size = fmin(PACK_LEN, payloadLen - i);
        char *pack = malloc(size);
        memcpy(pack, &(payload[i]), size);

        //Enviamos el trozo
        int err = send(sock, pack, size, 0);
        free(pack);
        if (err < 0)
        {
            ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
        }

        // wait for confirmation
        int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
        // Error occurred during receiving
        if (len < 0)
        {
            //En caso de error abortamos
            ESP_LOGE(TAG, "recv failed: errno %d", errno);
            break;
        }
        else
        {
            rx_buffer[len] = 0;
            char OK_r = rx_buffer[0];
            if (!OK_r)
            {
                ESP_LOGE(TAG, "Server error in fragmented send.");
                free(payload);
                return -1;
            }
        }
    }
    //el último mensaje es solo un \0 para avisarle al server que terminamos
    int err = send(sock, "\0", 1, 0);

    free(payload);

    return err;
}

void mimir() {
    const int deep_sleep_sec = 4;
    ESP_LOGI(TAG, "Entering deep sleep for %d seconds", deep_sleep_sec);
    esp_deep_sleep(1000000LL * deep_sleep_sec);
}

void parsemsg() {
    unsigned char change = (unsigned char) rx_buffer[1];
    unsigned char status = (unsigned char) rx_buffer[2];
    unsigned char new_protocol = (unsigned char) rx_buffer[3];
    unsigned char new_transport = (unsigned char) rx_buffer[4];

    bool change_protocol = false;
    bool change_transport = false;

    if (protocol != new_protocol) {
        protocol = new_protocol;
        change_protocol = true;
    }

    // Elijo que hacer
    if (transport != new_transport) {
        transport = new_transport;
        change_transport = true;
    }

    if (transport == 0 && !change_transport && !change_protocol) {
        // mimir
        mimir();

        
    }
}


void TCPConnection() {
#if defined(CONFIG_EXAMPLE_IPV4)
        struct sockaddr_in dest_addr;
        inet_pton(AF_INET, host_ip, &dest_addr.sin_addr);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
#elif defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
        struct sockaddr_storage dest_addr = { 0 };
        ESP_ERROR_CHECK(get_addr_from_stdin(PORT, SOCK_STREAM, &ip_protocol, &addr_family, &dest_addr));
#endif

    // Crea el socket
    int sTCP = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (sTCP < 0) {
        ESP_LOGE(TAG, "Unable to create TCP socket: errno %d", errno);
        mimir();
        return;
    }
    ESP_LOGI(TAG, "Socket TCP created, connecting to %s:%d", HOST_IP_ADDR, PORT);

    int err = connect(sTCP, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
        return;
    }
    ESP_LOGI(TAG, "Successfully connected");

    while (1) {
        payload = mensaje(protocol, transport);
        int msglen = messageLength(protocol);

        int err = TCP_send_frag(sTCP, transport, protocol);
        // send(sTCP, payload, msglen, 0);

        if (err < 0) {
            ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
            break;
        }

        int len = recv(sTCP, rx_buffer, sizeof(rx_buffer) - 1, 0);
        // Error occurred during receiving
        if (len < 0) {
            ESP_LOGE(TAG, "recv failed: errno %d", errno);
            break;
        }
        // Data received
        else {
            ESP_LOGI(TAG, "Received %d bytes from %s:", len, host_ip);
            parsemsg();
            return;
        }
        
    }

    if (transport == 0 && sTCP != -1) {
        ESP_LOGE(TAG, "Shutting down socket and restarting...");
        shutdown(sTCP, 0);
        close(sTCP);
    }
}


static void udp_client_task(void *pvParameters) {
#if defined(CONFIG_EXAMPLE_IPV4)
        struct sockaddr_in dest_addr;
        inet_pton(AF_INET, host_ip, &dest_addr.sin_addr);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
#elif defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
        struct sockaddr_storage dest_addr = { 0 };
        ESP_ERROR_CHECK(get_addr_from_stdin(PORT, SOCK_STREAM, &ip_protocol, &addr_family, &dest_addr));
#endif
    int alt_port = 5011;

    int sUDP = socket(addr_family, SOCK_DGRAM, ip_protocol);
    if (sUDP < 0) {
        ESP_LOGE(TAG, "Unable to create UDP socket: errno %d", errno);
        mimir();
        return;
    }

    // Set timeout
    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;
    setsockopt (sUDP, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

    ESP_LOGI(TAG, "Socket created, sending to %s:%d", HOST_IP_ADDR, alt_port);

    while (1) {
    
        // Envio paayload
        int msglen = messageLength(protocol);
        payload = mensaje(protocol, transport);

        int err = sendto(sUDP, payload, msglen, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err < 0) {
            ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Message sent:%s", payload);

        // Recibe respuesta
        struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
        socklen_t socklen = sizeof(source_addr);
        int len = recvfrom(sUDP, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

        // Error occurred during receiving
        if (len <= 4) {
            ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
            break;
        }
        // Data received
        else {
            ESP_LOGI(TAG, "Received %d bytes from %s:", len, host_ip);

            parsemsg();
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            return;
        }
    }

    if (sUDP != -1) {
        ESP_LOGE(TAG, "Shutting down socket and restarting...");
        shutdown(sUDP, 0);
        close(sUDP);
    }

    vTaskDelete(NULL);
}
