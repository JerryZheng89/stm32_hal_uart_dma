#include "usart.h"
#include "string.h"

//声明外部变量 
extern uint8_t receive_buff[255];
extern DMA_HandleTypeDef hdma_usart1_rx;

void USAR_UART_IDLECallback(UART_HandleTypeDef *huart);

void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)                                   //判断是否是串口1（！此处应写(huart->Instance == USART1)
    {
        if(RESET != __HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE))   //判断是否是空闲中断
        {
            __HAL_UART_CLEAR_IDLEFLAG(huart);                     //清楚空闲中断标志（否则会一直不断进入中断）
            //printf("\r\nUART1 Idle IQR Detected\r\n");
            USAR_UART_IDLECallback(huart);                          //调用中断处理函数
        }
    }
}

                                               
void USAR_UART_IDLECallback(UART_HandleTypeDef *huart)
{
    HAL_UART_DMAStop(huart);                                                     //停止本次DMA传输
    
    uint8_t data_length  = BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);   //计算接收到的数据长度

    //printf("Receive Data(length = %d): ",data_length);
    HAL_UART_Transmit(huart,receive_buff,data_length,0x200);                     //测试函数：将接收到的数据打印出去
    //printf("\r\n");
  
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);
  
    memset(receive_buff,0,data_length);                                            //清零接收缓冲区
    data_length = 0;
    HAL_UART_Receive_DMA(huart, (uint8_t*)receive_buff, 255);                    //重启开始DMA传输 每次255字节数据
}
