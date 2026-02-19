# User Manual

## Overview

This device is a DMX controller that allows you to manage up to 20 presets, each with a full DMX universe (512 channels). Configuration and management are performed via a user-friendly web interface. The enclosure is equipped with a display and a power barrel plug for the power adapter, a 3 pin DMX connector, and a 6.3mm jack plug for the foot switch.

---

## Presets

- **Up to 20 presets** can be stored and selected.
- Each preset can be configured for all 512 DMX channels (1 DMX universe).
- Presets are managed and edited through the web interface.

---

## Web Interface

The device hosts a website for configuration and control. Access the website by connecting to the device's network and entering its IP address in your browser.

### Preset Configuration

- Configure all 512 channels for each preset.
- Save, edit, and organize presets easily.

### Configuration Section

The website provides additional configuration options:

#### DMX Transmission

- **Send via ArtNET**: Enable or disable DMX transmission over ArtNET protocol.
- **Send to DMX connector**: Enable or disable DMX output via the physical DMX connector.
- At least one option must be enabled for DMX transmission. If both are disabled, a warning is shown: _"DMX will not be transmitted"_.

#### Foot Switch

- **Polarity**: Choose between Standard and Inverted polarity for the foot switch input.
- **Long Press Time**: Set the duration (in milliseconds) required for a long press action on the foot switch (range: 500–2000 ms).

---

## Enclosure and Connectors

### Front

- **Display**: Shows current status, preset, and other information.

### Back

- **Barrel Jack (Power)**: 5V, 1A input for powering the device.
- **6.3mm Jack Plug**: For connecting an external foot switch.
- **3-pin DMX Plug**: Standard DMX output connector for connecting to DMX lighting equipment.

---

## Quick Start

1. Connect power (5V, 1A) to the barrel jack. A boot sequence will start, and the display will show the status.
   An adapter with a 5V, 1A output is required. Ensure the adapter is compatible with your region's power standards.
2. Connect the DMX output to your lighting equipment using the 3-pin DMX connector.
3. Connect the foot switch to the 6.3mm jack plug.
4. Access the web interface to configure presets and settings.
5. Use the foot switch and web interface to operate the device.

---

---

## Foot Switch Usage

The foot switch provides hands-free control and special functions:

- **During power up:**
  - **Long press:** Initiates OTA (Over-The-Air) firmware update mode.
- **Normal operation:**
  - **Short press:** Selects the next preset.
  - **Long press:** Selects the previous preset.

To prevent accidental activation of the OTA update mode, the device will only enter this mode if the foot switch is held down for a long press during the boot sequence. If the foot switch is pressed after the device has fully booted, it will not trigger OTA mode.

### Blackout Preset

- **At startup:** The blackout preset is selected and the display shows `P---`.
- **When at preset 1:**
  - A long press will show `P0-1` (still transmitting P1).
  - Only when a long press of 5 seconds or more is performed, the blackout preset becomes active (display shows `P---`).
  - Any press (long or short) while in blackout will move to P1.

---

## Display Information

The 4-digit 7-segment display provides feedback and status. Below is a table of each display item and its 4-character representation:

| Function / Status        |      Displayed Text      |
| ------------------------ | :----------------------: |
| Preset number (e.g. 15)  |         `P  15`          |
| Blackout preset          |     'P0-1' or `P---`     |
| Booting                  |          `boot`          |
| Boot ready               |          `rEAd`          |
| OTA update in progress   |          `otAb`          |
| OTA finished (success)   |     `otAF` or `donE`     |
| OTA failed               |          `FAIL`          |
| DMX transmit error       |          `dEr `          |
| Artnet error             |          `AEr `          |
| No WiFi                  |          `noFi`          |
| WiFi connected           |          `ConF`          |
| NVRAM error              |          `nEr `          |
| Webserver error          |          `wEr `          |
| Webserver connected      |          `wCon`          |
| Number of presets loaded | `L  20` (for 20 presets) |
| Preset removed           |          `rEM `          |
| Preset added             |          `Add `          |
| Preset changed           |          `CHAn`          |
| Config changed           |          `CnFG`          |

> Note: Some messages are abbreviated to fit the 4-digit display. Characters are shown as they would appear on a 7-segment display.

---

For further assistance, refer to the website's help section or contact support.
