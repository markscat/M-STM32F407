/*
 * I2C_PCA9685.c
 *
 *  Created on: 2026年10月29日
 *      Author: user
 */


#include "i2c.h"
#include <I2C_parameter.h>
#include <I2C_PCA9685.h>
/*
#define PCA9685_ADDR 0x40 // 假設 A0~A5 全接地
#define PCA9685_MODE1 0x00
#define PCA9685_PRESCALE 0xFE
#define PCA9685_LED0_ON_L 0x06
*/
HAL_StatusTypeDef PCA9685_Init(void) {
    uint8_t oldmode, newmode;
    HAL_StatusTypeDef status;

    // 1. 讀取目前的 MODE1
    status = HAL_I2C_Mem_Read(hi2c_main, PCA9685_ADDR << 1, PCA9685_MODE1, 1, &oldmode, 1, 100);
    if (status != HAL_OK) return status; // 讀取失敗，直接退貨

    // 2. 進入 Sleep 模式 (準備設定頻率)
    newmode = (oldmode & 0x7F) | 0x10;
    status = HAL_I2C_Mem_Write(hi2c_main, PCA9685_ADDR << 1, PCA9685_MODE1, 1, &newmode, 1, 100);
    if (status != HAL_OK) return status;

    // 3. 寫入 Prescale (50Hz)
    uint8_t prescale = 121;
    status = HAL_I2C_Mem_Write(hi2c_main, PCA9685_ADDR << 1, PCA9685_PRESCALE, 1, &prescale, 1, 100);
    if (status != HAL_OK) return status;

    // 4. 喚醒晶片：開啟 Auto-Increment (0x20)，關閉 Sleep
    uint8_t wakeup_cmd = 0x20;
    status = HAL_I2C_Mem_Write(hi2c_main, PCA9685_ADDR << 1, PCA9685_MODE1, 1, &wakeup_cmd, 1, 100);
    if (status != HAL_OK) return status;

    // 等待振盪器穩定 (官方建議 > 500us)
    HAL_Delay(1);

    // 5. 【關鍵】讀回 MODE1，確認晶片真的醒了 (Sleep bit 為 0)
    uint8_t check_mode;
    status = HAL_I2C_Mem_Read(hi2c_main, PCA9685_ADDR << 1, PCA9685_MODE1, 1, &check_mode, 1, 100);
    if (status != HAL_OK) return status;

    // 檢查 Sleep bit (bit 4) 是否真的被清除了
    if (check_mode & 0x10) {
        return HAL_ERROR; // 還是睡著的，初始化失敗
    }

    return HAL_OK; // 確認晶片已待命，初始化成功
}

/**
 *
	1. 先算 1 個 Ticks 代表多少時間：
		PCA9685 的週期是 20ms（20,000 微秒），切成 4096 份。
		1 個 Tick = 20,000 / 4096 ≈ 4.88 微秒。

	2. 再算你要的脈寬佔了多少 Ticks：
		0 度 (0.5ms = 500微秒)： 500 / 4.88 ≈ 102
		90 度 (1.5ms = 1500微秒)： 1500 / 4.88 ≈ 307
		180 度 (2.5ms = 2500微秒)： 2500 / 4.88 ≈ 512

	3. 那如果要算 45 度呢？
		你可以用內插法，或者直接用公式：
		因為 0 到 180 度之間，對應的數字是從 102 到 512。
		這段範圍總共差了：
		512 - 102 = 410。

		所以每 1 度 = 410 / 180 ≈ 2.27 個 Ticks。

		公式： 目標數字 = 102 + (角度 × 410 / 180)

		算 45 度： 102 + (45 × 2.27) = 204
		算 90 度： 102 + (90 × 2.27) = 307
		算 135 度： 102 + (135 × 2.27) = 410

 *
 */
HAL_StatusTypeDef PCA9685_SetPWM(uint8_t channel, uint16_t off_ticks)
{
    // 邊界檢查：PCA9685 只有 16 個通道 (0~15)
    if (channel > 15) {
        return HAL_ERROR;
    }

    // 限制 off_ticks 範圍：12-bit 最大是 4095
    if (off_ticks > 4095) {
        off_ticks = 4095;
    }

    // 計算該通道的 LED0_ON_L 暫存器位址
    // 公式：起始位址 + 4 * 通道號碼
    uint8_t reg_addr = PCA9685_LED0_ON_L + (4 * channel);

    // 準備要寫入的 4 個 byte
    uint8_t pwm_data[4];

    // ON 永遠設為 0 (代表從週期的一開始就保持低電平，直到 OFF 的時間到)
    pwm_data[0] = 0x00;                         // ON_L
    pwm_data[1] = 0x00;                         // ON_H

    // OFF 設為我們計算好的 ticks
    pwm_data[2] = (uint8_t)(off_ticks & 0xFF);        // OFF_L (低 8 位)
    pwm_data[3] = (uint8_t)((off_ticks >> 8) & 0x0F); // OFF_H (高 4 位，遮罩 0x0F)

    // 透過 I2C 連續寫入 4 個 byte
    return HAL_I2C_Mem_Write(hi2c_main,
                             PCA9685_ADDR << 1,
                             reg_addr,
                             I2C_MEMADD_SIZE_8BIT,
                             pwm_data,
                             4,
                             100);
}
