# OhioIoT — Scaler SDK for PlatformIO

The **Scaler** tier of the OhioIoT device firmware SDK for ESP32.

This code base is designed to help you connect your device to the OhioIoT cloud-based MQTT broker.  WiFi connect and reconnect, and MQTT connect and reconnect, are handled for you (WiFi retries every 10 seconds).

This code is subject to ongoing development. If you have any feedback or questions, please send a note from the contact page on the website: [ohioiot.com/contact](https://ohioiot.com/contact).


## Quick Start

1. Git clone this repo.
2. Rename `src/credentials.template` -> `src/credentials.h`.
3. Add your WiFi and MQTT credentials to `src/credentials.h`.
4. Build and upload (PlatformIO: Upload, or `pio run --target upload`). Open the Serial Monitor at 115200 baud and wait for `mqtt connected`.

Your `src/credentials.h` should look like this:

```c
#define WIFI_SSID  "your-wifi-name"
#define WIFI_PASS  "your-wifi-password"
#define MQTT_USER  "your-mqtt-user"      // found in the Settings tab of the OhioIoT app
#define MQTT_PASS  "your-mqtt-password"  // found in the Settings tab of the OhioIoT app
```

The SDK connects to `mqtt.ohioiot.com` on port **8883** with TLS. The CA certificate is built into the SDK, so you don't download a cert or create a `ca_cert.h` file.

You can override those defaults with build flags in your platformio.ini file.

Port **1883** is only for a local, unsecured broker on your own network; it won't work with the OhioIoT broker.

To use a local broker from PlatformIO, uncomment and fill in these flags in `platformio.ini`:

```ini
build_flags =
    -D ALLOW_INSECURE_MQTT
    -D MQTT_HOST=\"192.168.x.x\"
    -D MQTT_PORT=1883
```

## Your first message

Put this in `src/main.cpp`.  The four values live in `src/credentials.h`.

`~/~/x` becomes `{user}/{device_id}/x`.  Publishing to `~/~/numb/<graph>/<field>` makes an automatic graph.

The SDK never calls `Serial.begin()`.  This sketch calls it so the Serial Monitor shows the incoming message.

```cpp
#include "Arduino.h"

#include "credentials.h"
#include "controller.h"


// inbox for this device
static const char * subscription_list[] = {
    "~/~/inbox",
    nullptr    // required end of the list
};


// runs when a message arrives
void on_message(char * topic, char * payload) {
    Serial.println(payload);
}


void setup() {
    Serial.begin(115200);

    controller.setup(WIFI_SSID, WIFI_PASS, MQTT_USER, MQTT_PASS);

    mqtt.set_subscriptions(subscription_list);
    mqtt.set_callback(on_message);
}


void loop() {
    controller.loop();

    if (mqtt.is_connected) {
        // publish every 5 seconds
        static unsigned long timer = 0;

        if (millis() - timer >= 5000) {
            timer = millis();
            float temperature = 21.5;    // replace with a sensor reading
            mqtt.publish("~/~/numb/temperature/temp1", temperature);
        }
    }
}
```

## Device ID

The device id is generated once, on first boot, from the hardware random number generator.  It is saved in NVS (Preferences) and reused after that.  It is 8 lowercase letters and digits.  It must be unique per device on the broker.

## What's Included

| Part | What it does |
| --- | --- |
| `_certificates` | holds the CA certificate for the OhioIoT broker |
| `device_id` | creates the device id once and saves it in NVS |
| `wifi_tools` | connects to WiFi and retries every 10 seconds |
| `mqtt` | connects to the broker, publishes, and subscribes |
| `events` | counts boots, drops, and retries, saved in NVS |
| `messages` | routes incoming topics to handlers |
| `metrics` | reads chip, memory, signal, and uptime |
| `monitor` | publishes identity and a heartbeat |
| `json` | builds small JSON objects in a char buffer |

## Library layout

The project is laid out like this.

```text
src/
    main.cpp
    credentials.template
lib/
    controller/
modules/
    _certificates/
    device_id/
    wifi_tools/
    mqtt/
    events/
    messages/
    metrics/
    monitor/
    json/
```

## The Controller

The controller lives in `lib/controller/`. It quarterbacks the modules in `modules/` so they can remain relatively unaware of each other.

`src/main.cpp` and `lib/controller/` are yours to edit. Leave the folders under `modules/` as they shipped. Other libraries can sit in `lib/` beside the controller.

You do not need to edit the controller for this to work. It is plain, readable source. Open it any time to see how the pieces fit together, or tweak it to change how the framework behaves. Nothing here is a black box.

*Find video tutorials on making IoT systems at the [YouTube channel](https://www.youtube.com/@ohioiot).*

*OhioIoT is an IoT platform designed for [small-scale IoT projects](https://ohioiot.com).*

