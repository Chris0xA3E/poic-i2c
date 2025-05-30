/*
 * i2c.c
 *
 *  Created on: Nov 9, 2024
 *      Author: master
 */


#include "i2c.h"
#include "core_riscv.h"
#include "PIOC_SFR.h"

#define     IIC_SFR_ADDR1  ((uint8_t *)&(PIOC->D8_DATA_REG8))
#define     IIC_SFR_ADDR2  ((uint8_t *)&(PIOC->D8_DATA_REG16))
#define     IIC_SFR_ADDR3  ((uint8_t *)&(PIOC->D8_DATA_REG24))
#define   R16_DATA_REG4_5  (*((volatile unsigned short *)(PIOC_SFR_BASE+0x24))) // RW/RW, data buffer 4~5
#define     PIOC_TIM_PSC2      ((uint8_t)0x07)
#define     PIOC_TIM_PSC4      ((uint8_t)0x06)
#define     PIOC_TIM_PSC8      ((uint8_t)0x05)
#define     PIOC_TIM_PSC16     ((uint8_t)0x04)
#define     PIOC_TIM_PSC32     ((uint8_t)0x03)
#define     PIOC_TIM_PSC128    ((uint8_t)0x02)
#define     PIOC_TIM_PSC512    ((uint8_t)0x01)
#define     PIOC_TIM_PSC2048   ((uint8_t)0x00)

uint8_t  PIOC_IIC_FLAG=0;
uint16_t PIOC_IIC_RemainLEN0=0;
uint16_t PIOC_IIC_RemainLEN1=0;
uint16_t PIOC_IIC_RemainLEN2=0;
uint16_t PIOC_IIC_RemainLEN3=0;
uint8_t  *PIOC_IIC_BUF_ADDR0 = NULL;
uint8_t  *PIOC_IIC_BUF_ADDR1 = NULL;
uint8_t  *PIOC_IIC_BUF_ADDR2 = NULL;
uint8_t  *PIOC_IIC_BUF_ADDR3 = NULL;



__attribute__((aligned(16)))  const unsigned char PIOC_CODE[] =
{0x00,0x00,0xE2,0x61,0xFF,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,   /* ...a............ */
 0x00,0x00,0x30,0x00,0x03,0x70,0x00,0x00,0x03,0x70,0x00,0x00,0x03,0x70,0x00,0x00,   /* ..0..p...p...p.. */
 0x03,0x70,0x00,0x00,0xFF,0x2C,0x00,0x00,0x0A,0x30,0x30,0x00,0x21,0x02,0x07,0x10,   /* .p...,...00.!... */
 0x20,0x02,0x06,0x10,0x30,0x00,0x14,0x00,0x1E,0x02,0x0B,0x48,0x0B,0x49,0x0F,0x23,   /* ....0......H.I.# */
 0x03,0x41,0x03,0x43,0x26,0x51,0x03,0x4B,0x08,0x28,0x27,0x10,0x28,0x24,0x16,0x70,   /* .A.C&Q.K.('.($.p */
 0x05,0x28,0x0A,0x70,0x0B,0x41,0x05,0x28,0x0A,0x70,0x21,0x02,0x05,0x10,0x06,0x4D,   /* .(.p.A.(.p!....M */
 0x06,0x4C,0x23,0x02,0x1F,0x10,0x17,0x00,0xBF,0x00,0x16,0x00,0x17,0x00,0xBE,0x00,   /* .L#............. */
 0x16,0x00,0x17,0x00,0xBD,0x00,0x16,0x00,0x17,0x00,0xBC,0x00,0x16,0x00,0x17,0x00,   /* ................ */
 0xBB,0x00,0x16,0x00,0x17,0x00,0xBA,0x00,0x16,0x00,0x17,0x00,0xB9,0x00,0x16,0x00,   /* ................ */
 0x17,0x00,0x03,0x53,0x1F,0x40,0xB8,0x00,0x16,0x00,0x17,0x00,0x0A,0x41,0x16,0x00,   /* ...S.@.......A.. */
 0x1F,0x00,0xFD,0x3C,0x03,0x5B,0x87,0x60,0x17,0x00,0x0B,0x41,0x0A,0x49,0x22,0x02,   /* ...<.[.`...A.I". */
 0x1F,0x10,0xBF,0x00,0x16,0x00,0x17,0x00,0xBE,0x00,0x16,0x00,0x17,0x00,0xBD,0x00,   /* ................ */
 0x16,0x00,0x17,0x00,0xBC,0x00,0x16,0x00,0x17,0x00,0xBB,0x00,0x16,0x00,0x17,0x00,   /* ................ */
 0xBA,0x00,0x16,0x00,0x17,0x00,0xB9,0x00,0x16,0x00,0x17,0x00,0xB8,0x00,0x16,0x00,   /* ................ */
 0x17,0x00,0x0A,0x41,0x16,0x00,0x1F,0x00,0xFD,0x3C,0x23,0x58,0xCA,0x60,0x17,0x00,   /* ...A.....<#X.`.. */
 0x0B,0x49,0x0A,0x49,0x0B,0x40,0x06,0x45,0x06,0x44,0x01,0x28,0x0A,0x70,0x06,0x4D,   /* .I.I.@.E.D.(.p.M */
 0x06,0x4C,0x0B,0x48,0x16,0x00,0x06,0x45,0x06,0x44,0x03,0x43,0x28,0x60,0x1F,0x58,   /* .L.H...E.D.C(`.X */
 0xCA,0x60,0x17,0x00,0x0B,0x40,0x06,0x45,0x06,0x44,0x0B,0x28,0x0A,0x70,0x06,0x4D,   /* .`...@.E.D.(.p.M */
 0x06,0x4C,0x0B,0x48,0x17,0x00,0x0A,0x41,0x16,0x00,0xFF,0x00,0x17,0x00,0x16,0x00,   /* .L.H...A........ */
 0xFE,0x00,0x17,0x00,0x16,0x00,0xFD,0x00,0x17,0x00,0x16,0x00,0xFC,0x00,0x17,0x00,   /* ................ */
 0x16,0x00,0xFB,0x00,0x17,0x00,0x16,0x00,0xFA,0x00,0x17,0x00,0x16,0x00,0xF9,0x00,   /* ................ */
 0x17,0x00,0x24,0x15,0x24,0x04,0x03,0x52,0x25,0x15,0x16,0x00,0xF8,0x00,0x24,0x02,   /* ..$.$..R%.....$. */
 0x25,0x0A,0xC2,0x34,0x17,0x00,0x0B,0x41,0x0A,0x49,0x1F,0x02,0x01,0x10,0x16,0x00,   /* %..4...A.I...... */
 0x27,0x15,0x92,0x30,0x03,0x51,0x28,0x24,0x02,0x28,0x03,0x1B,0x08,0x28,0x27,0x10,   /* '..0.Q($.(...('. */
 0x1C,0x4F,0x92,0x60,0x17,0x00,0x0B,0x49,0x0A,0x49,0x1F,0x02,0x01,0x10,0x16,0x00,   /* .O.`...I.I...... */
 0x1C,0x4F,0xFD,0x60,0x17,0x00,0x0B,0x41,0x0A,0x49,0x01,0x02,0x1F,0x10,0xBF,0x00,   /* .O.`...A.I...... */
 0x16,0x00,0x17,0x00,0x03,0x70,0xBE,0x00,0x16,0x00,0x17,0x00,0xBD,0x00,0x16,0x00,   /* .....p.......... */
 0x17,0x00,0xBC,0x00,0x16,0x00,0x17,0x00,0xBB,0x00,0x16,0x00,0x17,0x00,0xBA,0x00,   /* ................ */
 0x16,0x00,0x17,0x00,0xB9,0x00,0x16,0x00,0x27,0x15,0xED,0x30,0x03,0x51,0x28,0x24,   /* ........'..0.Q($ */
 0x02,0x28,0x03,0x1B,0x08,0x28,0x27,0x10,0x1C,0x4F,0x17,0x00,0xB8,0x00,0x16,0x00,   /* .(...('..O...... */
 0x24,0x15,0x24,0x04,0x03,0x52,0x25,0x15,0x17,0x00,0x0A,0x41,0x16,0x00,0x1F,0x00,   /* $.$..R%....A.... */
 0xFD,0x3C,0x24,0x02,0x25,0x0A,0xFD,0x34,0xCA,0x60,0x17,0x00,0x0B,0x41,0x0A,0x49,   /* .<$.%..4.`...A.I */
 0x16,0x00,0x06,0x45,0x06,0x44,0x01,0x28,0x0A,0x70,0x0B,0x49,0x1B,0x60,0x03,0x41,   /* ...E.D.(.p.I.`.A */
 0x03,0x43,0x28,0x24,0x0B,0x48,0x0B,0x49,0x0C,0x23,0x16,0x00,0x1F,0x00,0x0D,0x39,   /* .C($.H.I.#.....9 */
 0x15,0x00,0x03,0x70,0x1E,0x00,0x0D,0x39,0x08,0x28,0x27,0x10,0x23,0x40,0x17,0x00,   /* ...p...9.('.#@.. */
 0x16,0x00,0xFF,0x00,0x17,0x00,0x16,0x00,0xFE,0x00,0x17,0x00,0x16,0x00,0xFD,0x00,   /* ................ */
 0x17,0x00,0x16,0x00,0xFC,0x00,0x17,0x00,0x16,0x00,0xFB,0x00,0x17,0x00,0x16,0x00,   /* ................ */
 0xFA,0x00,0x17,0x00,0x16,0x00,0xF9,0x00,0x1F,0x40,0x1F,0x02,0x23,0x0B,0x07,0x31,   /* .........@..#..1 */
 0x17,0x00,0x16,0x00,0xF8,0x00,0x03,0x53,0x73,0x61,0x1F,0x58,0x3D,0x61,0x26,0x51,   /* .......Ssa.X=a&Q */
 0x07,0x61,0x17,0x00,0x0B,0x41,0x0A,0x49,0xAE,0x61,0x26,0x51,0x43,0x61,0x17,0x00,   /* .a...A.I.a&QCa.. */
 0x0B,0x41,0x0A,0x49,0x79,0x61,0x17,0x00,0x0B,0x41,0x0A,0x49,0x16,0x00,0x17,0x00,   /* .A.Iya...A.I.... */
 0x0A,0x41,0x16,0x00,0xFF,0x00,0x17,0x00,0x16,0x00,0xFE,0x00,0x17,0x00,0x16,0x00,   /* .A.............. */
 0xFD,0x00,0x17,0x00,0x16,0x00,0xFC,0x00,0x17,0x00,0x16,0x00,0xFB,0x00,0x17,0x00,   /* ................ */
 0x16,0x00,0xFA,0x00,0x17,0x00,0x16,0x00,0xF9,0x00,0x17,0x00,0x16,0x00,0xF8,0x00,   /* ................ */
 0x1F,0x02,0x22,0x0B,0x07,0x31,0x17,0x00,0x0B,0x41,0x0A,0x49,0x16,0x00,0x17,0x00,   /* .."..1...A.I.... */
 0x0A,0x41,0x16,0x00,0xFF,0x00,0x1F,0x00,0x89,0x39,0x0B,0x5F,0x6D,0x61,0x1E,0x00,   /* .A.......9._ma.. */
 0x8B,0x39,0x03,0x4B,0x17,0x61,0x1F,0x58,0x07,0x61,0x17,0x00,0x0B,0x41,0x0A,0x49,   /* .9.K.a.X.a...A.I */
 0xAE,0x61,0x23,0x48,0x16,0x00,0x17,0x00,0x0A,0x41,0x16,0x00,0xFF,0x00,0x1F,0x00,   /* .a#H.....A...... */
 0x8A,0x3D,0x03,0x70,0x0B,0x57,0x82,0x61,0x1E,0x00,0x8B,0x39,0x26,0x4A,0x1C,0x4F,   /* .=.p.W.a...9&J.O */
 0x07,0x61,0x23,0x48,0x17,0x00,0x16,0x00,0xFE,0x00,0x17,0x00,0x16,0x00,0xFD,0x00,   /* .a#H............ */
 0x17,0x00,0x16,0x00,0xFC,0x00,0x17,0x00,0x16,0x00,0xFB,0x00,0x17,0x00,0x16,0x00,   /* ................ */
 0xFA,0x00,0x17,0x00,0x16,0x00,0xF9,0x00,0x17,0x00,0x16,0x00,0xF8,0x00,0x1F,0x02,   /* ................ */
 0x01,0x10,0x17,0x00,0x0B,0x41,0x0A,0x49,0x27,0x15,0x79,0x31,0x03,0x51,0x28,0x24,   /* .....A.I'.y1.Q($ */
 0x02,0x28,0x03,0x1B,0x08,0x28,0x27,0x10,0x1C,0x4F,0x79,0x61,0x23,0x40,0x1C,0x4F,   /* .(...('..Oya#@.O */
 0x38,0x24,0x03,0x49,0x16,0x00,0x17,0x00,0x04,0x00,0x01,0x0A,0x1F,0x10,0x01,0x01,   /* 8$.I............ */
 0x01,0x02,0xBF,0x00,0x0A,0x49,0x16,0x00,0x17,0x00,0xBE,0x00,0x16,0x00,0x17,0x00,   /* .....I.......... */
 0xBD,0x00,0x16,0x00,0x17,0x00,0xBC,0x00,0x16,0x00,0x17,0x00,0xBB,0x00,0x16,0x00,   /* ................ */
 0x17,0x00,0xBA,0x00,0x16,0x00,0x17,0x00,0xB9,0x00,0x16,0x00,0x27,0x15,0xD7,0x31,   /* ............'..1 */
 0x03,0x51,0x28,0x24,0x02,0x28,0x03,0x1B,0x08,0x28,0x27,0x10,0x1C,0x4F,0x17,0x00,   /* .Q($.(...('..O.. */
 0xB8,0x00,0x16,0x00,0x17,0x00,0x0A,0x41,0x16,0x00,0x1F,0x00,0xB2,0x39,0x26,0x4A,   /* .......A.....9&J */
 0x1C,0x4F,0x07,0x61,0x00,0x00,0x00,0x00,0x14,0x00,0x26,0x58,0x07,0x61,0x1B,0x60,   /* .O.a......&X.a.` */
 0xE2,0x61,0xE9,0x61};  /* .a.a */


/*********************************************************************
 * @fn      PIOC_INIT
 *
 * @brief   Initializes PIOC
 *
 * @return  none
 */



void PIOC_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_18|GPIO_Pin_19;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    NVIC_EnableIRQ( PIOC_IRQn );                                        //enable PIOC interrupt
    NVIC_SetPriority(PIOC_IRQn,0xf0);

    memcpy((uint8_t *)(PIOC_SRAM_BASE),PIOC_CODE,sizeof(PIOC_CODE));    // load code for PIOC
    R8_SYS_CFG |= RB_MST_RESET;                                         // reset PIOC
    R8_SYS_CFG = RB_MST_IO_EN1 | RB_MST_IO_EN0;                         // enable IO0&IO1
    R8_SYS_CFG |= RB_MST_CLK_GATE;                                      // open PIOC clock
}


/*********************************************************************
 * @fn      PIOC_IRQHandler
 *
 * @brief   This function handles PIOC exception.
 *
 * @return  none
 */
void PIOC_IRQHandler( void )
{
    uint8_t *addr_target = NULL;
    uint8_t *addr_data = NULL;
    uint8_t data_len = 0;
    if((R8_SYS_CFG&RB_INT_REQ)!=RESET)
    {
        //Data read mode
        if((R8_DATA_REG3 & 0X01)!=RESET)
        {
            addr_target = PIOC_IIC_BUF_ADDR0;
            if(PIOC_IIC_FLAG==1)
            {
                addr_data = IIC_SFR_ADDR1;
                PIOC_IIC_FLAG=2;
            }
            else if(PIOC_IIC_FLAG==2)
            {
                addr_data = IIC_SFR_ADDR2;
                PIOC_IIC_FLAG=1;
            }
            if(PIOC_IIC_RemainLEN0>8)
            {
                if((R8_DATA_REG6 & 0x04)!=RESET)
                    data_len = 8-R8_DATA_REG7;
                else
                    data_len = 8;
                PIOC_IIC_BUF_ADDR0 += 8;
                PIOC_IIC_RemainLEN0 -= 8;
            }
            else if(PIOC_IIC_RemainLEN0>0)
            {
                if((R8_DATA_REG6 & 0x04)!=RESET)
                    data_len = 8-R8_DATA_REG7;
                else
                    data_len = PIOC_IIC_RemainLEN0;
                PIOC_IIC_RemainLEN0 = 0;
                PIOC_IIC_FLAG = 0;
                PIOC_IIC_BUF_ADDR0 = NULL;
            }

        }
        //Data write Mode
        else
        {
            addr_data = PIOC_IIC_BUF_ADDR1;
            if(PIOC_IIC_FLAG==1)
            {
                addr_target = IIC_SFR_ADDR1;
                PIOC_IIC_FLAG=2;
            }
            else if(PIOC_IIC_FLAG==2)
            {
                addr_target = IIC_SFR_ADDR2;
                PIOC_IIC_FLAG=1;
            }
            if(PIOC_IIC_RemainLEN1>8)
            {
                data_len = 8;
                PIOC_IIC_BUF_ADDR1 += 8;
                PIOC_IIC_RemainLEN1 -= 8;
            }
            else if(PIOC_IIC_RemainLEN1>0)
            {
                data_len = PIOC_IIC_RemainLEN1;
                PIOC_IIC_RemainLEN1 = 0;
                PIOC_IIC_FLAG = 0;
                PIOC_IIC_BUF_ADDR1 = NULL;
            }
        }
        memcpy( addr_target, addr_data, data_len );
        //Slave reception completed or transmission completed,
        if((R8_DATA_REG6 & 0x04)!=RESET)
        {
            R8_DATA_REG6 &= ~(0X04);
            //Rewrite the data to be sent and received
            PIOC_IIC_SLAVE(PIOC_IIC_BUF_ADDR3,PIOC_IIC_RemainLEN3,PIOC_IIC_BUF_ADDR2,PIOC_IIC_RemainLEN2);
        }
        R8_CTRL_RD=11;    // False write any value to R8_CTRL_RD, clear interrupt flag
    }

}

/*********************************************************************
 * @fn      PIOC_IIC_INIT
 *
 * @brief   Initializes PIOC_IIC,
 *
 * @param   arr - 0~255.
 *          psc - PIOC_TIM_PSC2
 *                PIOC_TIM_PSC4
 *                PIOC_TIM_PSC8
 *                PIOC_TIM_PSC16
 *                PIOC_TIM_PSC32
 *                PIOC_TIM_PSC128
 *                PIOC_TIM_PSC512
 *                PIOC_TIM_PSC2048
 *          address - In 7-bit address mode, bits 1-7 are valid addresses
 *                    In 10 bit address mode, bits 0-9 are valid addresses
 *
 * @return  none
 */
void PIOC_IIC_INIT(u8 arr, u8 psc, u16 address)
{
    uint32_t tmp=0;
    tmp |= (uint32_t)(((0XFF-arr)<<8)|psc);
    if(address>0XFF)
    {
        tmp |= (uint32_t)(((0xf0 << 8) | ((address & 0x0300)<<1) | (address & 0xff))<<16);
        R8_DATA_REG6 |= (1<<1);
    }
    else
    {
        tmp |= (uint32_t)((address & 0xfe)<<24);
        R8_DATA_REG6 &= ~(1<<1);
    }
    R32_DATA_REG0_3 = tmp;
}

/*********************************************************************
 * @fn      PIOC_IIC_SEND
 *
 * @brief   Host send data.
 *
 * @param   p_source_addr - data.
 *          total_bytes - total data number(byte).
 *
 * @return  none
 */
void PIOC_IIC_SEND(uint8_t *p_source_addr,uint16_t total_bytes)
{
    if(total_bytes<=16)
    {
        memcpy( IIC_SFR_ADDR1, p_source_addr, total_bytes );
        PIOC_IIC_FLAG = 0;
        PIOC_IIC_RemainLEN1 = 0;
    }
    else
    {
        memcpy( IIC_SFR_ADDR1, p_source_addr, 16 );
        PIOC_IIC_FLAG = 1;
        PIOC_IIC_RemainLEN1 = total_bytes-16;
        PIOC_IIC_BUF_ADDR1 = p_source_addr+16;
    }
    R16_DATA_REG4_5 = total_bytes;
    R8_DATA_REG6 |= 0x01;
    R8_DATA_REG3 &= ~(0x01);        //write
    R8_CTRL_WR = 0X33;              // To R8_CTRL_WR write any value,then start
}

/*********************************************************************
 * @fn      PIOC_IIC_Receive
 *
 * @brief   Host receive data
 *
 * @param   p_source_addr - data.
 *          total_bytes - total data number(byte).
 *
 * @return  none
 */
void PIOC_IIC_Receive(uint8_t *p_source_addr,uint16_t total_bytes)
{
    PIOC_IIC_BUF_ADDR0 = p_source_addr;
    PIOC_IIC_RemainLEN0 = total_bytes;
    PIOC_IIC_FLAG = 1;
    R16_DATA_REG4_5 = total_bytes;
    R8_DATA_REG6 |= 0x01;
    R8_DATA_REG3 |= 0x01;           //read
    R8_CTRL_WR = 0X33;              // To R8_CTRL_WR write any value,then start
}

/*********************************************************************
 * @fn      PIOC_IIC_SLAVE
 *
 * @brief   slave mode
 *
 * @param   p_send_addr - send data.
 *          send_len - total data number(byte).
 *          p_receive_addr - receive data.
 *          receive_len - receive maximum(byte).
 *
 * @return  none
 */
void PIOC_IIC_SLAVE(uint8_t *p_send_addr,uint16_t send_len,uint8_t *p_receive_addr,uint16_t receive_len)
{
    PIOC_IIC_BUF_ADDR0 = PIOC_IIC_BUF_ADDR2 = p_receive_addr;
    PIOC_IIC_RemainLEN0 = PIOC_IIC_RemainLEN2 = receive_len;
    PIOC_IIC_BUF_ADDR3 = p_send_addr;
    PIOC_IIC_RemainLEN3 = send_len;
    if(send_len<=8)
    {
        memcpy( IIC_SFR_ADDR3, p_send_addr, send_len );
        PIOC_IIC_FLAG = 0;
        PIOC_IIC_RemainLEN1 = 0;
    }
    else
    {
        memcpy( IIC_SFR_ADDR3, p_send_addr, 8 );
        PIOC_IIC_FLAG = 1;
        PIOC_IIC_RemainLEN1 = send_len-8;
        PIOC_IIC_BUF_ADDR1 = p_send_addr+8;
    }
    R8_DATA_REG6 &= ~(0x01);
    R8_CTRL_WR = 0X33;
}
