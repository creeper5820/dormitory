#pragma once

#include "memory.h"
#include "SerialManager.hh"

/// @brief 唤醒NFC模块的指令
const uint8_t command_start[] = {
    0x55, 0x55, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xFF, 0x03, 0xFD, 0xD4,
    0x14, 0x01, 0x17, 0x00
};

/// @brief 寻找IC卡的指令, 发送之后, 进入寻找状态, 直到寻找到一张卡
/// @note 倘若想开启下一次寻找, 需要再发送一次寻找指令
const uint8_t command_find[] = {
    0x00, 0x00, 0xFF, 0x04, 0xFC,
    0xD4, 0x4A, 0x02, 0x00, 0xE0,
    0x00
};

/// @brief 发送唤醒NFC指令后的应答
const uint8_t return_alive[] = {
    0x00, 0x00, 0xFF, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0xFF, 0x02,
    0xFE, 0xD5, 0x15, 0x16, 0x00
};

/// @brief 应答码
const uint8_t return_ack[] = {
    0x00, 0x00, 0xff, 0x00, 0xff, 0x00
};

/// @brief 头帧
const uint8_t return_head[] = {
    0x00, 0x00, 0xff
};

const uint8_t id_member[][4] = {
    { 0x93, 0x36, 0x2A, 0x13 },
    { 0x13, 0x1A, 0x37, 0x10 },
    { 0x32, 0x7E, 0x6A, 0x53 }
};

enum EVENT {
    NFC_PASS = 0,
    NFC_BLOCK,
    NFC_NONE,
};

typedef int event;

class NfcController {
private:
    SerialManager serial_;

    uint8_t id_[4];
    size_t size_ = 0;

public:
    NfcController(UART_HandleTypeDef* huart)
        : serial_(SerialManager(huart))
    {
        EmptyCache();
        EmptyId();
    }

    void Init()
    {
        Recevice();
        Awake();
    }

    void Recevice()
    {
        serial_.Recevice(3);
    }

    SerialManager& GetSerial()
    {
        return serial_;
    }

    void Awake()
    {
        serial_.Send(1, (uint8_t*)command_start, sizeof(command_start));
    }

    void Find()
    {
        serial_.Send(1, (uint8_t*)command_find, sizeof(command_find));
    }

    void EmptyId()
    {
        memset(id_, '\0', sizeof(id_));
    }

    void EmptyCache()
    {
        memset(serial_.GetData(), '\0', CACHE_SIZE);
        size_ = 0;
    }

    bool CheckAlive()
    {
        if (size_ != sizeof(return_alive))
            return 0;

        for (int i = 0; i < (int)sizeof(return_alive); i++) {
            if (serial_.GetData()[i] != return_alive[i])
                return 0;
        }

        return 1;
    }

    bool Check(int& position, const uint8_t* return_check, int size_check)
    {
        for (int i = 0; i < size_check; i++) {
            if (serial_.GetData()[position + i] != return_check[i])
                return 0;
        }
        position += size_check;
        return 1;
    }

    event ReturnHandler()
    {
        // extern SerialManager type_c;

        if (serial_.GetStatus() != READY)
            return NFC_NONE;

        int position = 0;

        // 校验应答头
        Check(position, return_ack, sizeof(return_ack));

        // 校验帧头
        if (Check(position, return_head, sizeof(return_head)))

            // 校验位数
            if (serial_.GetData()[position] == 0x0c
                && serial_.GetData()[position + 1] == 0xf4) {

                // ID位
                position = 13;

                for (int i = 0; i < 3; i++)
                    if (Check(position, &(id_member[i][0]), 4)) {
                        return NFC_PASS;
                    }

                return NFC_BLOCK;
            }

        return NFC_NONE;
    }
};