#pragma once

#include "SerialManager.hh"
#include "NfcController.hh"
#include "PwmGenerator.hh"
#include "DoorEvent.hh"

#define MAX_PWM_RATIO 0.135
#define MIN_PWM_RATIO 0.015

void CountAdd(uint32_t& count)
{
    HAL_Delay(1);
    count++;
}

bool IsTask(const uint32_t& count, const uint16_t count_task)
{
    if (count % count_task == 0)
        return 1;
    else
        return 0;
}

/// @brief 设置舵机位置
/// @param speed -1(逆时针) ~ 1(顺时针)
void SetSteerPosition(PwmGenerator& pwm, float speed)
{
    if (speed > 1 || speed < -1)
        return;

    pwm.SetPwmRatio((MIN_PWM_RATIO - MAX_PWM_RATIO) * speed / 2 + (MAX_PWM_RATIO + MIN_PWM_RATIO) / 2);
}

/// @brief 开门事件
/// @param pwm 控制舵机的pwm控制器
void OpenDoor(PwmGenerator& pwm)
{
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

    SetSteerPosition(pwm, -0.45);
    HAL_Delay(1000);

    SetSteerPosition(pwm, 0.96);
    HAL_Delay(1000);

    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}