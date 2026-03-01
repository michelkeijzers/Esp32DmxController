# 100-199 Overall Hardware Architecture and Design

## FR-100 Microcontroller Selection ✅

**Description:** The system should use a suitable microcontroller. It must have sufficient processing power, memory, and peripheral support.<br/>
**Rationale:** _The microcontroller must have sufficient processing power, memory, and peripheral support to meet the system requirements._

### DD-100 ESP32 Selection ✅

**Description:** An ESP32 C3 dev board will be used for this project. <br/>
**Rationale:** _The ESP32 C3 offers a good balance of price, performance, memory, and peripheral support, making it suitable for our application._

# 200-299 Overall Software Architecture and Design

## NF-200 Software Expansion and Modularity

**Description:** The software architecture shall allow new modules to be added without modifying existing ones (except the main module).<br/>
**Rationale:** _Modularity supports scalability and future feature expansion._

### DD-200 ESP-IDF and FreeRTOS Usage

**Description:** The software will be developed in C++ using the ESP-IDF framework and FreeRTOS for task management.
Every module will gets its own RTOS task.<br/>
**Rationale:** _ESP-IDF provides a robust development environment for ESP32, while FreeRTOS enables efficient multitasking and real-time performance.
The use of C++ allows for object-oriented design, improving code organization and maintainability._

### DD-201 Event-Driven Design

**Description:** The system will be designed using an event-driven architecture. The events will be handled typically by RTOS queues and will all
be flown through a main task.<br/>
**Rationale:** _Event-driven design ensures responsiveness and efficient resource usage, being isolated from other tasks and allowing for better
separation of concerns._

## FR-210 Visible Error Indication

**Description:** The user shall get a visible indication of any errors that occur during operation, such as invalid input or communication failures.
<br/>
**Rationale:** \_Providing visible error indications improves user experience and allows for easier troubleshooting.

### DD-210 Structured Error Handling

**Description:** All modules shall implement structured error handling, returning status codes or exceptions where applicable.<br/>
**Rationale:** _Robust error handling enhances system reliability and maintainability._

# 300-399 Foot Switch

## FR-300 Foot Switch Controlled Preset Iteration

**Description:** The foot switch shall allow the user to iterate through presets using short and long presses. A short press shall select the next
preset, while a long press shall select the previous preset.<br>
**Rationale:** _Short press provides a simple and intuitive way to cycle forward through presets which is the default direction of iterating._<br/>
**Relates to:** TODO: Webpage configuration for circular iteration.

### DD-320 Long Press Detection

**Description:** The system shall detect long presses of the foot switch, defined as a press duration of at least one second. The timing threshold for long presses will be configurable through the web interface.<br/>
**Rationale:** _Long press detection allows for additional functionality (e.g., selecting the previous preset) and provides flexibility for user preferences._<br/>
**Relates to:** DD-300 Foot Switch Controlled Preset Iteration, TODO: Webpage configuration for long press duration.

### DD-310 6.3mm Foot Switch Connector

**Description:** A standard 6.3mm foot switch connector will be used for the foot switch input. The system will support both a normally open (NO) and normally closed (NC) foot switch.<br/>
**Rationale:** _Foot switches typically use a 6.3mm connector, ensuring compatibility with a wide range of commercially available foot switches. The foot switch can be further away from the device, providing flexibility in placement._<br/>
**Relates to:** TODO: Webpage configuration for foot switch type (NO/NC).

### DD-350 Foot Switch Debouncing

**Description:** The foot switch input shall be debounced. The website will provide a configuration option for the debounce duration.<br/>
**Rationale:** _Debouncing ensures accurate detection of switch presses and prevents false triggers caused by mechanical bounce._<br/>
**Relates to:** TODO: Webpage configuration for debounce duration.

### DD-310 Foot Switch Interrupt-Driven Event Handling

**Description:** The foot switch state checks will be performed using interrupts rather than polling. Depressing foot switch changes are ignored.<br/>
**Rationale:** Interrupts ensure responsiveness and efficient CPU usage compared to polling.
Ignoring depress events prevents unintended preset changes when the foot switch is released.

# 400-499 Presets

## FR-400 Preset Management

**Description:** The system shall support a range of presets that can be selected via the foot switch and managed through the web interface.
Each preset will contain DMX data for one universe (512 channels).<br/>
**Rationale:** _Preset management keeps the DMX data for each of the presets organized and easily accessible._

### DD-400 Preset Amount

**Description:** The system shall support a minimum of 2 presets and a maximum of 20 presets. <br/>
**Rationale:** _This range provides flexibility while maintaining manageable resource usage.
Foot switch presses will iterate through the available presets._

### DD-410 Preset Data Structure

**Description:** The preset data will contain 512 DMX channel values for one universe, stored in a structured format. <br/>
**Rationale:** _A single universe is enough, as the fixture setup will stay small and only one DMX connection is required._

### DD-450 Preset Data Mutex Protection

**Description:** Access to preset data shall be protected by mutexes to ensure thread safety when accessed by multiple tasks.
Only when writing, the mutex shall be locked to prevent concurrent modifications.<br/>
**Rationale:** _Mutex protection prevents data corruption and ensures consistency when multiple tasks access preset data._

# 500-599 Non Volatile Storage

## FR-500 NVRAM Storage

## FR-500 NVRAM Configuration Storage

**Description:** Configuration settings shall be stored in NVRAM to ensure persistence across power cycles. <br/>
**Rationale:** _Storing configuration settings in NVRAM ensures that user preferences and system configurations are retained even when the device is powered off, enhancing reliability and user experience._

## FR-510 NVRAM Preset Storage

**Description:** Preset data shall be stored in NVRAM to ensure persistence across power cycles. <br/>
**Rationale:** _Storing preset data in NVRAM ensures that user-defined presets are retained even when the device is powered off, enhancing reliability and user experience._

# 600-699 DMX Transmission

## FR-600 DMX Transmission

**Description:** The system shall transmit DMX data to a DMX connector.<br/>
**Rationale:** _Transmitting DMX data is the core functionality of the system, allowing it to control lighting fixtures and other DMX-compatible devices._

### DD-600 DMX Transmission Implementation

**Description:** DMX transmission will be handled through a Max3485 Dev board connected to the ESP32. The transmission can be enabled/disabled.<br/>
**Rationale:** _Using a dedicated DMX output board ensures reliable signal transmission and simplifies hardware integration._<br/>
**Relates to:** TODO: React webpage configuration for enabling this feature.

## DD-610 Single Universe

**Description:** The system will transmit DMX data for a single universe (512 channels).<br/>
**Rationale:** _A single universe is sufficient for the intended use case, simplifying the design and reducing resource requirements._

# 700-799 Display

## FR-700 Display

**Description:** The system shall provide a display to show the current preset and other relevant information such as error conditions. <br/>
**Rationale:** _A display provides immediate feedback to the user about the current system state and any errors that may occur, improving usability and troubleshooting._

### DD-700 Seven Segment Display, 4 digits

**Description:** A 4-digit seven segment display will be used to show the current preset number and error codes.
4 digits are enough to show the preset number (up to 20) and error codes. <br/>
An internal TM1637 driver will be used to control the display, to reduce the number of GPIO pins required. <br/>
**Rationale:** _A seven segment display is a simple and effective way to display numeric information. When using a C3 with limited GPIO pins, a TM1637 driver allows for controlling the display with fewer pins, simplifying hardware design._

# 800-899 OTA

## FR-800 OTA Updates

**Description:** The ESP32 shall support OTA updates for both firmware and React webpage content.<br/>
**Rationale:** OTA updates enable remote maintenance and upgrades without requiring physical access to the device.

## FR-810 OTA Update Trigger

**Description:** The OTA update will be triggered by an option on the website. Success or failure of the update will be indicated on the web interface. <br/>
**Rationale:** Providing an option on the website allows users to easily initiate OTA updates without needing physical interaction with the device. The display does not need to be used for this, as the web interface can provide more detailed feedback on the update process.

# FR 900-999 Web Interface

## FR-900 Generic Functionality

**Description:** The web interface shall provide functionality for a Blackout, OTA support, showing the user manual and the ESP32 IP Address.<br/>
**Rationale:** _These features provide essential functionality for controlling the system, performing maintenance, and accessing important information._

### DD-900 ESP32 IP Address Display

**Description:** The web interface shall display the current IP address of the ESP32. <br/>

### DD-902 Blackout

**Description:** The web interface shall include a Blackout button that, when pressed, will immediately set all DMX channel values to zero, effectively turning off all connected fixtures. <br/>
**Rationale:** _A Blackout function allows users to quickly turn off all connected fixtures before/after the gig or during breaks._

### DD-904 OTA Support

**Description:** The web interface shall include an option to trigger OTA updates for both firmware and webpage content. A file can be selected which is then transmitted to the ESP32 for updating. The interface will provide feedback on the success or failure of the update process. <br/>
**Rationale:** _Providing OTA update functionality through the web interface allows users to easily maintain and upgrade the system without needing physical access to the device._

### DD-906 User Manual

**Description:** The web interface shall provide access to the user manual, allowing users to view instructions and guidance for operating the system. <br/>
**Rationale:** _Providing the user manual through the web interface ensures that users have easy access to important information and instructions, improving usability and reducing the need for physical documentation._

## FR-920 Presets Web Interface

**Description:** A web interface shall be used to manage presets, allowing users to view, edit, and upload preset data to the ESP32. <br/>
**Rationale:** _A web-based interface simplifies preset management and provides a user-friendly way to interact with the system._

### DD-920 Presets List

**Description:** The web interface shall display a list of available presets, showing the numbers of presets, and per preset, the sequence number, name, two arrows for changing the order, a Plus button for inserting a preset and an X button for deleting a preset. <br/>
**Rationale:** _Displaying a list of presets allows users to easily navigate and manage their presets._

### DD-922 Preset Editor

**Description:** The web interface shall provide a preset editor, allowing users to modify the details of individual presets, including name and the value of each DMX address.<br/>
**Rationale:** _A preset editor allows users to customize their presets directly through the web interface, enhancing usability and flexibility._

### DD-924 DMX Value Editor

**Description:** The web interface shall include a DMX value editor, allowing users to set the value for each DMX channel within a preset. Big buttons should be used so users can edit also via a small phone touchscreen. <br/>
**Rationale:** _A DMX value editor allows users to easily edit the valueo._

## FR-950 Configuration Web Interface

**Description:** A web interface shall be provided for users to configure system settings. The interface will allow users to view, edit the following settings:

- Maximum number of presets
- Circular Navigation (on/off)
- Foot switch polarity (Normally Open/Normally Closed)
- Long press duration threshold <br/>

**Rationale:** _A web interface allows users to easily configure the system and manage settings without needing physical access to the device or complex software tools._

### DD-950 Configuration Screen

**Description:** The web interface shall include a single configuration screen with all options.<br/>
**Rationale:** _Having a single configuration screen provides a centralized location for users to manage all settings, improving usability and accessibility._
