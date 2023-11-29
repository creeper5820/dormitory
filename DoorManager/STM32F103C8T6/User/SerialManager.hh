#pragma once

#include "dma.h"
#include "usart.h"
#include "memory.h"

#define CACHE_SIZE 50

typedef enum {
    WAIT,
    READY,
} Status;

class SerialManager {
private:
    UART_HandleTypeDef* huart_;
    uint8_t status_ = READY;
    uint8_t cache_[CACHE_SIZE];
    uint32_t cache_size_ = 0;

public:
    ///@brief 构造函数
    /// @param huart 传入串口的操作柄
    SerialManager(UART_HandleTypeDef* huart)
        : huart_(huart)
    {
    }

    ///@brief 串口发送
    ///@param type 模式 0为正常发送 1为DMA发送
    ///@param data 数据包的首地址
    ///@param size 数据包的大小
    ///@param time_out 正常发送的超时时间 默认50ms DMA发送时可以忽略
    /// @retval HAL status: HAL_OK(0x00) | HAL_ERROR | HAL_BUSY | HAL_TIMEOUT
    HAL_StatusTypeDef Send(uint8_t type, uint8_t* data, uint16_t size, uint32_t time_out = 50)
    {
        switch (type) {
        case 0:
            return HAL_UART_Transmit(huart_, (uint8_t*)data, size, time_out);
        case 1:
            return HAL_UART_Transmit_DMA(huart_, (uint8_t*)data, size);
        }

        return HAL_ERROR;
    }

    /// @brief 开启串口接收
    ///@param type 0.阻塞接收  1.中断接收  2.DMA  3.ToIdle_DMA
    ///@param time_out 阻塞发送的超时时间，默认为50
    ///@retval HAL status: HAL_OK(0x00) | HAL_ERROR | HAL_BUSY | HAL_TIMEOUT
    HAL_StatusTypeDef Recevice(uint8_t type, uint32_t time_out = 50)
    {
        switch (type) {
        case 0:
            // 阻塞发送直接使用最大长度
            cache_size_ = CACHE_SIZE;
            return HAL_UART_Receive(huart_, (uint8_t*)cache_, CACHE_SIZE, time_out);
        case 1:
            return HAL_UART_Receive_IT(huart_, (uint8_t*)cache_, CACHE_SIZE);
        case 2:
            return HAL_UART_Receive_DMA(huart_, (uint8_t*)cache_, CACHE_SIZE);
        case 3:
            return HAL_UARTEx_ReceiveToIdle_DMA(huart_, (uint8_t*)cache_, CACHE_SIZE);
        }

        return HAL_ERROR;
    }

    ///@brief 获取串口的操作柄
    ///@retval UART_HandleTypeDef
    UART_HandleTypeDef* GetHandleType()
    {
        return huart_;
    }

    ///@brief 获取串口的状态
    /// @note 状态目前有两个 READY 和 WAIT
    /// @retval uint8_t类型的状态值
    uint8_t GetStatus()
    {
        return status_;
    }

    ///@brief 获取数据缓存的首地址，最大长度为 CACHE_SIZE
    ///@retval uint8_t *指针
    uint8_t* GetData()
    {
        return cache_;
    }

    ///@brief 获取数据缓存的长度值
    uint32_t GetDataSize()
    {
        return cache_size_;
    }

    ///@brief 设置串口管理的状态为等待中
    void SetWait()
    {
        status_ = WAIT;
    }

    ///@brief 设置串口管理的状态为就绪
    void SetReady()
    {
        status_ = READY;
    }

    /// @brief 设置当前缓存数据的真实长度大小
    ///@param size 数据的真实长度大小
    void SetSize(uint32_t size)
    {
        cache_size_ = size;
    }

    /// @brief 清空缓存数据，将所有数据赋为 \0
    void EmptyCache()
    {
        memset(cache_, '\0', CACHE_SIZE);
    }

    /// @brief 终端回调函数
    /// @param huart 串口句柄
    void CallBack(UART_HandleTypeDef* huart, uint16_t size)
    {
        if (huart == huart_) {
            SetReady();
            cache_size_ = size;
        }
    }

    /// @brief 你好世界 测试用函数
    /// @param type 发送的类型，1.阻塞发送 2.DMA发送
    void HelloWorld(uint8_t type)
    {
        Send(type, (uint8_t*)&"Hello World!\n", sizeof("Hello World!"));
    }
};
