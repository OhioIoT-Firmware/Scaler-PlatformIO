# OhioIoT — Scaler (PlatformIO)

The **Scaler** tier of the OhioIoT device firmware SDK for ESP32.

Everything in Minimalist, plus telemetry, app commands, and message routing.

## Quick start

1. Clone or download this repo:
   ```
   git clone https://github.com/OhioIoT-Firmware/Scaler-PlatformIO.git scaler
   ```
2. Copy `src/credentials.template` to `src/credentials.h` and fill in your WiFi/MQTT values (credentials.h is git-ignored, so your secrets stay out of the repo).
3. Build {{QUICK_START}} upload (PlatformIO Upload button, or `pio run -t upload`).

## What's included

- _certificates
- device_id
- wifi_tools
- mqtt
- events
- messages
- metrics
- monitor
- json

## The controller

You don't need to touch it — but the controller in `lib/_controller/` is plain,
readable source. Open it any time to see how the pieces fit together, or tweak it
to change how the framework behaves. Nothing here is a black box.

## Feedback

This SDK is built to be read. If something's unclear or could be better, open an
issue — feedback is welcome.

## need to add a not that pubsubclient is required
