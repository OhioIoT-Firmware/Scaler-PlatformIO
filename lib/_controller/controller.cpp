

/*
	NOTE: these SDKs deliberately ignore error handling.  someone else's code is hard enough to read, let alone recognizing key logic
	from boilerplate error handling.  Error handling is an important part of firmware, but I left it out here so you can focus on getting 
	your arms around the system logic.  Once you are comfortable, wrap your code in all of the error handling that you feel is appropriate.
*/

#include "Arduino.h"

#include "controller.h"

#include "ca_cert.h"
#include "device_id.h"
#include "events.h"
#include "messages.h"
#include "metrics.h"
#include "monitor.h"
#include "mqtt.h"
#include "wifi_tools.h"



/* 
To override the CA_CERT to point your device to a different MQTT broker, create your own CA cert variable and define CA_CERT to point to that variable name: 
	const char YOUR_CERT[] = "-----BEGIN CERTIFICATE-----\n"
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
		"-----END CERTIFICATE-----\n";

	#define CA_CERT YOUR_CERT
*/
#ifndef CA_CERT
    #define CA_CERT OHIOIOT_CA_CERT
#endif


// Override the following with build flags in platformio if you want something different

#ifndef MQTT_HOST
    #define MQTT_HOST "mqtt.ohioiot.com"
#endif

#ifndef MQTT_PORT
    #define MQTT_PORT 8883
#endif

char wifi_ssid[WIFI_SSID_LEN];	// defined in wifi_tools.h
char wifi_pass[WIFI_PASS_LEN];


Controller::Controller() {}

Controller controller;


static const char * std_subscription_list[] = {
	"~/~/settings/+",		// champion only
	"~/~/command/+",		// scaler and champion
	"~/~/ota/+",			// champion only — start an OTA
	"~/~/ota_abort",		// champion only — cancel an in-flight OTA
	"~/~/ota_accept",		// champion only — accept pending firmware
	"~/~/ota_rollback",		// champion only — revert to previous image
	"~/~/clear_counters",	// scaler and champion
	"~/~/restart",			// scaler and champion
	nullptr
};







void Controller::setup(const char * wifi_ssid, const char * wifi_pass, const char * mqtt_user, const char * mqtt_pass) {

    char deviceID[DEVICE_ID_LEN];
    device_id.get_or_set(deviceID);

	wifi_tools.begin(wifi_ssid, wifi_pass);

	Serial.print("\tusing MQTT host: ");	Serial.println(MQTT_HOST);
	Serial.print("\tusing MQTT port: ");	Serial.println(MQTT_PORT);

	mqtt.setup(MQTT_HOST, MQTT_PORT, mqtt_user, mqtt_pass, deviceID, CA_CERT); 

	monitor.setup(8000);

	// brownout = power problem during last session.  reset_reason was
	// captured in monitor.setup -> metrics.check_all, so we can check it now.
	if (strcmp(metrics.reset_reason, "BROWNOUT") == 0) {
		events.increment("wifi_brown_outs");
	}

	// install the framework's standard subscription list.  user's own
	// subscriptions and any add_command_namespace calls go on top of this.
	mqtt.set_std_subscriptions(std_subscription_list);


	messages.set_clear_counter_handler([](const char* code) {
		events.reset_all();   // wipes everything including starts and brown_outs
		monitor.refresh_counters();
	});

	messages.set_restart_handler([](const char* code) {
		Serial.println("\n\t### RESTARTING ###\n");
		events.flush();		// persist any RAM-only counts before we go down; no-op if nothing pending
		delay(100);
		ESP.restart();
	});
	mqtt.set_std_callback(messages.main_handler);	// router gets first look; user's raw handler runs for anything it doesn't claim

}


static bool namespaces_sent = false;

void Controller::loop() {

	// do something that doesn't require MQTT

	if (wifi_tools.is_connected) {

		mqtt.maintain();

		if (mqtt.is_connected) {

			monitor.send_heartbeat();

			// do something that requires MQTT

			if (!namespaces_sent) {
				mqtt.publish_namespaces();
				namespaces_sent = true;
			}

		}


	} else {

		if (wifi_tools.reconnect()) {
			events.increment_ram("wifi_retries");	// RAM only: fires every RECONNECT_INTERVAL while offline, so no NVS write per attempt.  flushed on the reconnect edge below.
		}
		// @Scaler>
		mqtt.report_disconnect();

	}

	static bool wifi_was_connected = false;
	static bool mqtt_was_connected = false;
	static bool mqtt_has_connected = false;   // first connect after boot isn't a reconnect

	if (wifi_was_connected && !wifi_tools.is_connected) {
		Serial.println("\n\twifi disconnected...\n");
		events.increment_ram("wifi_drops");		// RAM only — a flapping AP can fire this often; persisted on the next clean reconnect
	}

	// clean reconnect — persist any retry/drop counts accumulated in RAM
	// while we were offline.  one NVS write per outage instead of one per
	// 10-second retry attempt.
	if (!wifi_was_connected && wifi_tools.is_connected) {
		events.flush();
	}

	// mqtt_drops      = falling edge of is_connected
	// mqtt_reconnects = rising edge, excluding the very first connect
	if (mqtt_was_connected && !mqtt.is_connected) {
		events.increment("mqtt_drops");
	}
	if (!mqtt_was_connected && mqtt.is_connected) {
		if (mqtt_has_connected) events.increment("mqtt_reconnects");
		mqtt_has_connected = true;
	}
	wifi_was_connected = wifi_tools.is_connected;
	mqtt_was_connected = mqtt.is_connected;
	

}


