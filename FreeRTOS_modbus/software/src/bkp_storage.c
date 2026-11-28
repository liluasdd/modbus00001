#include "bkp_storage.h"
#include "usertype.h"

RTC_HandleTypeDef hrtc;


/**
 * @brief  解锁备份域写保护 (必须在 main 初始化阶段调用一次)
 */
void BKP_Unlock(void)
{
    // 1. 使能 PWR 和 BKP 外设时钟
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_RCC_BKP_CLK_ENABLE();

    // 2. 允许访问备份域 (设置 PWR_CR 寄存器的 DBP 位)
    HAL_PWR_EnableBkUpAccess();
}

/**
 * @brief  将参数保存到 BKP 寄存器
 * @param  params: 要保存的参数结构体指针
 */
void BKP_SaveParams(BkpParams_t *params)
{
    uint16_t *pData = (uint16_t *)params;
    uint32_t bkpBaseAddr = BKP_BASE; // BKP 基地址

    // 确保魔数正确
    params->magic_number = BKP_MAGIC_NUM;

    // 将结构体数据按 16 位 (半字) 依次写入 BKP_DR1 ~ BKP_DR10
    // 结构体大小 14 字节，需要写入 7 个 16 位寄存器
    for (uint8_t i = 0; i < sizeof(BkpParams_t) / 2; i++)
    {
        // 计算目标寄存器地址并写入
        *(__IO uint16_t *)(bkpBaseAddr + (i * 2) + BKP_DR1_OFFSET) = pData[i];
    }
}

/**
 * @brief  从 BKP 寄存器读取参数
 * @param  params: 存放读取结果的参数结构体指针
 * @retval 无
 */
void BKP_LoadParams(BkpParams_t *params)
{
    uint16_t *pData = (uint16_t *)params;
    uint32_t bkpBaseAddr = BKP_BASE;

    // 从 BKP_DR1 ~ BKP_DR10 依次读取数据到结构体
    for (uint8_t i = 0; i < sizeof(BkpParams_t) / 2; i++)
    {
        pData[i] = *(__IO uint16_t *)(bkpBaseAddr + (i * 2) + BKP_DR1_OFFSET);
    }
}

/* 写入 16 位标志位集合 */
void BKP_Save_Flags(uint16_t flags)
{
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, (uint32_t)flags);
}

/* 写入 32 位输出计数 (需拆分) */
void BKP_Save_OutputCount(uint32_t count)
{
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, (uint32_t)(count & 0xFFFF));         // 低16位
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, (uint32_t)((count >> 16) & 0xFFFF)); // 高16位
}


/* 写入 16 位电池电压 */
void BKP_Save_BatVoltage(uint16_t voltage)
{
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR6, (uint32_t)voltage);
}

/* 写入 8 位电量 (占用 DR7 的低 8 位) */
void BKP_Save_BatLevel(uint8_t level)
{
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR7, (uint32_t)level);
}

/* 读取 16 位标志位集合 */
uint16_t BKP_Read_Flags(void)
{
    return (uint16_t)HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);
}

/* 读取 32 位输出计数 (需合并) */
uint32_t BKP_Read_OutputCount(void)
{
    uint32_t low = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2);
    uint32_t high = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR3);
    return (high << 16) | low;
}

/* 读取 32 位 ADC 增益 */
float BKP_Read_AdcGain(void)
{
    uint16_t low = (uint16_t)HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR4);
    uint16_t high = (uint16_t)HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR5);
    uint32_t combined = ((uint32_t)high << 16) | low;

    float gain;
    memcpy(&gain, &combined, 4);
    return gain;
}

/* 读取 16 位电池电压 */
uint16_t BKP_Read_BatVoltage(void)
{
    return (uint16_t)HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR6);
}

/* 读取 8 位电量 */
uint8_t BKP_Read_BatLevel(void)
{
    return (uint8_t)HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR7);
}

uint16_t Modbus_Addr_Baudrate;
uint16_t Flags;
uint16_t B_voltage;
uint8_t B_level;
uint8_t modbus_baudrate;
uint8_t modbus_slave_addr;
uint16_t run_count;
uint16_t mermery_ram;
uint32_t MB_Master_Usart_Baudrate;
float F_gain;
uint32_t Count_num;

// 保存 BKP 参数
void BKP_Unlock_Save(void)
{
    Flags = 0;
    if (g_b_power_onoff)
        Flags |= 0x01;
    if (g_b_lock_machine)
        Flags |= 0x02;
    if (g_b_machine_tim_error)
        Flags |= 0x04;
    if (g_b_machine_outputtim_error)
        Flags |= 0x08;

    BKP_Unlock(); // 解锁 BKP 写保护
    // 保存状态标志位集合
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, (uint32_t)Flags);

    /* 保存输出计数 */
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, (uint32_t)(Count_num & 0xFFFF));         // 低16位
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, (uint32_t)((Count_num >> 16) & 0xFFFF)); // 高16位
    /* 保存 ADC 增益 */
    uint16_t *p = (uint16_t *)&F_gain;
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, (uint32_t)p[0]); // 低16位
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, (uint32_t)p[1]); // 高16位

    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR6, (uint32_t)B_voltage); // 保存电池电压
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR7, (uint32_t)B_level);   // 保存电量
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR8, (uint32_t)run_count); // 保存运行次数
    /* 保存 Modbus 地址/频率 前八位地址，后八位频率选择*/
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR9, (uint32_t)Modbus_Addr_Baudrate); // 保存 Modbus 地址
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR10, (uint32_t)(BKP_MAGIC_NUM));     // 保存魔数

    HAL_PWR_DisableBkUpAccess(); // 立即上锁：恢复写保护，防止后续误操作
}

// 读取 BKP 参数
void BKP_Unlock_Read(void)
{
    // 读取状态标志位集合
    Flags = (uint16_t)HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);
    // 读取输出计数
    Count_num = (uint32_t)(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2) << 16) | HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR3);
    // 读取 ADC 增益
    uint16_t low = (uint16_t)HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR4);
    uint16_t high = (uint16_t)HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR5);
    F_gain = ((uint32_t)high << 16) | low;
    // 读取电池电压
    B_voltage = (uint16_t)HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR6);
    // 读取电量
    B_level = (uint8_t)HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR7);
    // 读取运行次数
    run_count = (uint16_t)HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR8);
    // 读取 Modbus 地址
    Modbus_Addr_Baudrate = (uint16_t)HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR9);
    // 读取魔数
    mermery_ram = (uint32_t)HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR10);
}

void BKP_Unlock_init(void)
{
    BKP_Unlock();      // 1. 解锁 BKP 写保护
    BKP_Unlock_Read(); // 2. 读取 BKP 参数
    if (mermery_ram == BKP_MAGIC_NUM)
    {
        if (Flags & 0x01)
            g_b_power_onoff = 1;
        if (Flags & 0x02)
            g_b_lock_machine = 1;
        if (Flags & 0x04)
            g_b_machine_tim_error = 1;
        if (Flags & 0x08)
            g_b_machine_outputtim_error = 1;

        modbus_baudrate = Modbus_Addr_Baudrate >> 8;     // 高八位为 Modbus 频率
        modbus_slave_addr = Modbus_Addr_Baudrate & 0xFF; // 低八位为 Modbus 从站地址
        if (modbus_baudrate == 1)
            MB_Master_Usart_Baudrate = 9600;
        else if (modbus_baudrate == 2)
            MB_Master_Usart_Baudrate = 115200;
        else
            MB_Master_Usart_Baudrate = 9600;

        run_count++;
        if (run_count >= 0xFFF0)
        {
            run_count = 0;
        }
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR8, (uint32_t)run_count); // 更新运行次数
    }
    else
    {
        // 【情况 B】魔数错误，说明是首次上电，或者 VBAT 电池没电导致数据丢失
        // 必须在这里赋予所有参数安全的“出厂默认值”！

        g_b_power_onoff = 0;
        g_b_lock_machine = 0;
        g_b_machine_tim_error = 0;
        g_b_machine_outputtim_error = 0;

        Count_num = 0;
        F_gain = 1.0f;     // 默认增益 1.0
        B_voltage = 400;   // 默认电池电压 4V 单位10mV
        B_level = 100;     // 默认满电
        run_count = 1;   // 首次运行，次数为 1
        modbus_slave_addr = 1; // 默认 Modbus 地址为 1

        // 将默认值写入 BKP 并打上魔数
        BKP_Unlock_Save();
    }

//  初始化完成，立即上锁：恢复写保护，防止后续误操作
HAL_PWR_DisableBkUpAccess();
#if 0
    BKP_Unlock();
    // 2. 尝试从 BKP 加载参数
    BKP_LoadParams(&myParams);
    // 3. 校验魔数，判断是第一次上电，还是看门狗复位
    if (myParams.magic_number != BKP_MAGIC_NUM)
    {
        // 第一次上电或 VBAT 没电，加载默认参数
        myParams.modbus_addr = 1;
        myParams.adc_offset = 0.0f;
        myParams.adc_gain = 1.0f;
        myParams.run_count = 0;

        // 将默认参数存入 BKP
        BKP_SaveParams(&myParams);
    }
    else
    {
        myParams.run_count++;      // 运行次数+1
        BKP_SaveParams(&myParams); // 更新运行次数
    }
    // 4. 立即上锁：恢复写保护，防止后续误操作
    HAL_PWR_DisableBkUpAccess();
#endif
}

// void Modbus_Task(void *argument)
// {
//     if (modbus_cmd_received == 1)
//     {
//         myParams.adc_gain = new_gain_from_modbus;// 从上位机接收新的 ADC 增益

//         // 直接保存，瞬间完成，不会卡死 FreeRTOS
//         BKP_SaveParams(&myParams);
//     }
// }
