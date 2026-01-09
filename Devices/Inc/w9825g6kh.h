/**
 * @note 需要先在CubeMX里配置好FMC,再选择不调用对应的函数(为了省去配置GPIO的步骤)
 */


#ifndef __W9825G6KH_H
#define __W9825G6KH_H
#include "stdlib.h"
#include "stdint.h"
#include "fmc.h"
#include "stm32h743xx.h"
#include "stm32h7xx_ll_fmc.h"
#include "stm32h7xx_hal_sdram.h"

typedef struct
{
    SDRAM_HandleTypeDef *sdram;
    FMC_SDRAM_CommandTypeDef *command;
} W9825G6KH_Instance;

W9825G6KH_Instance *W9825G6KH_Register(void);
void W9825G6KH_Init(W9825G6KH_Instance *instance);

#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000) 
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200) 

#endif