#pragma once

#include "MFRC522.h"
#include "SPI.h"

#define SS_PIN D8
#define RST_PIN D0

typedef struct User_ID {
    byte uid[4] = { 0 };
} User_ID;

// 二次封装源代码的类
class NFC_Controller {
private:
    MFRC522 mfcr522;
    MFRC522::MIFARE_Key key_;
    User_ID user_id_[4];

public:
    NFC_Controller()
        : mfcr522(MFRC522(SS_PIN, RST_PIN))
    {
        for (byte i = 0; i < 6; i++) {
            key_.keyByte[i] = 0xFF;
        }
    }

    void NFC_Init()
    {
        // 协议和类的初始化
        SPI.begin();
        mfcr522.PCD_Init();

        Serial.println();
        Serial.print(F("Reader :"));

        // 打印一些信息
        mfcr522.PCD_DumpVersionToSerial();

        for (byte i = 0; i < 6; i++) {
            key_.keyByte[i] = 0xFF;
        }

        Serial.println();
        Serial.println(F("This code scan the MIFARE Classic NUID."));
        Serial.print(F("Using the following key:"));
        Print_Hex(key_.keyByte, MFRC522::MF_KEY_SIZE);
    }

    bool Find_Card()
    {
        if (
            // 排除一些情况
            !mfcr522.PICC_IsNewCardPresent()
            || !mfcr522.PICC_ReadCardSerial())
            return false;
        else {
            // // 打印NFC卡的类型
            // Serial.print(F("PICC type: "));
            MFRC522::PICC_Type piccType = mfcr522.PICC_GetType(mfcr522.uid.sak);
            // Serial.println(mfcr522.PICC_GetTypeName(piccType));

            // 处理不能处理的NFC卡类型
            if (!Check_Type(piccType))
                return false;

            // // 打印UID TAG
            // Serial.println(F("The NUID tag is:"));
            // Serial.print(F("In hex: "));
            // Print_Hex(mfcr522.uid.uidByte, mfcr522.uid.size);
            // Serial.println();
            // Serial.print(F("In dec: "));
            // Print_Dec(mfcr522.uid.uidByte, mfcr522.uid.size);
            // Serial.println();

            // Halt PICC
            mfcr522.PICC_HaltA();

            // Stop encryption on PCD
            mfcr522.PCD_StopCrypto1();
            return true;
        }
    }

    /**
     * @brief 判断NFC卡是否匹配某一用户
    */
    bool Identify_User(int user_number)
    {
        return user_id_[user_number].uid[0] == mfcr522.uid.uidByte[0]
            && user_id_[user_number].uid[1] == mfcr522.uid.uidByte[1]
            && user_id_[user_number].uid[2] == mfcr522.uid.uidByte[2]
            && user_id_[user_number].uid[3] == mfcr522.uid.uidByte[3];
    }

    /**
     * @brief 判断NFC卡是否属于南京理工大学410宿舍的成员
    */
    bool Identify_All()
    {
        return Identify_User(0)
            || Identify_User(1)
            || Identify_User(2)
            || Identify_User(3);
    }

    // 设置某一位成员的UID TAG
    void Set_User_ID(User_ID user_id, int number)
    {
        for (int count = 0; count < 4; count++)
            user_id_[number].uid[count] = user_id.uid[count];
    }

    bool Check_Type(MFRC522::PICC_Type piccType)
    {
        if (piccType == MFRC522::PICC_TYPE_MIFARE_MINI
            || piccType == MFRC522::PICC_TYPE_MIFARE_1K
            || piccType == MFRC522::PICC_TYPE_MIFARE_4K)
            return true;
        else {
            Serial.println(F("Your tag is not of type MIFARE Classic."));
            return false;
        }
    }

    void Print_Hex(byte* buffer, byte bufferSize)
    {
        for (byte i = 0; i < bufferSize; i++) {
            Serial.print(buffer[i] < 0x10 ? " 0" : " ");
            Serial.print(buffer[i], HEX);
        }
    }

    void Print_Dec(byte* buffer, byte bufferSize)
    {
        for (byte i = 0; i < bufferSize; i++) {
            Serial.print(buffer[i] < 0x10 ? " 0" : " ");
            Serial.print(buffer[i], DEC);
        }
    }
};