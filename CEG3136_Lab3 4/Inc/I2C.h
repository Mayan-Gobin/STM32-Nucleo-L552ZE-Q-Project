#ifndef I2C_H_
#define I2C_H_


#include <stdbool.h>
#include "stm32l5xx.h"
#include "gpio.h"


// I2C bus connection
typedef struct {
   I2C_TypeDef *iface;   // Interface registers I2C1-I2C4
   Pin_t        pinSDA;  // MCU pin for SDA
   Pin_t        pinSCL;  // MCU pin for SCL
} I2C_Bus_t;


extern I2C_Bus_t LeafyI2C;   // I2C bus on Leafy mainboard


// I2C transfer record
typedef struct I2C_Xfer_t {
   I2C_Bus_t       *bus;   // Pointer to I2C bus structure
   uint8_t          addr;  // 7-bit target address and read/write bit
   uint8_t         *data;  // Pointer to data buffer
   int              size;  // Total number of bytes in transfer
   bool             stop;  // Whether or not to issue a STOP condition
   bool             busy;  // Busy indicator (queued or in progress)
   struct I2C_Xfer_t *next; // Pointer to next transfer in queue
} I2C_Xfer_t;


void I2C_Enable(I2C_Bus_t bus);   // Enable I2C bus connection
void I2C_Request(I2C_Xfer_t *p);   // Request a new transfer
void ServiceI2CRequests(void);     // Called from main loop


#endif /* I2C_H_ */
