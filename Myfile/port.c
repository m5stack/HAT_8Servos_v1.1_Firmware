#include "tim.h"
#include "port.h"
#include "Steer.h"
#include "motor.h"

typedef struct 
{
  GPIO_TypeDef* GPIOx;
  uint16_t GPIO_Pin;
}PWM_GPIO;//Ä£ÄâPWM½Ó¿Ú

volatile unsigned short int motorCount = 0;
volatile unsigned short int steerCount = 0;
extern unsigned short int steer_pulse[];
extern unsigned short int motor_pulse[];
extern PWM_GPIO steer_list[];
extern PWM_GPIO motor_list[];

/**
  * @brief  Rewrite the callback function.
  * @param  Callback function parameters.
  */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==htim14.Instance)
	{
		if(++steerCount==20000)
		{
			steerCount=0;
		}
		if(steerCount==0)
		{
			for(int i=0; i<STEER_NUMBER; i++)
			{
				HAL_GPIO_WritePin(steer_list[i].GPIOx,steer_list[i].GPIO_Pin,GPIO_PIN_SET);
			}
		}
		for(int i=0; i<STEER_NUMBER; i++)
		{
			if(steerCount == steer_pulse[i])
				HAL_GPIO_WritePin(steer_list[i].GPIOx,steer_list[i].GPIO_Pin,GPIO_PIN_RESET);
		}
		if(++motorCount==1000)
		{
			motorCount=0;
		}
		if(motorCount==0)
		{
			for(int i=0; i<MOTOR_NUMBER*2; i++)
			{
				HAL_GPIO_WritePin(motor_list[i].GPIOx,motor_list[i].GPIO_Pin,GPIO_PIN_SET);
			}
		}
		for(int i=0; i<MOTOR_NUMBER*2; i++)
		{
			if(motorCount == motor_pulse[i])
				HAL_GPIO_WritePin(motor_list[i].GPIOx,motor_list[i].GPIO_Pin,GPIO_PIN_RESET);
		}
	}
}
