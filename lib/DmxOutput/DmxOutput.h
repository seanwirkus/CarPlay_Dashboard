#pragma once

#include <Arduino.h>
#include <driver/uart.h>

class DmxOutput {
public:
    static constexpr size_t kUniverseSize = 512;

    bool begin(uint8_t txPin, int8_t dirPin, uart_port_t uartNum = UART_NUM_1);
    void setSlot(uint16_t slot, uint8_t value);
    uint8_t getSlot(uint16_t slot) const;
    void clear();
    void sendFrame();

private:
    bool configureUart(int baudRate, uart_stop_bits_t stopBits);

    uart_port_t _uartNum = UART_NUM_1;
    int8_t _dirPin = -1;
    bool _ready = false;
    uint8_t _frame[kUniverseSize + 1] = {0};
};
