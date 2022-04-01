#include "servo.h"
#include "tim.h"
#include "i2c_ex.h"

#define ANGLE_TO_PULSE(angle) ((uint16_t)(500 + (angle * 2000 / 180)))
#define PULSE_TO_COUNT(pulse) ((uint8_t)(pulse / 10))
#define PULSE_TO_ANGLE(pulse) ((uint8_t)((pulse - 500) * 180 / 2000))

volatile uint8_t servo_tim_count[SERVER_NUMBER];
volatile uint16_t servo_pulse[SERVER_NUMBER];
volatile uint8_t servo_angle[SERVER_NUMBER];

uint8_t servo_angle_last[SERVER_NUMBER];
uint16_t servo_pulse_last[SERVER_NUMBER];

typedef struct {
  TIM_HandleTypeDef* tim;
  int channel;
} Tim_channel_t;

Tim_channel_t servo_port[] = {
  {&htim3,  TIM_CHANNEL_4},
  {&htim17,  TIM_CHANNEL_1}
};
/**
  * @brief  Update Pulse.
  * @param  pos TIM handle.
  * @param  pulse TIM Channels to be configured.
  *          This parameter can be one of the following values:
  *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
  *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
  *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
  *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
	* @param  __COMPARE__ specifies the Capture Compare register new value.
  */
static void UpdateTimPulse(uint8_t pos, uint16_t pulse)
{
  __HAL_TIM_SET_COMPARE(servo_port[pos].tim, servo_port[pos].channel, pulse);
}

/**
  * @brief  Servo motor initialization.
  */
void ServoInit(void)
{
  for(uint8_t i = 0; i < 12; i++)
  {
    servo_angle_last[i] = 255;
    servo_angle[i] = 255;
  }
  I2CAddReg(0x00, (uint8_t *)servo_angle, SERVER_NUMBER, 8);
  I2CAddReg(0x10, (uint8_t *)servo_pulse, SERVER_NUMBER > 8 ? 16 : SERVER_NUMBER * 2, 16);
}
/**
  * @brief  Set servo motor rotation.
  * @param  pos TIM handle.
  * @param  angle Rotation angle.
  */
void SetServoAngle(uint8_t pos, uint8_t angle) 
{
  if(angle > 180) return;
  servo_angle[pos] = angle;
  servo_pulse[pos] = ANGLE_TO_PULSE(angle);
  servo_tim_count[pos] = PULSE_TO_COUNT(servo_pulse[pos]);
  UpdateTimPulse(pos, servo_pulse[pos]);
}
/**
  * @brief  Set servo motor Pulse.
  * @param  pos TIM handle.
  * @param  pulse Number of pulses.
  */
void SetServoPulse(uint8_t pos, uint16_t pulse)
{
  if(pulse > 2500) return;
  if(pulse < 500) return;
  servo_pulse[pos] = pulse;
  servo_angle[pos] = PULSE_TO_ANGLE(pulse);
  servo_tim_count[pos] = PULSE_TO_COUNT(pulse);
  UpdateTimPulse(pos, servo_pulse[pos]);  
}
/**
  * @brief  Update the status of the servo motor.
  */
void ServoUpdate(void)
{
  for(uint8_t i = 0; i < SERVER_NUMBER; i++) {
    if(servo_angle[i] != servo_angle_last[i]) {
      SetServoAngle(i, servo_angle[i]);
      servo_angle_last[i] = servo_angle[i];
      servo_pulse_last[i] = servo_pulse[i];
    }

    if(servo_pulse[i] != servo_pulse_last[i]) {
      SetServoPulse(i, servo_pulse[i]);
      servo_angle_last[i] = servo_angle[i];
      servo_pulse_last[i] = servo_pulse[i];
    }
  }
}
