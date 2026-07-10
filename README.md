# OhioIoT — Minimalist SDK for PlatformIO

The **Minimalist** tier of the OhioIoT device firmware SDK for ESP32.

This code base is designed to help you connect your device to the OhioIoT cloud-based MQTT broker.  You can override those default with build flags in your platformio.ini file.

*** THIS CODE IS STILL UNDER DEVELOPMENT *** 
If you have any questions, please send a note from the web page: [OhioIoT.com/contact](https://ohioiot.com/contact).

```cpp

// define your variables

void setup() {
    controller.setup(WIFI_SSID, WIFI_PASS, MQTT_USER, MQTT_PASS);
    // add your own code
}

void loop()  {
    controller.loop();
    // add your own code
}

```

## Quick Start

1. git clone this repo
2. rename src/credentials.tempate -> credentials.h
3. Add your WiFi and MQTT credentials to the credentials.h; MQTT credentials found in the Settings tab of the app
4. Burn to your device

## What's Included

- _certificates
- _controller
- device_id
- wifi_tools
- mqtt
*** added with this tier:
- events
- messages
- metrics
- monitor
- json

## The Controller
The controller quarterbacks the remaining modules so that they can remain relatively unaware of each other.
You do not need to edit this module for this to work.  It is plain,
readable source. Open it any time to see how the pieces fit together, or tweak it
to change how the framework behaves. Nothing here is a black box.

Feedback is welcome.

