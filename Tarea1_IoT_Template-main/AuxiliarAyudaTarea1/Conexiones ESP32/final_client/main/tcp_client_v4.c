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

static const char *TAG = "example";
char *payload;

unsigned char protocol = 0;
unsigned char transport = 0;

unsigned char change = 1;
unsigned char status;
unsigned char reset = 0;


void tcp_client(void)
{
    char rx_buffer[128];
    char host_ip[] = HOST_IP_ADDR;
    int addr_family = 0;
    int ip_protocol = 0;


    while (1) {
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

        // TCP Mode
        if (transport == 0) {
            int sTCP = socket(addr_family, SOCK_STREAM, ip_protocol);
            if (sTCP < 0) {
                ESP_LOGE(TAG, "Unable to create TCP socket: errno %d", errno);
                break;
            }
            ESP_LOGI(TAG, "Socket TCP created, connecting to %s:%d", host_ip, PORT);

            int err = connect(sTCP, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
            if (err != 0) {
                ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
                break;
            }
            ESP_LOGI(TAG, "Successfully connected");

            while (1) {
                payload = mensaje(protocol, transport);
                int msglen = messageLength(protocol);
                int err = send(sTCP, payload, msglen, 0);
                if (err < 0) {
                    ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                    break;
                }

                int len = recv(sTCP, rx_buffer, sizeof(rx_buffer) - 1, 0);
                // Error occurred during receiving
                if (len <= 4) {
                    ESP_LOGE(TAG, "recv failed: errno %d", errno);
                    break;
                }
                // Data received

                change = (unsigned char) rx_buffer[1];
                status = (unsigned char) rx_buffer[2];
                protocol = (unsigned char) rx_buffer[3];
                transport = (unsigned char) rx_buffer[4];
                

                if (reset == 1 || (protocol == 0 && transport == 0 && status == 10)) {
                    rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
                    ESP_LOGI(TAG, "Received %d bytes from %s:", len, host_ip);
                    ESP_LOGI(TAG, "%s", rx_buffer);

                    const int deep_sleep_sec = 4;
                    ESP_LOGI(TAG, "Entering deep sleep for %d seconds", deep_sleep_sec);
                    esp_deep_sleep(1000000LL * deep_sleep_sec);
                }
                reset = 1;
            }

            if (sTCP != -1) {
                ESP_LOGE(TAG, "Shutting down socket and restarting...");
                shutdown(sTCP, 0);
                close(sTCP);
            }

        }

        else if (transport == 1) {
            int alt_port = 5011;

            int sUDP = socket(addr_family, SOCK_DGRAM, ip_protocol);
            if (sUDP < 0) {
                ESP_LOGE(TAG, "Unable to create UDP socket: errno %d", errno);
                break;
            }

            // Set timeout
            struct timeval timeout;
            timeout.tv_sec = 10;
            timeout.tv_usec = 0;
            setsockopt (sUDP, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

            ESP_LOGI(TAG, "Socket created, sending to %s:%d", HOST_IP_ADDR, alt_port);

            while (1) {
            
                int msglen = messageLength(protocol);
                payload = mensaje(protocol, transport);

                int err = sendto(sUDP, payload, msglen, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
                if (err < 0) {
                    ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                    break;
                }
                ESP_LOGI(TAG, "Message sent:%s", payload);

                struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
                socklen_t socklen = sizeof(source_addr);
                int len = recvfrom(sUDP, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

                // Error occurred during receiving
                if (len <= 4) {
                    ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
                    break;
                }
                // Data received

                ESP_LOGI(TAG, "Received %d bytes from %s:", len, host_ip);

                unsigned char change = (unsigned char) rx_buffer[1];
                unsigned char status = (unsigned char) rx_buffer[2];
                protocol = (unsigned char) rx_buffer[3];
                transport = (unsigned char) rx_buffer[4];
                
                if (status == 10) {
                    ESP_LOGI(TAG, "Received expected message, reconnecting");

                    if (change == 1) {
                        const int deep_sleep_sec = 1;
                        ESP_LOGI(TAG, "Entering deep sleep for %d seconds", deep_sleep_sec);
                        esp_deep_sleep(1000000LL * deep_sleep_sec);
                    }

                    break;
                }
            }

            if (sUDP != -1) {
                ESP_LOGE(TAG, "Shutting down socket and restarting...");
                shutdown(sUDP, 0);
                close(sUDP);
            }
        }

    }
}
