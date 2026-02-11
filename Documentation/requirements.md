**_Requirements_**

# Architecture Requirements

## REQ.ARC1xx – Hardware Platform

### REQ.ARC.100 – ESP Platform Selection

**Description:** The system shall be implemented on an ESP microcontroller (ESP32 or later).  
**Rationale:** ESP devices provide integrated Wi-Fi/Bluetooth, sufficient processing power, and are widely supported for embedded IoT applications.

### REQ.ARC.110 – Interrupt-Driven Hardware Events

**Description:** Hardware events (e.g., GPIO changes, timers, communication interfaces) shall be handled using interrupts where applicable.  
**Rationale:** Interrupts ensure responsiveness and efficient CPU usage compared to polling.

### REQ.ARC.120 – Green LED System Running Indicator

**Description:** A green LED shall be illuminated when the ESP32 is running normally.  
**Rationale:** A visual indicator provides immediate feedback to the user that the system is operational and powered.

---

## REQ.ARC.2xx – Development Framework

### REQ.ARC.200 – ESP-IDF Framework Usage

**Description:** The system shall be developed using the ESP-IDF framework provided by Espressif.  
**Rationale:** ESP-IDF is the official SDK, ensuring compatibility, long-term support, and access to low-level hardware features.

### REQ.ARC.210 – C++ Language Requirement

**Description:** All application code shall be written in C++.
**Rationale:** C++ provides object-oriented design, strong type safety, and better maintainability compared to plain C.

---

## REQ.ARC.3xx – Software Architecture

### REQ.ARC.300 – Object-Oriented Design

**Description:** All system components shall be implemented as C++ classes.  
**Rationale:** Encapsulation and modularity improve maintainability, scalability, and testability.

### REQ.ARC.310 – Layered Architecture

**Description:** The software shall be structured into layers: Hardware Abstraction, Drivers, Middleware, and Application.  
**Rationale:** Separation of concerns reduces coupling and improves clarity.

### REQ.ARC.320 – Event-Driven Model

**Description:** The system shall use an event-driven architecture, leveraging interrupts and FreeRTOS tasks for concurrency.  
**Rationale:** Event-driven design ensures responsiveness and efficient resource usage.

---

## REQ.ARC.4xx – Coding Standards

### REQ.ARC.400 – Consistent Naming Conventions

**Description:** All classes, methods, and variables shall follow a consistent naming convention (e.g., CamelCase for classes, snake_case for variables).  
**Rationale:** Consistency improves readability and reduces errors.

### REQ.ARC.410 – Documentation Requirement

**Description:** Each class and method shall include inline documentation using Doxygen-style comments.  
**Rationale:** Documentation ensures clarity for future developers and maintainers.

---

## REQ.ARC.5xx – Reliability & Maintainability

### REQ.ARC.500 – Error Handling Strategy

**Description:** All modules shall implement structured error handling, returning status codes or exceptions where applicable.  
**Rationale:** Robust error handling improves system reliability and debugging.

### REQ.ARC.510 – Modular Expansion Capability

**Description:** The architecture shall allow new modules to be added without modifying existing ones.  
**Rationale:** Modularity supports scalability and future feature expansion.

---

## REQ.ARC.6xx – Performance

### REQ.ARC.600 – Real-Time Responsiveness

**Description:** The system shall meet real-time constraints by prioritizing interrupt-driven tasks and using FreeRTOS scheduling.  
**Rationale:** Ensures timely responses to hardware and user events.

### REQ.ARC.610 – Resource Efficiency

**Description:** The system shall minimize CPU and memory usage by leveraging lightweight data structures and avoiding unnecessary dynamic allocation.  
**Rationale:** Embedded systems have limited resources; efficiency is critical.

# Foot Switch Requirements

## REQ.FOO.1xx – FootSwitch Hardware & Input

### REQ.FOO.100 – Foot Switch Integration

**Description:** The system shall integrate a physical foot switch as an input device.  
**Rationale:** The foot switch provides a simple and reliable user interface for triggering system actions.

### REQ.FOO.110 – Debouncing via ESP-IDF

**Description:** The foot switch input shall be debounced using ESP-IDF mechanisms.  
**Rationale:** Debouncing ensures accurate detection of switch presses and avoids false triggers caused by mechanical bounce.

---

## REQ.FOO.2xx – Foot Switch Events

### REQ.FOO.200 – Event Detection Timing

**Description:** The system shall detect switch events after one second of press duration, distinguishing between long press, short press, and depress.  
**Rationale:** A one-second threshold ensures consistent event recognition and avoids accidental triggers.

### REQ.FOO.210 – Configurable Timing Constants

**Description:** The timing thresholds for long press, short press, and depress shall be configurable constants.  
**Rationale:** Configurable values allow flexibility and adaptation to different use cases or user preferences.

### REQ.FOO.220 – OTA Trigger via Long Press at Startup

**Description:** A long press of the foot switch at system startup shall trigger an OTA update process.  
**Rationale:** This provides a simple and secure mechanism for initiating firmware updates without additional hardware interfaces.

---

## REQ.FOO.3xx – Software Architecture

### REQ.FOO.300 – Dedicated Foot switch Class

**Description:** The foot switch shall be implemented as a dedicated C++ class within ESP-IDF.  
**Rationale:** Encapsulation in a class improves modularity, maintainability, and testability.

### REQ.FOO.310 – Dedicated Foot Switch Task

**Description:** The foot switch class shall run in its own FreeRTOS task.  
**Rationale:** A dedicated task ensures responsive event handling and avoids blocking other system processes.

### REQ.FOO.320 – Event Handling in Main Application

**Description:** The four switch events (long press, short press, depress, startup long press) shall be handled in the main application, not within the foot switch class.  
**Rationale:** Centralizing event handling in the main application maintains separation of concerns and simplifies integration with other system modules.

# 🎚️ Preset Manager Requirements

## REQ.PRE.1xx – Preset Configuration

### REQ.PRE.100 – Preset Range

**Description:** The system shall support a minimum of 2 presets and a maximum of 20 presets.  
**Rationale:** This ensures flexibility while maintaining manageable resource usage.

### REQ.PRE.110 – Preset Wrap-Around

**Description:** When the last preset is incremented, the system shall wrap around to the first preset.  
**Rationale:** Wrap-around behavior provides a seamless user experience without requiring manual reset.

---

## REQ.PRE.2xx – Preset Events

### REQ.PRE.200 – Short Press Event

**Description:** A short press event shall select the next preset.  
**Rationale:** Short press provides a simple and intuitive way to cycle forward through presets.

### REQ.PRE.210 – Long Press Event

**Description:** A long press event shall select the previous preset.  
**Rationale:** Long press provides a simple and intuitive way to cycle backward through presets.

### REQ.PRE.220 – Depress Event Ignored

**Description:** A depress event shall be ignored by the Preset Manager.  
**Rationale:** Ignoring depress events simplifies event handling and avoids unintended actions.

---

## REQ.PRE.3xx – Software Architecture

### REQ.PRE.300 – Dedicated Preset Manager Class

**Description:** The Preset Manager shall be implemented as a dedicated C++ class within ESP-IDF.  
**Rationale:** Encapsulation in a class improves modularity, maintainability, and testability.

### REQ.PRE.310 – Event Handling in Main Application

**Description:** Preset selection events (short press, long press, depress) shall be handled in the main application, not within the Preset Manager class.  
**Rationale:** Centralizing event handling in the main application maintains separation of concerns and simplifies integration with other system modules.

---

## REQ.PRE.4xx – Preset Data

### REQ.PRE.400 – DMX Universe Allocation

**Description:** Each preset shall contain two DMX universes, each with 512 values.  
**Rationale:** This ensures compatibility with DMX lighting systems and provides sufficient control granularity.

### REQ.PRE.410 – Preset Data Storage

**Description:** Preset data shall be stored in a structured format accessible by the Preset Manager class.  
**Rationale:** Structured storage ensures efficient access, modification, and scalability of preset configurations.

### REQ.PRE.420 – Preset Data Persistence in NVRAM

**Description:**
Preset data shall be stored persistently in NVRAM to retain values across system restarts and power cycles.
**Rationale:** NVRAM storage ensures reliability and prevents loss of configuration data, supporting long-term usability.

# Art-Net Requirements

## REQ.ART.1xx – Art-Net Transmission

### REQ.ART.100 – Preset DMX Data Transmission

**Description:** Preset DMX data shall be transmitted via the Art-Net protocol.  
**Rationale:** Art-Net is a widely adopted standard for DMX data distribution over IP networks, ensuring compatibility with lighting control systems.

### REQ.ART.110 – Universe Selection

**Description:** If the second DMX universe is disabled in configuration, only the first universe shall be transmitted.  
**Rationale:** This provides flexibility in system configuration and avoids unnecessary network traffic.

---

## REQ.ART.2xx – Channel Configuration

### REQ.ART.200 – Configurable Channel Limit

**Description:** The system shall only transmit DMX channels up to the maximum value set in configuration.  
**Rationale:** Limiting channel transmission optimizes bandwidth usage and allows tailoring to specific lighting setups.

### REQ.ART.210 – Dynamic Channel Allocation

**Description:** The number of DMX channels transmitted shall be dynamically adjustable based on configuration settings.  
**Rationale:** Dynamic allocation ensures scalability and adaptability to different lighting environments.

---

## REQ.ART.3xx – Software Architecture

### REQ.ART.300 – Art-Net Integration Module

**Description:** Art-Net transmission shall be implemented as a dedicated C++ class within ESP-IDF.  
**Rationale:** Encapsulation in a class improves modularity, maintainability, and testability.

### REQ.ART.310 – Configuration-Driven Behavior

**Description:** Art-Net transmission behavior (universe enable/disable, channel limits) shall be controlled via configuration items.  
**Rationale:** Configuration-driven design allows flexible deployment without code modifications.

# React Website Requirements

## REQ.REA.1xx – Hosting & Deployment

### REQ.REA.100 – React Website Hosting

**Description:** The ESP32 shall host a React-based webpage to provide configuration and preset management.  
**Rationale:** Hosting the webpage directly on the ESP32 ensures a self-contained system without requiring external servers.

### REQ.REA.110 – File System Storage

**Description:** The compiled React pages shall be stored in the ESP32’s file system.  
**Rationale:** Storing the React pages locally allows offline operation and simplifies deployment.

### REQ.REA.120 – React Page Upload

**Description:** Compiled React pages shall be uploadable to the ESP32 for updates.  
**Rationale:** This enables easy updates and maintenance of the user interface without firmware changes.

---

## REQ.REA.2xx – Configuration Management

### REQ.REA.200 – Configuration Settings Interface

**Description:** The React webpage shall provide an interface to manage all configuration settings.  
**Rationale:** A web-based interface simplifies configuration and improves usability.

### REQ.REA.210 – Configuration-Driven Display

**Description:** The React webpage shall only display data according to the configuration settings.  
**Rationale:** Ensures clarity and prevents user confusion by hiding irrelevant options.

---

## REQ.REA.3xx – Preset Management

### REQ.REA.300 – Preset Range Support

**Description:** The React webpage shall support management of 2 to 20 presets.  
**Rationale:** Matches system requirements for preset flexibility.

### REQ.REA.310 – Preset DMX Data Display

**Description:** For each preset, the React webpage shall display the DMX channels of the two universes.  
**Rationale:** Provides visibility and control over preset data for users.

### REQ.REA.320 – Preset Data Upload to NVRAM

**Description:** The React webpage shall be able to send preset data to the ESP32’s NVRAM via a dedicated task.  
**Rationale:** Ensures persistence of presets across restarts and power cycles.

---

## REQ.REA.4xx – ESP32 Integration

### REQ.REA.400 – Dedicated NVRAM Task

**Description:** A specific task shall be created on the ESP32 to handle preset data uploads from the React webpage.  
**Rationale:** Separation of concerns improves reliability and prevents blocking other system tasks.

### REQ.REA.410 – Configuration Synchronization

**Description:** The ESP32 shall synchronize configuration changes made via the React webpage with its internal settings.  
**Rationale:** Ensures consistency between the web interface and system behavior.

# OTA (Over-the-Air Update) Requirements

## REQ.OTA.1?? – General OTA Framework

### REQ.OTA.100 – OTA Support

**Description:** The ESP32 shall support OTA updates for both firmware and React webpage content.  
**Rationale:** OTA updates enable remote maintenance and upgrades without requiring physical access to the device.

### REQ.OTA.110 – Secure OTA Process

**Description:** OTA updates shall be performed securely, ensuring integrity and authenticity of the update files.  
**Rationale:** Security prevents unauthorized modifications and protects system reliability.

---

## REQ.OTA.2?? – Firmware OTA

### REQ.OTA.200 – Firmware Update Capability

**Description:** The ESP32 shall support OTA updates for its firmware.  
**Rationale:** Firmware OTA ensures that bug fixes, new features, and security patches can be deployed remotely.

### REQ.OTA.210 – Firmware Storage & Rollback

**Description:** The ESP32 shall store the new firmware in flash memory and support rollback to the previous version if the update fails.  
**Rationale:** Rollback capability ensures system stability and prevents bricking due to failed updates.

### REQ.OTA.220 – OTA Trigger via Foot Switch

**Description:** A long press of the foot switch at startup shall trigger the firmware OTA process.  
**Rationale:** Provides a simple and user-friendly mechanism to initiate firmware updates.

---

## REQ.OTA.3?? – React Webpage OTA

### REQ.OTA.300 – React Page Update Capability

**Description:** The ESP32 shall support OTA updates for the React webpage files stored in its file system.  
**Rationale:** Enables updates to the user interface without requiring firmware changes.

### REQ.OTA.310 – React Page Upload

**Description:** Compiled React pages shall be uploadable to the ESP32 via OTA.  
**Rationale:** Simplifies deployment and maintenance of the web interface.

### REQ.OTA.320 – Dedicated OTA Task for React

**Description:** A specific task shall be created on the ESP32 to handle React webpage OTA updates.  
**Rationale:** Separation of concerns ensures reliability and prevents interference with firmware OTA processes.

---

## REQ.OTA.4?? – Configuration & Persistence

### REQ.OTA.400 – Configuration-Controlled OTA

**Description:** OTA behavior (enabled/disabled, update sources, triggers) shall be controlled via configuration settings.  
**Rationale:** Configuration-driven design provides flexibility and control over update policies.

### REQ.OTA.410 – Persistent OTA Settings

**Description:** OTA configuration settings shall be stored in NVRAM to ensure persistence across restarts and power cycles.  
**Rationale:** Persistent storage ensures consistent behavior and prevents accidental loss of OTA settings.
