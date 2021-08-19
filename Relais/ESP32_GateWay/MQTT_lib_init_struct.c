#include "MQTT_lib_init_struct.h"

esp_mqtt_client_config_t init_esp_mqtt_client_config(const char* clientId, const char* serverUri, const char * username, const char * password, const char* lastWillBuffer, int lastWillBufferLength, const char* cert) {
  const esp_mqtt_client_config_t mqtt_cfg = {
    .client_id = clientId,
    .uri = serverUri,
    .buffer_size = MQTT_BUFFER_SIZE_RECEIVE,
    .username = username,
    .password = password,
    .lwt_topic = "will",
    .lwt_msg = lastWillBuffer,
    .lwt_msg_len = lastWillBufferLength,
    .cert_pem = (const char *)cert,
    .reconnect_timeout_ms = 2000,
  };
  return mqtt_cfg;
}
