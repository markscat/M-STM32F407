/*
 * I2C_PCA9685.h
 *
 *  Created on: 2026年10月29日
 *      Author: user
 */

#ifndef INC_I2C_PCA9685_H_
#define INC_I2C_PCA9685_H_

#define PCA9685_ADDR 0x40 // 假設 A0~A5 全接地

/* =========================================================
 * PCA9685 暫存器位址定義
 * ========================================================= */

// 1. 模式暫存器 (Mode Registers)
#define PCA9685_MODE1           0x00  // 模式暫存器 1 (重啟、休眠、自動遞增等)
#define PCA9685_MODE2           0x01  // 模式暫存器 2 (反相、輸出驅動模式等)


// 2. 通道控制暫存器 (Channel Registers)
// 16 個通道，每個通道佔用 4 個位元組，依序為 ON_L, ON_H, OFF_L, OFF_H
#define PCA9685_LED0_ON_L       0x06  // 通道 0 的 ON 低 8 位
#define PCA9685_LED0_ON_H       0x07  // 通道 0 的 ON 高 4 位
#define PCA9685_LED0_OFF_L      0x08  // 通道 0 的 OFF 低 8 位
#define PCA9685_LED0_OFF_H      0x09  // 通道 0 的 OFF 高 4 位

// 最後一個通道 (通道 15) 的位址
#define PCA9685_LED15_ON_L      0x42
#define PCA9685_LED15_ON_H      0x43
#define PCA9685_LED15_OFF_L     0x44
#define PCA9685_LED15_OFF_H     0x45

// 3. 全通道廣播暫存器 (ALL CALL)
// 寫入這些暫存器，會同時改變所有 16 個通道
#define PCA9685_ALL_LED_ON_L    0xFA  // 全通道 ON 低 8 位
#define PCA9685_ALL_LED_ON_H    0xFB  // 全通道 ON 高 4 位
#define PCA9685_ALL_LED_OFF_L   0xFC  // 全通道 OFF 低 8 位
#define PCA9685_ALL_LED_OFF_H   0xFD  // 全通道 OFF 高 4 位

// 4. 頻率設定暫存器 (Prescale)
#define PCA9685_PRESCALE        0xFE  // 寫入此暫存器設定 PWM 頻率 (必須在 Sleep 模式下寫入)

// 5. 測試暫存器 (Test Mode)
#define PCA9685_TESTMODE        0xFF  // 保留給原廠測試用，一般不要動它

/* MODE1 位元定義 */
#define PCA9685_MODE1_RESTART   0x80  // Bit 7: 重啟 PWM 輸出
#define PCA9685_MODE1_EXTCLK    0x40  // Bit 6: 使用外部時鐘
#define PCA9685_MODE1_AI        0x20  // Bit 5: 自動遞增 (Auto-Increment)
#define PCA9685_MODE1_SLEEP     0x10  // Bit 4: 休眠模式
#define PCA9685_MODE1_SUB1      0x08  // Bit 3: 回應 SUB1 位址
#define PCA9685_MODE1_SUB2      0x04  // Bit 2: 回應 SUB2 位址
#define PCA9685_MODE1_SUB3      0x02  // Bit 1: 回應 SUB3 位址
#define PCA9685_MODE1_ALLCALL   0x01  // Bit 0: 回應 ALL CALL 位址

/* MODE2 位元定義 */
#define PCA9685_MODE2_INVRT     0x10  // Bit 4: 反相輸出 (1=反相, 0=正常)
#define PCA9685_MODE2_OCH       0x08  // Bit 3: 輸出更新時機 (0=立即, 1=收到 STOP 時)
#define PCA9685_MODE2_OUTDRV    0x04  // Bit 2: 輸出驅動模式 (0=開漏, 1=圖騰柱)

// 喚醒並設定頻率
HAL_StatusTypeDef  PCA9685_Init(void);

// 設定指定通道的脈寬 (off_ticks)
HAL_StatusTypeDef  PCA9685_SetPWM(uint8_t channel, uint16_t off_ticks);

#endif /* INC_I2C_PCA9685_H_ */
