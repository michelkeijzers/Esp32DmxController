# ESP32 DMX Controller with OTA Update

This ESP32-based DMX controller features foot switch input handling and over-the-air (OTA) firmware updates.

## Features

- **Foot Switch Control**: Debounced foot switch input with short/long press detection
- **DMX Preset Management**: Cycle through DMX presets with foot switch presses
- **OTA Updates**: Wireless firmware updates via HTTPS
- **ESP32-C3 Compatible**: Optimized for ESP32-C3 Mini Devkit

## Hardware Setup

- **Foot Switch**: Connected to GPIO_NUM_4 (active-low with pull-up)
- **ESP32-C3 Mini Devkit**: Target platform

## Usage

### Normal Operation

- **Short Press**: Cycle to next DMX preset (1→2→3→1...)
- **Long Press**: Cycle to previous DMX preset (3→2→1→3...)

### OTA Update Trigger

- **Triple Press**: Press the foot switch 3 times within 2 seconds to initiate OTA update
- The device will download firmware from the configured URL and restart automatically

## Configuration

### OTA Settings

Edit the following constants in `main.cpp`:

```cpp
#define OTA_URL "https://example.com/firmware.bin"  // Replace with your firmware URL
#define OTA_CERT_PEM NULL  // Set to certificate PEM for HTTPS with cert validation
```

### Foot Switch Pin

```cpp
#define FOOT_SWITCH_PIN GPIO_NUM_4 // Change this to your actual pin
```

## Building and Flashing

1. Configure ESP-IDF environment
2. Set target: `idf.py set-target esp32c3`
3. Build: `idf.py build`
4. Flash: `idf.py flash`
5. Monitor: `idf.py monitor`

## OTA Update Process

1. Host your firmware binary (.bin file) on a web server
2. Update the `OTA_URL` constant with the correct URL
3. Build and flash the initial firmware
4. Connect device to WiFi network
5. Triple-press the foot switch to trigger OTA update
6. Device will download and install new firmware automatically

## Network Requirements

For OTA to work, the ESP32 must be connected to a WiFi network. You may need to add WiFi configuration code or use ESP-IDF's WiFi provisioning features.

## Dependencies

- ESP-IDF v5.4.1
- Components: esp_https_ota, app_update, nvs_flash, esp_wifi, esp_event, driver

## File Structure

````
main/
├── main.cpp              # Main application with OTA functionality
├── foot_switch.hpp/cpp   # Foot switch input handling
├── dmx_preset_changer.hpp/cpp  # DMX preset management
└── CMakeLists.txt        # Build configuration
```</content>
<parameter name="filePath">README.md
````
