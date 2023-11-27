#pragma once

#include "main.h"
#include "tim.h"

class PwmGenerator {
private:
    TIM_HandleTypeDef* htim_;
    uint32_t channel_;

public:
    /// @brief PWM生成器的构造函数
    /// @param htim 定时器的句柄
    /// @param channel 定时器的通道
    PwmGenerator(TIM_HandleTypeDef* htim, uint32_t channel)
        : htim_(htim)
        , channel_(channel)
    {
        HAL_TIM_PWM_Start(htim_, channel_);
    }

    /// @brief 设置PWM的非空值
    /// @param pwm 其为值
    void SetPwm(const uint32_t pwm)
    {
        if (pwm < 0 || pwm > htim_->Init.Period)
            return;

        __HAL_TIM_SET_COMPARE(htim_, channel_, pwm);
    }

    /// @brief 设置PWM的非空比例
    /// @param ratio 其为比例, 范围 0.0 ~ 1.0
    void SetPwmRatio(float ratio)
    {
        if (ratio > 1 || ratio < 0)
            return;

        __HAL_TIM_SET_COMPARE(htim_, channel_, ratio * (htim_->Init.Period));
    }
};
