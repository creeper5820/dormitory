#include "Task.hh"
#include "main.h"
#include "gpio.h"
#include "SerialManager.hh"
#include "NfcController.hh"
#include "PwmGenerator.hh"
#include "DoorEvent.hh"

uint32_t count = 0;
event flag_nfc = NFC_NONE;

NfcController nfc(&huart3);
SerialManager type_c(&huart1);
PwmGenerator pwm(&htim4, TIM_CHANNEL_1);

/// @brief 主循环
void Loop()
{
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);

    nfc.Init();
    pwm.Init();

    while (1) {
        CountAdd(count);

        if (IsTask(count, 100)) {
            flag_nfc = nfc.ReturnHandler();

            if (flag_nfc == NFC_PASS) {
                OpenDoor(pwm);
                nfc.GetSerial().SetWait();
                nfc.EmptyCache();
            }

            if (HAL_GPIO_ReadPin(OPEN_GPIO_Port, OPEN_Pin) == GPIO_PIN_SET)
                OpenDoor(pwm);
        }

        if (IsTask(count, 1000)) {
            nfc.Find();
        }

        if (IsTask(count, 10000)) {
            nfc.Awake();

            count = 0;
        }
    }
}

/// @brief 串口收发回调函数覆写
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t Size)
{
    if (huart == nfc.GetSerial().GetHandleType()) {
        nfc.GetSerial().SetSize(Size);
        nfc.GetSerial().SetReady();
        nfc.Recevice();

        type_c.Send(1, nfc.GetSerial().GetData(), nfc.GetSerial().GetDataSize());
    }
}