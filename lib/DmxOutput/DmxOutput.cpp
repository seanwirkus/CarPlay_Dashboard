#include "DmxOutput.h"

#include <freertos/FreeRTOS.h>

namespace {

constexpr int kDmxBaudRate = 250000;
constexpr int kBreakBaudRate = 83333;
constexpr uint8_t kBreakByte = 0x00;

}  // namespace

bool DmxOutput::begin(uint8_t txPin, int8_t dirPin, uart_port_t uartNum) {
    _uartNum = uartNum;
    _dirPin = dirPin;
    clear();

    if (_dirPin >= 0) {
        pinMode(_dirPin, OUTPUT);
        digitalWrite(_dirPin, HIGH);
    }

    esp_err_t err = uart_driver_install(_uartNum, 0, 1024, 0, nullptr, 0);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        return false;
    }

    err = uart_set_pin(_uartNum, txPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (err != ESP_OK) {
        return false;
    }

    _ready = configureUart(kDmxBaudRate, UART_STOP_BITS_2);
    return _ready;
}

void DmxOutput::setSlot(uint16_t slot, uint8_t value) {
    if (slot == 0 || slot > kUniverseSize) {
        return;
    }
    _frame[slot] = value;
}

uint8_t DmxOutput::getSlot(uint16_t slot) const {
    if (slot == 0 || slot > kUniverseSize) {
        return 0;
    }
    return _frame[slot];
}

void DmxOutput::clear() {
    memset(_frame, 0, sizeof(_frame));
}

void DmxOutput::sendFrame() {
    if (!_ready) {
        return;
    }

    configureUart(kBreakBaudRate, UART_STOP_BITS_1);
    uart_write_bytes(_uartNum, reinterpret_cast<const char*>(&kBreakByte), 1);
    uart_wait_tx_done(_uartNum, pdMS_TO_TICKS(10));
    delayMicroseconds(16);

    configureUart(kDmxBaudRate, UART_STOP_BITS_2);
    uart_write_bytes(_uartNum, reinterpret_cast<const char*>(_frame), sizeof(_frame));
    uart_wait_tx_done(_uartNum, pdMS_TO_TICKS(30));
}

bool DmxOutput::configureUart(int baudRate, uart_stop_bits_t stopBits) {
    uart_config_t config = {};
    config.baud_rate = baudRate;
    config.data_bits = UART_DATA_8_BITS;
    config.parity = UART_PARITY_DISABLE;
    config.stop_bits = stopBits;
    config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    config.rx_flow_ctrl_thresh = 0;
#if defined(UART_SCLK_DEFAULT)
    config.source_clk = UART_SCLK_DEFAULT;
#endif

    return uart_param_config(_uartNum, &config) == ESP_OK;
}
