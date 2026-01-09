#include "w9825g6kh.h"

W9825G6KH_Instance *W9825G6KH_Register(void)
{
    SDRAM_HandleTypeDef *sdram = (SDRAM_HandleTypeDef *)malloc(sizeof(SDRAM_HandleTypeDef));
    if (sdram == NULL)
        return NULL;

    FMC_SDRAM_TimingTypeDef SdramTiming = {0};

    /* hsdram1.Init */
    sdram->Instance = FMC_SDRAM_DEVICE;
    sdram->Init.SDBank = FMC_SDRAM_BANK1;                             // 选择BANK区
    sdram->Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9;       // 行地址宽度
    sdram->Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_13;            // 列地址线宽度
    sdram->Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;         // 数据宽度
    sdram->Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;    // bank数量
    sdram->Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;                 // CAS
    sdram->Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE; // 禁止写保护
    sdram->Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;             // 分频
    sdram->Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;                  // 突发模式
    sdram->Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_1;              // 读延迟

    /* SdramTiming */
    SdramTiming.LoadToActiveDelay = 2;
    SdramTiming.ExitSelfRefreshDelay = 7;
    SdramTiming.SelfRefreshTime = 4;
    SdramTiming.RowCycleDelay = 7;
    SdramTiming.WriteRecoveryTime = 2;
    SdramTiming.RPDelay = 2;
    SdramTiming.RCDDelay = 2;

    if (HAL_SDRAM_Init(sdram, &SdramTiming) != HAL_OK)
    {
        free(sdram);
        return NULL;
    }

    W9825G6KH_Instance *instance = (W9825G6KH_Instance *)malloc(sizeof(W9825G6KH_Instance));
    if (instance == NULL)
    {
        free(sdram);
        return NULL;
    }
    instance->sdram = sdram;

    return instance;
}

void W9825G6KH_Init(W9825G6KH_Instance *instance)
{
    __IO uint32_t tmpmrd = 0;

    /* Configure a clock configuration enable command */
    instance->command->CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;     // 开启SDRAM时钟
    instance->command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1; // 选择要控制的区域
    instance->command->AutoRefreshNumber = 1;
    instance->command->ModeRegisterDefinition = 0;

    HAL_SDRAM_SendCommand(instance->sdram, instance->command, 0x1000); // 发送控制指令
    HAL_Delay(1);                                                      // 延时等待

    /* Configure a PALL (precharge all) command */
    instance->command->CommandMode = FMC_SDRAM_CMD_PALL;           // 预充电命令
    instance->command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1; // 选择要控制的区域
    instance->command->AutoRefreshNumber = 1;
    instance->command->ModeRegisterDefinition = 0;
    HAL_SDRAM_SendCommand(instance->sdram, instance->command, 0x1000); // 发送控制指令
    HAL_Delay(1);                                                      // 延时等待

    /* Configure a Auto-Refresh command */
    instance->command->CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE; // 使用自动刷新
    instance->command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;   // 选择要控制的区域
    instance->command->AutoRefreshNumber = 8;                        // 自动刷新次数
    instance->command->ModeRegisterDefinition = 0;
    HAL_SDRAM_SendCommand(instance->sdram, instance->command, 0x1000); // 发送控制指令
    HAL_Delay(1);                                                      // 延时等待

    /* Program the external memory mode register */
    tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_2 |
             SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL |
             SDRAM_MODEREG_CAS_LATENCY_3 |
             SDRAM_MODEREG_OPERATING_MODE_STANDARD |
             SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

    instance->command->CommandMode = FMC_SDRAM_CMD_LOAD_MODE;      // 加载模式寄存器命令
    instance->command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1; // 选择要控制的区域
    instance->command->AutoRefreshNumber = 1;
    instance->command->ModeRegisterDefinition = tmpmrd;
    HAL_SDRAM_SendCommand(instance->sdram, instance->command, 0x1000); // 发送控制指令
    HAL_Delay(1);                                                      // 延时等待

    HAL_SDRAM_ProgramRefreshRate(instance->sdram, 918); // 配置刷新率
}
