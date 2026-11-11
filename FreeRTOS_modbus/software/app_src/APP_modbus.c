#include "app_modbus.h"
#include "mb.h"
#include "mbframe.h"

#if 0
// 1. 定义双缓冲区
uint16_t usRegBuf_A[REG_NUM] = {0};
uint16_t usRegBuf_B[REG_NUM] = {0};

// 2. 定义读写指针
uint16_t *volatile ptr_ReadBuf = usRegBuf_A;
uint16_t *volatile ptr_WriteBuf = usRegBuf_B;

// 3. 提供给 ADC 任务调用的更新函数
void Modbus_Update_Registers(uint16_t *new_data, uint16_t len)
{
    if (len > REG_NUM)
        len = REG_NUM;

    // 拷贝数据到后台写缓冲
    for (int i = 0; i < len; i++)
    {
        ptr_WriteBuf[i] = new_data[i];
    }
    taskENTER_CRITICAL();/* 进入临界区 */
    // 瞬间交换指针（原子操作）
    uint16_t *temp = ptr_ReadBuf;
    ptr_ReadBuf = ptr_WriteBuf;
    ptr_WriteBuf = temp;
    taskEXIT_CRITICAL();/* 退出临界区 */
}

// 4. Modbus 读写回调
eMBErrorCode eMBRegHoldingCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode)
{
    eMBErrorCode eStatus = MB_ENOERR;

    // FreeModbus 内部处理时，地址会 +1。所以我们要在这里 -1 还原成从 0 开始的数组索引
    usAddress--;

    // 检查地址是否越界 (假设你的寄存器起始地址是 0)
    if ((usAddress + usNRegs) > REG_NUM)
    {
        return MB_ENOREG; // 返回异常码 02 (非法数据地址)
    }

    switch (eMode)
    {
    case MB_REG_READ:
        // 主机读：从【读双缓冲】里取最新数据发给主机
        for (uint16_t i = 0; i < usNRegs; i++)
        {
            uint16_t val = ptr_ReadBuf[usAddress + i];
            *pucRegBuffer++ = (UCHAR)(val >> 8);   // 高字节
            *pucRegBuffer++ = (UCHAR)(val & 0xFF); // 低字节
        }
        break;

    case MB_REG_WRITE:
        // 主机写：把主机发来的数据存入【写双缓冲】
        for (uint16_t i = 0; i < usNRegs; i++)
        {
            uint16_t val = (*pucRegBuffer++ << 8);
            val |= *pucRegBuffer++;
            ptr_WriteBuf[usAddress + i] = val;
        }
        // 如果希望主机写完后立刻能读到自己写的值，可以顺便同步给读缓冲区：
        for (uint16_t i = 0; i < usNRegs; i++)
        {
            ptr_ReadBuf[usAddress + i] = ptr_WriteBuf[usAddress + i];
        }
        break;
    }

    return eStatus;
}
#endif
