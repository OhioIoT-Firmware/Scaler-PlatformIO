

#include "Arduino.h"

#include "credentials.h"
#include "controller.h"
#include "messages.h"
#include "mqtt.h"



// REGULAR SUBSCRIPTIONS AND MESSAGE HANDLING Part 1 ----------------------------------

static const char * subscription_list[] = {
	"~/~/cypress",
	nullptr				// keep this
};


void messageHandler(char * topic, char * payload) {
	Serial.println(payload);	// replace this with something more interesting when you are ready
}



// STRUCTURED COMMANDS Part 1 ---------------------------------------------------------

static const char * command_namespaces[] = {
	"arborvitae",
	"dogwood",
	nullptr			// keep this
};


bool commandHandler(const char * sub_topic, const char * payload, char * error) {
	Serial.println(payload);	// replace this with something more interesting when you are ready
	return true;	// return false if the command couldn't be processed
					// write a string into the error buffer if you want to return a error message
}



void setup() {

	Serial.begin(115200);
	Serial.println("\n\n\n+++++++++++  DEVICE BOOT  ++++++++++++++++++++++++++++++++++\n");

	controller.setup(WIFI_SSID, WIFI_PASS, MQTT_USER, MQTT_PASS);

	// REGULAR SUBSCRIPTIONS AND MESSAGE HANDLING Part 2 -----------------------------------
	mqtt.set_subscriptions(subscription_list);
	mqtt.set_callback(messageHandler);

	// STRUCTURED COMMANDS Part 2 ----------------------------------------------------------
	mqtt.set_command_namespaces(command_namespaces);
	messages.set_command_handler(commandHandler);
}




void loop() {

	controller.loop();

	if (mqtt.is_connected) {

		// do something
		
	}

}

