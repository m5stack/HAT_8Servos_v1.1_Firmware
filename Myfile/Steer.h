#ifndef __STEER_H
#define __STEER_H
#ifdef __cplusplus
 extern "C" {
#endif 


#define STEER_NUMBER 8


extern unsigned short int steer_pulse[STEER_NUMBER];
extern void Analog_PWM(void);
extern void SetSteerAngle(unsigned char pos, unsigned char angle);
extern void SetSteerPulse(unsigned char pos, unsigned short int pulse);
extern void SteerUpdate(void);
extern void SteerInit(void);


#ifdef __cplusplus
}
#endif

#endif
