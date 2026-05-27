#include "waveshare_io_extension.h"

io_extension_obj_t IO_EXTENSION;

static void IO_EXTENSION_IO_Mode(uint8_t pin)
{
    uint8_t data[2] = {IO_EXTENSION_Mode, pin};
    DEV_I2C_Write_Nbyte(IO_EXTENSION.addr, data, 2);
}

void IO_EXTENSION_Init()
{
    DEV_I2C_Set_Slave_Addr(&IO_EXTENSION.addr, IO_EXTENSION_ADDR);

    IO_EXTENSION_IO_Mode(0xff);

    IO_EXTENSION.Last_io_value = 0xFF;
    IO_EXTENSION.Last_od_value = 0xFF;

    uint8_t data[2] = {IO_EXTENSION_IO_OUTPUT_ADDR, IO_EXTENSION.Last_io_value};
    DEV_I2C_Write_Nbyte(IO_EXTENSION.addr, data, 2);
}

void IO_EXTENSION_Output(uint8_t pin, uint8_t value)
{
    if (value == 1) {
        IO_EXTENSION.Last_io_value |= (1 << pin);
    } else {
        IO_EXTENSION.Last_io_value &= (~(1 << pin));
    }

    uint8_t data[2] = {IO_EXTENSION_IO_OUTPUT_ADDR, IO_EXTENSION.Last_io_value};
    DEV_I2C_Write_Nbyte(IO_EXTENSION.addr, data, 2);
}

uint8_t IO_EXTENSION_Input(uint8_t pin)
{
    uint8_t value = 0;
    DEV_I2C_Read_Nbyte(IO_EXTENSION.addr, IO_EXTENSION_IO_INPUT_ADDR, &value, 1);
    return ((value & (1 << pin)) > 0);
}
