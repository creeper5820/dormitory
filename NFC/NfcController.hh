#pragma once

#include "memory.h"
#include "SoftwareSerial.h"

#define TX 5
#define RX 4

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

class NfcController {
private:
    EspSoftwareSerial::UART serial_;

    uint8_t cache_[50];
    uint8_t id_[4];
    size_t size_ = 0;

public:
    NfcController()
    {
    }

    void Init()
    {
        serial_.begin(115200, SWSERIAL_8N1, RX, TX, false);
        if (!serial_)
            while (1) {
                delay(1000);
                Serial.println("Software serial init error");
            }
    }

    void Awake()
    {
        serial_.write(command_start, sizeof(command_start));
    }

    void Find()
    {
        serial_.write(command_find, sizeof(command_find));
    }

    void EmptyId()
    {
        memset(id_, '\0', sizeof(id_));
    }

    void EmptyCache()
    {
        memset(cache_, '\0', sizeof(cache_));
        size_ = 0;
    }

    bool CheckAlive()
    {
        if (size_ != sizeof(return_alive))
            return 0;

        for (int i = 0; i < (int)sizeof(return_alive); i++) {
            if (cache_[i] != return_alive[i])
                return 0;
        }

        return 1;
    }

    void CheckId()
    {
    }

    bool IsAvailable()
    {
        return serial_.available() > 0;
    }

    bool Read()
    {
        if (IsAvailable()) {
            serial_.readBytes(cache_, size_);
            Serial.write(cache_, size_);
            return 1;
        }

        EmptyCache();
        return 0;
    }
};