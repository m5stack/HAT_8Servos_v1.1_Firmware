#include "motor.h"
#include "tim.h"
#include "i2c_ex.h"


#define ABS(x) (((x) > 0) ? (x) : (-(x)))
#define SPEED_TO_COUNT(speed) ((uint16_t)(ABS(speed) * 1000 / 127))

int8_t motor_speed[MOTOR_NUMBER];
int8_t motor_speed_last[MOTOR_NUMBER];

typedef struct {
  TIM_HandleTypeDef *ahtim;
	TIM_HandleTypeDef *bhtim;
  uint16_t ahead_ch;
  uint16_t back_ch;
} MotorTim_t;


MotorTim_t motor_list[MOTOR_NUMBER] = {
  {&htim3, &htim17, TIM_CHANNEL_1, TIM_CHANNEL_1},
	{&htim14, &htim3, TIM_CHANNEL_1, TIM_CHANNEL_4},
};

/**
  * @brief  Motor initialization.
  */
void MotorInit(void)
{
  I2CAddReg(0x30, (uint8_t *)motor_speed, MOTOR_NUMBER, 8);
}
/**
  * @brief  Set motor speed.
  * @param  pos TIM handle.
  * @param  angle motor speed.
  */
void MotorSetSpeed(uint8_t pos, int8_t speed)
{
//	if(speed > 100 || speed < -100)
//		return;
  if(speed >= 0) 
  {
    __HAL_TIM_SET_COMPARE(motor_list[pos].bhtim, motor_list[pos].back_ch, 0);
    __HAL_TIM_SET_COMPARE(motor_list[pos].ahtim, motor_list[pos].ahead_ch, SPEED_TO_COUNT(speed));
  } 
  else 
  {
    __HAL_TIM_SET_COMPARE(motor_list[pos].ahtim,  motor_list[pos].ahead_ch, 0);
    __HAL_TIM_SET_COMPARE(motor_list[pos].bhtim, motor_list[pos].back_ch, SPEED_TO_COUNT(speed));
  }
}
/**
  * @brief  motor update.
  */
void MotorUpdate(void)
{
  for(uint8_t i = 0; i < MOTOR_NUMBER; i++)
  {
    if(motor_speed_last[i] != motor_speed[i])
    {
      motor_speed_last[i] = motor_speed[i];
      MotorSetSpeed(i, motor_speed[i]);
    }
  }
}

