#include "seven_segment_display.hpp"
#include <esp_check.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char *LOG_TAG = "SevenSegmentDisplay";
static const int QUEUE_CAPACITY = 10;
static const int TASK_PRIORITY = 5;

// Digit patterns for common cathode 7-segment display
// Each bit represents a segment: bit 0 = A, 1 = B, 2 = C, 3 = D, 4 = E, 5 = F, 6 = G
// DP is handled separately
/* static */ const uint8_t SevenSegmentDisplay::digitPatterns_[] = {
    0b00111111, // 0: ABCDEF
    0b00000110, // 1: BC
    0b01011011, // 2: ABDEG
    0b01001111, // 3: ABCDG
    0b01100110, // 4: BCFG
    0b01101101, // 5: ACDFG
    0b01111101, // 6: ACDEFG
    0b00000111, // 7: ABC
    0b01111111, // 8: ABCDEFG
    0b01101111, // 9: ABCDFG
    0b01110111, // A: ABCEFG
    0b01111100, // b: CDEFG
    0b00111001, // C: ADEF
    0b01011110, // d: BCDEG
    0b01111001, // E: ADEFG
    0b01110001, // F: AEFG
    0b01101111, // g: ABCDFG
    0b01110110, // H: BCEFG
    0b00000110, // i: BC
    0b00001110, // j: BCD
    0b00111000, // l: DEF
    0b00011100, // L: DEF
    0b01110110, // n: CEFG
    0b01111110, // o: CDEFG
    0b01110011, // p: ABEFG
    0b01100111, // q: ABCFG
    0b01010100, // r: EG
    0b01101101, // s: ACDFG
    0b01111000, // t: DEF + G
    0b00111110, // u: CDEF
    0b00111110, // U: BCDEF
    0b01111110, // y: BCDEFG
    0b01111110, // Y: BCDEFG
    0b00000001, // - (dash): G
    0b01000000, // _ (underscore): D
    0b00001000, // = (equals): G + D
    0b10000000, // . (decimal point): DP (if available)
    0b00000000, // (space): none
    0b11111111  // All segments on
};

SevenSegmentDisplay::SevenSegmentDisplay() : RtosTask(), currentPattern_(0), decimalPointOn_(false) {}

SevenSegmentDisplay::~SevenSegmentDisplay() {}

esp_err_t SevenSegmentDisplay::init(QueueHandle_t dmxControllerEventQueue, const gpio_num_t pins[8]) {
    if (RtosTask::init("SevenSegmentDisplayTask", 2048, TASK_PRIORITY, QUEUE_CAPACITY, sizeof(Event),
            dmxControllerEventQueue) != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Failed to initialize SevenSegmentDisplayTask");
        return ESP_FAIL;
    }

    if (!pins) {
        ESP_LOGE(LOG_TAG, "Invalid pins array");
        return ESP_ERR_INVALID_ARG;
    }

    currentPattern_ = 0;
    decimalPointOn_ = false;

    // Configure GPIO pins
    for (int i = 0; i < 8; i++) {
        segmentPins_[i] = pins[i];

        gpio_config_t io_conf = {.pin_bit_mask = (1ULL << pins[i]),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE};

        if (gpio_config(&io_conf) != ESP_OK) {
            ESP_LOGE(LOG_TAG, "Failed to configure GPIO pin %d", pins[i]);
            return ESP_FAIL;
        }
        segmentPins_[i] = pins[i];
    }

    ESP_LOGI(LOG_TAG, "SevenSegmentDisplay task started");
    return ESP_OK;
}

void SevenSegmentDisplay::taskEntry(void *param) { static_cast<SevenSegmentDisplay *>(param)->taskLoop(); }

void SevenSegmentDisplay::taskLoop() {
    Event event;
    while (true) {
        if (xQueueReceive(eventQueue_, &event, portMAX_DELAY) == pdTRUE) {
            displayDigit(event.character, event.dot);
        }
    }
}

esp_err_t SevenSegmentDisplay::displayDigit(char character, bool dot) {
    esp_err_t ret = ESP_OK;
    uint8_t pattern = 0;
    decimalPointOn_ = dot;

    // Map character to pattern
    if (character >= '0' && character <= '9') {
        pattern = digitPatterns_[character - '0'];
    } else if (character >= 'A' && character <= 'F') {
        pattern = digitPatterns_[10 + (character - 'A')];
    } else if (character >= 'a' && character <= 'f') {
        pattern = digitPatterns_[10 + (character - 'a')];
    } else if (character == '-') {
        pattern = digitPatterns_[32];
    } else if (character == '_') {
        pattern = digitPatterns_[33];
    } else if (character == '=') {
        pattern = digitPatterns_[34];
    } else if (character == '.') {
        pattern = digitPatterns_[35];
    } else if (character == ' ') {
        pattern = digitPatterns_[36];
    } else {
        pattern = digitPatterns_[37]; // All segments on for unknown
        ret = ESP_ERR_INVALID_ARG;
    }

    if (ret == ESP_OK) {
        currentPattern_ = pattern;
        ret = updateDisplay();
    }
    return ret;
}

esp_err_t SevenSegmentDisplay::updateDisplay() {
    if (!initialized_) {
        return ESP_ERR_INVALID_STATE;
    }

    // Set each segment based on the current pattern and display type
    for (int i = 0; i < 7; i++) { // Segments A-G
        bool segmentOn = (currentPattern_ & (1 << i)) != 0;
        bool gpioLevel;

        gpioLevel = !segmentOn; // LOW = on for common anode

        gpio_set_level(segmentPins_[i], gpioLevel ? 1 : 0);
    }

    // Set decimal point
    bool dpGpioLevel = !decimalPointOn_;
    gpio_set_level(segmentPins_[SEG_DP], dpGpioLevel);

    return ESP_OK;
}