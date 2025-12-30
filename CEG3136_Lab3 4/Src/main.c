// Standard library headers
#include <stdio.h>
// Device driver headers
#include "systick.h"
#include "i2c.h"
#include "gpio.h"
#include "touchpad.h"
#include "spi.h"
// App headers
#include "alarm.h"
#include "whackamole.h"
#include "display.h"
#include "calc.h"
#include "enviro.h"
#include "motor.h"

int main (void)
{
 // Initialize apps
 Init_Alarm();
 Init_WhackAMole();
 Init_Calc();
 Init_Enviro();
 Init_Motor();
 // Enable services
 StartSysTick();
 while (1) {
 // Run apps
 Task_Alarm();
 Task_WhackAMole();
 Task_Calc();
 Task_Enviro();
 Task_Motor();

 UpdateIOExpanders();
 UpdateDisplay();
 ScanTouchpad();
 ServiceI2CRequests();
 // Service SPI-based peripherals (Enviro, etc.)
 ServiceSPIRequests();
 WaitForSysTick();
 }
}
