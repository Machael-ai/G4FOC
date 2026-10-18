#include "main.h"
#include "fdcan.h"  // 确保包含FDCAN头文件
#include "cybergear.h"

#define CAN_EXTENDED_ID  0x1234567  // 29位扩展ID
uint8_t can_data[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88}; // 8字节数据

void Send_Simple_CAN_Frame(void)
{
    FDCAN_TxHeaderTypeDef TxHeader;
    
    // 配置发送帧头（扩展帧）
    TxHeader.Identifier = CAN_EXTENDED_ID;      // 29位ID
    TxHeader.IdType = FDCAN_EXTENDED_ID;        // 扩展帧
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;    // 数据帧
    TxHeader.DataLength = FDCAN_DLC_BYTES_8;    // 数据长度=8字节
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;     // 经典CAN模式
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;      // 经典CAN帧
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;
    
    // 发送数据（使用FIFO队列）
    if(HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, can_data) != HAL_OK)
    {
        // 发送失败处理（可添加错误处理）
        Error_Handler();
    }
}
