#include "portb.h"
#include "adc.h"
#include "gpio.h"
#include "i2c_ex.h"

volatile uint32_t adc_value_buff[240];

volatile uint16_t adc_value[3];
volatile uint8_t digital_value[4];

uint8_t digital_value_last[2];

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) 
{
  uint64_t adcTotal1 = 0;
  uint64_t adcTotal2 = 0;
  uint64_t adcTotal3 = 0;
  
	for (uint8_t i = 0; i < 80; i++) 
  {
    adcTotal1 += adc_value_buff[i * 3];
    adcTotal2 += adc_value_buff[i * 3 + 1];
    adcTotal3 += adc_value_buff[i * 3 + 2];
  }
	
	adc_value[0] = adcTotal1 / 80;
	adc_value[1] = adcTotal2 / 80;
	adc_value[2] = adcTotal3 / 80;

  digital_value[0] = (adc_value[0] > 1000) ? 1 : 0;
  digital_value[1] = (adc_value[1] > 1000) ? 1 : 0;
  digital_value[2] = (adc_value[2] > 1000) ? 1 : 0;
}

void AdcInit(void)
{
  I2CAddReg(0x40, (uint8_t *)adc_value, 6, 16);
  I2CAddReg(0x50, (uint8_t *)digital_value, 4, 8);
  HAL_ADC_Start_DMA(&hadc, (uint32_t *)adc_value_buff, 240);
}

void PortBUpdate(void)
{
  if(digital_value[0] != digital_value_last[0])
  {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, (GPIO_PinState)digital_value[0]);
    digital_value_last[0] = digital_value[0];
  }
  
  if(digital_value[1] != digital_value_last[1])
  {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, (GPIO_PinState)digital_value[1]);
    digital_value_last[1] = digital_value[1];
  }
}
