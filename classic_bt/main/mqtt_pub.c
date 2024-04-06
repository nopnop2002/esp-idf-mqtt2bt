/* MQTT (over TCP) Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_mac.h" // esp_base_mac_addr_get
#include "mqtt_client.h"

#include "cmd.h"

static const char *TAG = "PUB";

extern QueueHandle_t xQueuePublish;

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
#else
static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
#endif
{
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
	esp_mqtt_event_handle_t event = event_data;
#endif
	CMD_t cmdBuf;
	cmdBuf.mqtt_event_id = event->event_id;
	BaseType_t err;

	switch (event->event_id) {
		case MQTT_EVENT_CONNECTED:
			ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
			err = xQueueSendFromISR(xQueuePublish, &cmdBuf, NULL);
			if (err != pdTRUE) {
				ESP_LOGE(TAG, "xQueueSendFromISR Fail");
			}
			break;
		case MQTT_EVENT_DISCONNECTED:
			ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
			err = xQueueSendFromISR(xQueuePublish, &cmdBuf, NULL);
			if (err != pdTRUE) {
				ESP_LOGE(TAG, "xQueueSendFromISR Fail");
			}
			break;
		case MQTT_EVENT_SUBSCRIBED:
			ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
			break;
		case MQTT_EVENT_UNSUBSCRIBED:
			ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
			break;
		case MQTT_EVENT_PUBLISHED:
			ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
			break;
		case MQTT_EVENT_DATA:
			ESP_LOGI(TAG, "MQTT_EVENT_DATA");
			break;
		case MQTT_EVENT_ERROR:
			ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
			err = xQueueSendFromISR(xQueuePublish, &cmdBuf, NULL);
			if (err != pdTRUE) {
				ESP_LOGE(TAG, "xQueueSendFromISR Fail");
			}
			break;
		default:
			ESP_LOGI(TAG, "Other event id:%d", event->event_id);
			break;
	}
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
	return ESP_OK;
#endif
}

esp_err_t query_mdns_host(const char * host_name, char *ip);
void convert_mdns_host(char * from, char * to);

void mqtt_pub(void *pvParameters)
{
	ESP_LOGI(TAG, "Start");
	ESP_LOGI(TAG, "CONFIG_MQTT_BROKER=[%s]", CONFIG_MQTT_BROKER);

	// Set client id from mac
	uint8_t mac[8];
	ESP_ERROR_CHECK(esp_base_mac_addr_get(mac));
	for(int i=0;i<8;i++) {
		ESP_LOGI(TAG, "mac[%d]=%x", i, mac[i]);
	}
	char client_id[64];
	sprintf(client_id, "pub-%02x%02x%02x%02x%02x%02x", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	ESP_LOGI(TAG, "client_id=[%s]", client_id);

	// Resolve mDNS host name
	char ip[128];
	ESP_LOGI(TAG, "CONFIG_MQTT_BROKER=[%s]", CONFIG_MQTT_BROKER);
	convert_mdns_host(CONFIG_MQTT_BROKER, ip);
	ESP_LOGI(TAG, "ip=[%s]", ip);
	char uri[138];
	sprintf(uri, "mqtt://%s", ip);
	ESP_LOGI(TAG, "uri=[%s]", uri);

	// Initialize user context
	CMD_t cmdBuf;
	cmdBuf.taskHandle = xTaskGetCurrentTaskHandle();
	ESP_LOGI(TAG, "taskHandle=0x%x", (unsigned int)cmdBuf.taskHandle);
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
	esp_mqtt_client_config_t mqtt_cfg = {
		.broker.address.uri = uri,
		.broker.address.port = 1883,
#if CONFIG_BROKER_AUTHENTICATION
		.credentials.username = CONFIG_AUTHENTICATION_USERNAME,
		.credentials.authentication.password = CONFIG_AUTHENTICATION_PASSWORD,
#endif
		.credentials.client_id = client_id
	};
#else
	esp_mqtt_client_config_t mqtt_cfg = {
		.user_context = &cmdBuf,
		.uri = uri,
		.port = 1883,
		.event_handle = mqtt_event_handler,
#if CONFIG_BROKER_AUTHENTICATION
		.username = CONFIG_AUTHENTICATION_USERNAME,
		.password = CONFIG_AUTHENTICATION_PASSWORD,
#endif
		.client_id = client_id
	};
#endif

	esp_mqtt_client_handle_t mqtt_client = esp_mqtt_client_init(&mqtt_cfg);

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
	esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
#endif

	esp_err_t ret = esp_mqtt_client_start(mqtt_client);
	ESP_LOGI(TAG, "esp_mqtt_client_start ret=%d", ret);

	bool connected = false;
	while (1) {
		xQueueReceive(xQueuePublish, &cmdBuf, portMAX_DELAY);
		ESP_LOGI(TAG, "xQueueReceive. mqtt_event_id=%"PRIi32, cmdBuf.mqtt_event_id);
		if (cmdBuf.mqtt_event_id == MQTT_EVENT_CONNECTED) {
			connected = true;
		} else if (cmdBuf.mqtt_event_id == MQTT_EVENT_DISCONNECTED) {
			connected = false;
		} else if (cmdBuf.mqtt_event_id == SPP_PUBLISHE_EVT) {
			ESP_LOGI(TAG, "connected=%d", connected);
			if (connected) {
				ESP_LOGI(TAG, "cmdBuf.payload=[%.*s]", cmdBuf.length, cmdBuf.payload);
				//int msg_id = esp_mqtt_client_publish(mqtt_client, cmdBuf.topic, cmdBuf.data, 0, 1, 0);
				int msg_id = esp_mqtt_client_publish(mqtt_client, CONFIG_MQTT_PUB_TOPIC, (const char *)cmdBuf.payload, cmdBuf.length, 1, 0);
				ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
			}
		} else if (cmdBuf.mqtt_event_id == MQTT_EVENT_ERROR) {
			break;
		}
	} // end while

	ESP_LOGI(TAG, "Task Delete");
	esp_mqtt_client_stop(mqtt_client);
	vTaskDelete(NULL);

}
