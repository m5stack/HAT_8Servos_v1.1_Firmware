#include "Steer.h"
#include "tim.h"
#include "i2c_ex.h"


#define ANGLE_TO_PULSE(angle) ((uint16_t)(50 + (angle * 200 / 180)))//Angle convert to pulse
#define PULSE_TO_COUNT(pulse) ((uint8_t)(pulse)) // Pulse convert to percent
#define PULSE_TO_ANGLE(pulse) ((uint8_t)((pulse - 50) * 180 / 200)) // Pulse convert to angle

uint8_t steer_tim_count[STEER_NUMBER];//折算过后的占空比
uint16_t steer_pulse[STEER_NUMBER];//原始占空量
uint16_t i2c_steer_pulse[STEER_NUMBER];//原始占空量
uint8_t steer_angle[STEER_NUMBER];//折算后的角度
uint8_t steer_disable;
uint16_t steer_count = 0;//1us count

uint8_t steer_angle_last[STEER_NUMBER];//记录之前的角度
uint16_t steer_pulse_last[STEER_NUMBER];//记录之前的占空比
uint8_t steer_disable_last=0;


typedef struct {
  GPIO_TypeDef* GPIOx;
  uint16_t GPIO_Pin;
} PWM_GPIO;//模拟PWM接口

PWM_GPIO steering_gear[STEER_NUMBER]={
	
	{Steer_0_GPIO_Port,Steer_0_Pin},	
	{Steer_1_GPIO_Port,Steer_1_Pin},
	{Steer_2_GPIO_Port,Steer_2_Pin},
	{Steer_3_GPIO_Port,Steer_3_Pin},
	{Steer_4_GPIO_Port,Steer_4_Pin},
	{Steer_5_GPIO_Port,Steer_5_Pin},
	{Steer_6_GPIO_Port,Steer_6_Pin},
	{Steer_7_GPIO_Port,Steer_7_Pin},
		
};//四个模拟PWM接口
/**
  * @brief  Update duty cycle.
  * @param  Which output.
  * @param  Set duty cycle.
  */
static void UpdateTimPulse(uint8_t pos, uint16_t pulse)
{
  steer_pulse[pos] = pulse;
}
/**
  * @brief  Use the GPIO port output to simulate PWM.
  * @param  PWM port.
  */
void Analog_PWM(void)
{
	static char Switch[STEER_NUMBER]={0};
	for(int i=0; i<STEER_NUMBER; i++)
	{
		if(steer_count <= steer_pulse[i] && Switch[i]==0)
		{
			HAL_GPIO_WritePin(steering_gear[i].GPIOx,steering_gear[i].GPIO_Pin,GPIO_PIN_SET);
			Switch[i]=1;
		}
		else if(steer_count > steer_pulse[i] && Switch[i]==1)
		{
			HAL_GPIO_WritePin(steering_gear[i].GPIOx,steering_gear[i].GPIO_Pin,GPIO_PIN_RESET);
			Switch[i]=0;
		}
	}
}
/**
  * @brief  Steer initialization
  */
void SteerInit(void)
{
  for(uint8_t i = 0; i < STEER_NUMBER; i++)
  {
		steer_angle[i] = 0xff;
    steer_angle_last[i] = 0xff;
		steer_pulse[i] = 0xffff;
		steer_pulse_last[i] = 0xffff;
		steer_disable = 0;
  }
  I2CAddReg(0x00, (uint8_t *)steer_angle, STEER_NUMBER, 8);
  I2CAddReg(0x10, (uint8_t *)i2c_steer_pulse, STEER_NUMBER > 8 ? 16 : STEER_NUMBER * 2, 16);
	I2CAddReg(0x30, (uint8_t *)&steer_disable, 1, 8);
}
/**
  * @brief  Set Steer angle.
  * @param  Which output.
  */
void SetSteerAngle(uint8_t pos, uint8_t angle) 
{
  if(angle > 180) return;
  steer_angle[pos] = angle;
  steer_pulse[pos] = ANGLE_TO_PULSE(angle);
//  steer_tim_count[pos] = PULSE_TO_COUNT(steer_pulse[pos]);
  UpdateTimPulse(pos, steer_pulse[pos]);
}
/**
  * @brief  Set Steer Pulse.
  * @param  Which output.
  */
void SetSteerPulse(uint8_t pos, uint16_t pulse)
{
  if(pulse > 250) return;
  if(pulse < 50) return;
  steer_pulse[pos] = pulse;
  steer_angle[pos] = PULSE_TO_ANGLE(pulse);
//  steer_tim_count[pos] = PULSE_TO_COUNT(pulse);
  UpdateTimPulse(pos, steer_pulse[pos]);  
}
/**
  * @brief  Update related parameters.
  */
void SteerUpdate(void)
{
  for(uint8_t i = 0; i < STEER_NUMBER; i++) {
		steer_pulse[i] = (uint8_t)(i2c_steer_pulse[i]/10);
    if(steer_angle[i] != steer_angle_last[i]) {
      SetSteerAngle(i, steer_angle[i]);
      steer_angle_last[i] = steer_angle[i];
      steer_pulse_last[i] = steer_pulse[i];
    }

    if(steer_pulse[i] != steer_pulse_last[i]) {
      SetSteerPulse(i, steer_pulse[i]);
      steer_angle_last[i] = steer_angle[i];
      steer_pulse_last[i] = steer_pulse[i];
    }
		i2c_steer_pulse[i] = (steer_pulse[i]*10);
  }
	if (steer_disable != steer_disable_last)
	{
		if(steer_disable)
		{
			HAL_GPIO_WritePin(MosFET_GPIO_Port,MosFET_Pin,GPIO_PIN_SET);
		}
		else 
		{
			HAL_GPIO_WritePin(MosFET_GPIO_Port,MosFET_Pin,GPIO_PIN_RESET);
		}
		steer_disable_last = steer_disable;
	}
}
/**
  * @brief  Rewrite the callback function.
  * @param  Callback function parameters.
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==htim16.Instance)
	{
		if(++steer_count==2000)steer_count=0;
		Analog_PWM();
	}
}
