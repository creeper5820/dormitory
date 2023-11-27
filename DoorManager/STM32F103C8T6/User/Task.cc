#include "Task.hh"
#include "main.h"
#include "gpio.h"
#include "SerialManager.hh"
#include "NfcController.hh"
#include "PwmGenerator.hh"
#include "DoorEvent.hh"

#define COUNT_MAX 10000

SerialManager serial(&huart3);

float pwm_ratio = 0;

void Loop()
{
    NfcController nfc(&huart3);
    PwmGenerator pwm(&htim4, TIM_CHANNEL_1);
    uint32_t count = 0;

    while (1) {
        CountAdd(count);

        if (IsTask(count, 100)) {
            pwm.SetPwmRatio(pwm_ratio);
        }

        if (IsTask(count, 4000)) {
            HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        }

        if (IsTask(count, 2000)) {
            nfc.Find();
            // OpenDoor(pwm);
            HAL_Delay(1);
        }

        if (IsTask(count, COUNT_MAX)) {
            nfc.Awake();
            HAL_Delay(1);
            count = 1;
        }
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    serial.CallBack(huart);
}