#pragma once

#include "SerialManager.hh"
#include "NfcController.hh"
#include "PwmGenerator.hh"
#include "DoorEvent.hh"

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

/// @brief 开门事件
/// @param pwm 控制舵机的pwm控制器
/// @note 对于pwm占空比 0.2(顺时针最大速度) - 0.4(逆时针最大速度)
void OpenDoor(PwmGenerator pwm)
{
    pwm.SetPwmRatio(0.4);
    HAL_Delay(1400);
    pwm.SetPwmRatio(0.2);
    HAL_Delay(1400);
}