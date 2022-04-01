#ifndef __MOTOR_H
#define __MOTOR_H
#ifdef __cplusplus
 extern "C" {
#endif 

#define MOTOR_NUMBER 2 //Number of motors.

extern void MotorUpdate(void);
extern void MotorInit(void);

#ifdef __cplusplus
}
#endif

#endif
