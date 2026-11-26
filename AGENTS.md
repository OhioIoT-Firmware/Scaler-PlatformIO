# AGENTS.md: OhioIoT Scaler SDK for PlatformIO

Notes for AI coding tools.  Read this before you review or change a sketch that uses this repo.

## What this is

ESP32 firmware SDK that connects a device to the OhioIoT MQTT broker (`mqtt.ohioiot.com`, port 8883, TLS, CA built in).  Connecting is simple: add the library, enter WiFi and OhioIoT credentials, and there is no device registration.  The SDK keeps WiFi (fixed 10 second retry) and MQTT (retry with backoff) connected.

This is the **Scaler** tier.  Tiers stack: Minimalist, then Scaler, then Champion.  Modules in this tier: `controller`, `_certificates`, `device_id`, `wifi_tools`, `mqtt`, `events`, `messages`, `metrics`, `monitor`, `json`.

## Files that matter

- `src/main.cpp`: the sketch.
- `src/credentials.h`: the four WiFi and MQTT values, copied from `src/credentials.template`.  It is git-ignored.
- `lib/controller/`: the controller.  It runs the modules from `setup()` and `loop()`.
- `modules/`: one folder per module.  Leave these as shipped.
- `platformio.ini`: platform, PubSubClient, and optional build flags (`MQTT_HOST`, `MQTT_PORT`, `ALLOW_INSECURE_MQTT`, `MQTT_BUFFER_SIZE`).

## How a sketch uses it

- Includes: `#include "credentials.h"` and `#include "controller.h"`.  `controller.h` brings in `controller`, `mqtt`, and every module a sketch calls in this tier.
- In `setup()`: `controller.setup(WIFI_SSID, WIFI_PASS, MQTT_USER, MQTT_PASS);` then `mqtt.set_subscriptions(list);` and `mqtt.set_callback(handler);`.
- In `loop()`: call `controller.loop();` on every pass.  It keeps WiFi and MQTT up and delivers incoming messages.
- A subscription list ends with `nullptr`: `static const char * list[] = { "~/~/inbox", nullptr };`.
- Handler: `void handler(char * topic, char * payload)`.  `topic` arrives fully expanded.
- Topic shorthand: `~/` becomes `{mqtt_user}/`, and `~/~/` becomes `{mqtt_user}/{device_id}/` (this device).  `~/+/inbox` subscribes to `inbox` on all your devices.
- `mqtt.publish(topic, value)` takes text, int, float, or bool.  `~/~/numb/{graph}/{field}` makes an automatic graph.
- Publish only when `mqtt.is_connected` is true.  A publish while disconnected is dropped.
- The SDK never calls `Serial.begin()`.  The sketch does.

## Built-in message routes

The `messages` router sees each message first.  Anything it doesn't claim falls through to your handler.

- `~/~/command/{name}`, and `~/{namespace}/command/{name}[/{code}]` for each name in `mqtt.set_command_namespaces(list)` (nullptr-terminated).
- Command handler, set with `messages.set_command_handler(handler)`: `bool handler(const char * name, const char * payload, char * error)`.  Return true when the command is yours.  Leave `error` empty for success, or write the reason into it for failure.  The router then publishes `~/~/response/command/[{code}/]success` or `.../fail` with the error text.  Returning false means "not mine": no response, and the message falls through to your handler.
- `~/~/clear_counters` clears the event counters.  `~/~/restart` restarts the device.
- The monitor publishes device identity and a heartbeat to `~/~/status` on its own.

## Requirements

- arduino-esp32 core 2.0.17.  Core 3.x is not supported.
- PlatformIO: the official `espressif32` platform (core 2.x, tested on 6.13.0), not pioarduino.  Arduino IDE: esp32 boards package 2.0.17.
- PubSubClient 2.8 or later.

## Common mistakes to check

- Wrong core: 3.x, or the pioarduino platform.
- `controller.loop()` missing from `loop()`.
- `delay()` or other blocking code in `loop()`, so `controller.loop()` runs rarely and connections drop.  Time work with `millis()`.
- Publishing before `mqtt.is_connected` is true, including from `setup()`.  MQTT connects later, inside `controller.loop()`.
- A subscription list without `nullptr` at the end.
- `+` or `#` in a publish topic.  Wildcards are for subscriptions only.
- Real WiFi or MQTT credentials committed or pasted into a chat.  Use placeholders.
- Hard-coding a device id.  The SDK makes one on first boot (8 lowercase letters and digits) and keeps it in NVS.
- Using features this tier doesn't ship.  Scaler has no OTA, provisioning, settings, or power.  Those are Champion only.

## More

OhioIoT: https://ohioiot.com.  Docs for this tier: https://ohioiot.com/docs/code-samples/scaler
