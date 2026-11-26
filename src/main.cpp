#include "Arduino.h"

#include "credentials.h"
#include "controller.h"



// REGULAR SUBSCRIPTIONS AND MESSAGE HANDLING Part 1 ----------------------------------
//
// Topics this device listens to.  Two shorthands save you typing:
//
//     ~/        becomes   your-mqtt-user/
//     ~/~/      becomes   your-mqtt-user/this-device-id/
//
// So "~/~/inbox" below arrives as   your-mqtt-user/a1b2c3d4e/inbox
//
// Rename these, add your own, or leave just the nullptr if you don't need any.

static const char * subscription_list[] = {
	"~/~/inbox",			// anything sent to this one device
	"~/broadcast/config",			// a second topic, to show the list takes several
	nullptr					// keep this
};


// Runs for every message that arrives on a topic above.
// `topic` is the full expanded topic; `payload` is the message body.
void messageHandler(char * topic, char * payload) {
	Serial.println(payload);	// replace this with something more interesting when you are ready
}



// STRUCTURED COMMANDS Part 1 ---------------------------------------------------------
//
// A namespace groups related commands.  Each name below subscribes to
//
//     ~/{namespace}/command/{command-name}
//
// So "pump" catches   your-mqtt-user/pump/command/start
//
// Name them after the things your device controls.

static const char * command_namespaces[] = {
	"pump",
	"lights",
	nullptr					// keep this
};


// runs for every structured command that arrives
// sub_topic is just the command name, e.g. "start"
// return true to claim it, then success goes out, or fail if you wrote text into error
// return false if it isn't yours, then no response goes out and it falls through to messageHandler
bool commandHandler(const char * sub_topic, const char * payload, char * error) {
    Serial.println(payload);    // replace this with something more interesting when you are ready
    return true;
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

	// your own setup code goes here

}




void loop() {

    controller.loop();

    // Keep this loop non-blocking — no delay().  The controller needs to run
    // often to hold the WiFi and MQTT connections up.  For periodic work,
    // compare millis() against a timestamp you saved last time round.

    if (mqtt.is_connected) {

        static unsigned long last_publish_ms = 0;
        static int demo_counter = 0;

        if (millis() - last_publish_ms >= 10000) {
            last_publish_ms = millis();
            demo_counter = demo_counter + 1;
            // replace this with your own sensor reading
            // the graph shows after about 10 seconds
            mqtt.publish("~/~/numb/demo/counter", demo_counter);
        }

    }

}
