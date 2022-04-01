#ifndef __SERVO_H
#define __SERVO_H
#ifdef __cplusplus
 extern "C" {
#endif 

#include "gpio.h"

#define SERVER_NUMBER 2// Set the number of servo motors

extern volatile uint16_t servo_gpioa_enable;
extern volatile uint16_t servo_gpiob_enable;
extern volatile uint16_t servo_gpiof_enable;
extern volatile uint8_t servo_tim_count[SERVER_NUMBER];

extern void SetServoAngle(uint8_t pos, uint8_t angle);
extern void SetServoPulse(uint8_t pos, uint16_t pulse);
extern void ServoUpdate(void);
extern void ServoInit(void);

#ifdef __cplusplus
}
#endif

#endif
