#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
//#include "esp_netif.h"
//#include "protocol_examples_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#define MQTT_BUFFER_SIZE_RECEIVE 14*1024 // in bytes: 6 * 2 * 1024 + 2 * 1024 (whitelist + 2*1024 for the rest to be sure)

#ifdef __cplusplus
extern "C" {
#endif


esp_mqtt_client_config_t init_esp_mqtt_client_config(const char* clientId, const char* serverUri, const char * username, const char * password, const char* lastWillBuffer, int lastWillBufferLength, const char* cert);

#ifdef __cplusplus
} // extern "C"
#endif
