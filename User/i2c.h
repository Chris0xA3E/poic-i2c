/*
 * i2c.h
 *
 *  Created on: Nov 9, 2024
 *      Author: master
 */

#ifndef USER_I2C_H_
#define USER_I2C_H_

#include "debug.h"

void PIOC_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void PIOC_IIC_SLAVE(uint8_t *p_send_addr,uint16_t send_len,uint8_t *p_receive_addr,uint16_t receive_len);

/* I2C Mode Definition */
#define HOST_MODE     0
#define SLAVE_MODE    1

/* I2C Communication Mode Selection */
#define I2C_MODE      HOST_MODE
//#define I2C_MODE   SLAVE_MODE

/* Global define */

void PIOC_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void PIOC_IIC_SLAVE(uint8_t *p_send_addr,uint16_t send_len,uint8_t *p_receive_addr,uint16_t receive_len);
void PIOC_IIC_Receive(uint8_t *p_source_addr,uint16_t total_bytes);
void PIOC_IIC_SEND(uint8_t *p_source_addr,uint16_t total_bytes);
void PIOC_IIC_INIT(u8 arr, u8 psc, u16 address);
void PIOC_INIT(void);





#endif /* USER_I2C_H_ */
