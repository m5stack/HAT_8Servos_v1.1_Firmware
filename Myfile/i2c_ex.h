#ifndef __i2c_ex_H
#define __i2c_ex_H
#ifdef __cplusplus
  extern "C" {
#endif


#include "main.h"

extern void i2c1_set_send_data(uint8_t *tx_ptr, uint16_t len);
extern void I2CAddReg(uint8_t reg, uint8_t* buff, uint8_t len, uint8_t bit);
extern void I2CInit(void);
extern uint8_t I2CGetTxState(void);

#ifdef __cplusplus
}
#endif
#endif
