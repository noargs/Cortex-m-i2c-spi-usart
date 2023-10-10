#ifndef I2C_DRIVERS_F446XX_H_
#define I2C_DRIVERS_F446XX_H_

#include "stm32f4xx.h"

#define HSI_SOURCE                  0U
#define HSE_SOURCE                  1U
#define PLL_SOURCE                  2U

#define SLAVE_ADDRESS               0x68
#define MY_ADDR                     0x64

#define SPEED_STANDARD
// OR
//#define SPEED_FAST
//#define DUTY_ZERO

#define I2C_ENABLE_SR               1U
#define I2C_DISABLE_SR              0U

#define I2C_SCL_SPEED_SM            100000U
#define I2C_SCL_SPEED_FM2K          200000U
#define I2C_SCL_SPEED_FM4K          400000U

// @i2c_application_states
#define I2C_READY                   0U
#define I2C_BUSY_IN_RX              1U
#define I2C_BUSY_IN_TX              2U

typedef struct
{
  I2C_TypeDef  *i2cx;
  uint8_t      *tx_buffer;
  uint8_t      *rx_buffer;
  uint32_t     tx_length;
  uint32_t     rx_length;
  uint8_t      tx_rx_state;
  uint8_t      device_address;
  uint32_t     rx_size;
  uint8_t      repeated_start_sr;
} i2c_handle_t;

void I2C_Inits (I2C_TypeDef *i2cx);
void I2C_MasterSendData (I2C_TypeDef *i2cx, uint8_t *tx_buffer, uint32_t len, uint8_t slave_addr, uint8_t repeated_start);
void I2C_MasterReceiveData (I2C_TypeDef *i2cx, uint8_t *rx_buffer, uint8_t len, uint8_t slave_addr, uint8_t repeated_start);

uint8_t GPIO_ReadFromInputPin (GPIO_TypeDef *gpiox, uint8_t pin_number);

uint8_t I2C_MasterSendDataInterrupt (i2c_handle_t *i2c_handle, uint8_t *tx_buffer, uint32_t length, uint8_t slave_address, uint8_t repeated_start);
uint8_t I2C_MasterReceiveDataInterrupt (i2c_handle_t *i2c_handle, uint8_t *rx_buffer, uint32_t length, uint8_t slave_address, uint8_t repeated_start);


#endif /* I2C_DRIVERS_F446XX_H_ */
