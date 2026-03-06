# Requirements

- Check which requirements are implemented and which are not.

# DMX Controller

- When the number of filled presets is lower than the current preset index, adjust it accordingly and send around.

# RTOS Related

- Split Assert in Assert and AssertRtos (?)
- Remove assert from assert*.assertNotNull(..), change to assert*.notNull, for all assert methods.

# Foot Switch

- Only handle foot switch presses after boot (when presets/config is loaded). For this, add a few booleans in DmxController or internally and optionally use a state.
- Implement circular navigation

# DMX Transmission

- Implement DMX transmission via Max3485 with a rate of 44 Hz.

# Seven Segment Display

- Use a state diagram in case of error showing.
- Show error messages.
- Show 'boot' during boot.
- Show current preset index.

# NvStorage

- (Done) Reading presets and configuration from NVRAM, analyze blocking/asynchronous mechanism.
- Store presets and configuration in NVRAM, analyze blocking/asynchronous mechanism.

# OTA Update

- The OTA update is not implemented yet.

# Web Interface

## OTA Update

- The Cancel/Confirm button and displaying the file name does not work well.
- The file name does not show a path and is removed after Confirm.
- Implement error texts / success texts on the website.

# Testing

## System Testing

- (Done) Think how to test the system. Google mock system testing.

## Unit Testing

- Reduce duplication of code.
