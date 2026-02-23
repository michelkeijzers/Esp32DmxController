# Software Notes

## Print Statements/Tracing

The ESP32 Devkit 2 board has a built-in USB-to-serial converter, which allows for easy debugging using print statements. The serial output can be viewed using the Arduino IDE's Serial Monitor or any terminal program that supports serial communication (e.g., PuTTY, minicom).

For the ESP32 C3, the following setting need to be made for ESP IDF:

- idf.py menuconfig
  Componentn config -> ESP System Settings -> Channel for console output -> Set to: USB Serial/JTAG Controller instead of UART0 (as UART0 is used for DMX transmission)

# General Design

## Main Components

- ESP32 microcontroller
- Seven segment display (4 digits with dot), with TM1637 driver.
- DMX MAX3485 Dev board
- Enclosure
- Power supply (5V, 1A), external
- External foot switch (connected via 6.3mm jack)

### Enclosure

Front:

- Seven segment display (4 digits with dot)

Back:

- Barrel jack for power input (5V, 1A)
- 3-pin DMX output connector
- 6.3mm connector for foot switch

Internal:

- ESP32 microcontroller DevKit Board
- DMX MAX3485 Dev board
- Perf board for connecting components
- Wiring

### Perf Board

The perf board will be used to connect the ESP32 DevKit Board, DMX MAX3485 Dev board and electrical components.
The perf board has 31 x 26 holes and on the left/right side bigger pads.

To fit the perf board in the enclosure, the 4 courners need to be cut.

# Electrical Design

## Wiring and Connections

- ESP32 DevKit Board:
  - Power: 5V and GND from the barrel jack
  - TM1637 Display: CLK and DIO pins connected to GPIO pins on the ESP32
  - DMX MAX3485 Dev board: TXD, RXD, VCC, and GND connected to corresponding pins on the ESP32
  - Foot switch: Connected to a GPIO pin on the ESP32 for input detection
  - DMX output: Connected to the DMX MAX3485 Dev board for transmitting DMX data

### ESP32 Pin Mapping Table

| ESP32 Pin | Connected Device/Function | Target Pin/Connector | Notes                          |
| --------- | ------------------------- | -------------------- | ------------------------------ |
| GPIO16    | MAX3485 Sender            | DI                   | UART2 TX (unused but reserved) |
| GPIO18    | MAX3485 Sender            | DE/RE                | Direction control              |
| GPIO21    | Seven Segment Display     | CLK                  | TM1637 CLK                     |
| GPIO22    | Seven Segment Display     | DIO                  | TM1637 DIO                     |
| GPIO25    | Foot Switch               | Switch Input         | Digital Input                  |
| 5V        | Power Supply              | Barrel Jack 5V       | Main power input               |
| GND       | Power Supply              | Barrel Jack GND      | Ground reference               |

In case to use a ESP32 C3 Dev board, the pin mapping would be as follows:

| ESP32 C3 Pin | Connected Device/Function | Target Pin/Connector | Notes             |
| ------------ | ------------------------- | -------------------- | ----------------- |
| GPIO4        | MAX3485 Sender            | DI                   | UART1 TX for DMX  |
| GPIO5        | MAX3485 Sender            | DE/RE                | Direction control |
| GPIO6        | Seven Segment Display     | CLK                  | TM1637 CLK        |
| GPIO7        | Seven Segment Display     | DIO                  | TM1637 DIO        |
| GPIO10       | Foot Switch               | Switch Input         | Digital Input     |
| 5V           | Power Supply              | Barrel Jack 5V       | Main power input  |
| GND          | Power Supply              | Barrel Jack GND      | Ground reference  |

## Perf Board Layout

```
  ┌────────────────────────────────────────────────────────────────┐
  │ Perf Board Layout (31 x 26 holes)                              |
  | B C D E F G H I J K L M N O P Q R S T U V W X Y Z A B C D E Ter|
  ├────────────────────────────────────────────────────────────────┤
 A│       o o o o o o o SS--------------------------------------SS │
 B│       o o o o o o o |                                        | │
 C│       o o o o o o o |                                        | │
 D│ +-----------------+ |                                        | │
 E│ |                 | |                                        | │
 F│ |   USB Plug for  | |       ESP32 DevKit Board               | │
 G│ |   ESP32 DevKit  | |                                        | │
 H│ |     Board       | |                                        | │
 I│ +-----------------+ |                                        | │
 J│ o o o o o o o o o o |                                        | │
 K│ o o o o o o o o o o |                                        | │
 L│ o o o o o o o o o o SS--------------------------------------SS │
 M│ o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o  o │
 N│ o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o  o │
 O│ o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o  o │
 P│ o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o  o │
 Q│ o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o  o │
 R│ o o o o o o o o o o o o o o o o o o o o o o o o o o o o o o  o │
 S│ o o o +--------SS---------------------------------------SS+  o │
 T│ o o o | +----+                                            |  o │
 U│ o o o | |Ter |                                   VCC ======  o │
 V│ o o o | |min |                                   TXD ======  o │
 W│       | | al |        MAX3485 Module             RXD ======  o │
 X│       | |    |                                   GND ======  o │
 Y│       | +----+                                            |  o │
 Z│       +-------SS----------------------------------------SS+    │
  └────────────────────────────────────────────────────────────────┘
```

## Seven Segment Display

The seven segments are controlled by the TM1637 driver, which is connected to the ESP32 via two GPIO pins (CLK and DIO). The display will show the current preset number, status messages, and other relevant information.
It has a built in TM1637 driver, so it only requires two GPIO pins for control (CLK and DIO). The display can show numbers, letters, and some symbols, which will be used to indicate the current preset, status messages, and other relevant information.

The TM1637 driver can produce the following characters on the seven segment display:

- Numbers: 0-9
- Uppercase Letters: A, B (8), C, E, F, G, H, I (1), J, L, N, O (0), P, S (5), U, Y
- Lowercase Letters: b , c, d, e, g, h, i , j, n, o , q, r, t, u
- Symbols: dash (-), underscore (\_), degree (°), blank ( ), colon (clock colon), equals (=), triple bar (≡), left bracket ([), right bracket (]), apostrophe ('), quotes ("), caret (^), vertical bar (|), overline (‾)

# Manufacturing Design

The perf board will lay on the bottom of the enclosures and can be attached with the two screws of the enclosure bottom.
The ESP32 Devkit Board and the DMX MAX3485 Dev board will be attached to the perf board with screws and standoffs.
All other components are attached at the sides of the enclosures.
