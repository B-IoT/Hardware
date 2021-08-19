/* MQTT over Secure Websockets
*/
#include "MQTT_lib_init_struct.h"

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event);

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
  esp_mqtt_client_handle_t client = event->client;
  int msg_id;
  // your_context_t *context = event->context;
  switch (event->event_id) {
    case MQTT_EVENT_CONNECTED:
      Serial.println("MQTT CONNECTED EVENT");
      clientGlobal = client;
      callbackConnected();
      break;
    case MQTT_EVENT_DISCONNECTED:
      break;

    case MQTT_EVENT_SUBSCRIBED:
      Serial.println("MQTT SUBSCRIBED EVENT");
      break;
    case MQTT_EVENT_UNSUBSCRIBED:
      Serial.println("MQTT UNSUBSCRIBED EVENT");
      break;
    case MQTT_EVENT_PUBLISHED:
      Serial.printf("MQTT published\n");
      break;
    case MQTT_EVENT_DATA:
      Serial.println("MQTT DATA EVENT");
      Serial.printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
      Serial.printf("DATA=%.*s\r\n", event->data_len, event->data);
      callbackIncomingMsg(event->topic, event->topic_len, event->data, event->data_len);
      break;
    case MQTT_EVENT_ERROR:
      Serial.println("MQTT ERROR EVENT");
      break;
    default:
      break;
  }
  return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
  /* The argument passed to esp_mqtt_client_register_event can de accessed as handler_args*/
  mqtt_event_handler_cb((esp_mqtt_event_handle_t)event_data);
}

static void mqtt_init_client(const char* clientID, const char* mqttServerUriParam, const char* mqttUserParam, const char* mqttPasswordParam, const char* lastWillBuffer, int lastWillBufferLength) {
  const esp_mqtt_client_config_t mqtt_cfg = init_esp_mqtt_client_config(clientID, mqttServerUriParam, mqttUserParam, mqttPasswordParam, lastWillBuffer, lastWillBufferLength, certificate);

  Serial.println(esp_get_free_heap_size());
  esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
  Serial.printf("MQTT client initialized");
  /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
  esp_mqtt_client_register_event(client, (esp_mqtt_event_id_t)ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
  Serial.printf("MQTT client initialized\n");
  esp_mqtt_client_start(client);
  Serial.printf("MQTT client started\n");
}

static void mqtt_lib_subscribe(const char * topic) {
  if (clientGlobal == NULL) {
    Serial.println("Please init client before subscribing.");
  } else {
    esp_mqtt_client_subscribe(clientGlobal, topic, 1);
  }

}
static void mqtt_lib_publish(const char * topic, const char * data) {
  if (clientGlobal == NULL) {
    Serial.println("Please init client before subscribing.");
  } else {
    esp_mqtt_client_publish(clientGlobal, topic, data, 0, 0, 0);
  }

}
