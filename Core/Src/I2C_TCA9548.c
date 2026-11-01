/*
 *   ******************************************************************************
 * @file I2C_TCA9548.c
 * @brief I2C 擴充IC TCA9548 週邊功能程式
 *
 *  @date oct 21, 2026<br/>
 *  @author  Deepseek and Ethan<br/>
 *
 *    ******************************************************************************
 */


#include <I2C_Peripherals.h>
#include "I2C_TCA9548.h"
#include <stdio.h>

/* 目前通道快取（0xFF = 未知，強制下一次一定寫入） */
static uint8_t s_current_mask = 0xFF;

/**
  * @brief  初始化 TCA9548A
  * @retval TCA9548_OK        成功（裝置在位）
  * @retval TCA9548_ERR_COMM  裝置無回應
  * @note   本函式只確認「TCA9548A 在位」，不選擇通道。
  *         呼叫端可接著呼叫 TCA9548_SelectChannel() 選通道。
  */
TCA9548_Status TCA9548_Init(void)
{
    HAL_StatusTypeDef st = HAL_I2C_IsDeviceReady(
        hi2c_main,
        (uint16_t)(TCA9548_I2C_ADDR << 1),
        3,
        RTC_TIMEOUT);

    if (st != HAL_OK) {
        printf("[TCA9548] Init failed: device not found at 0x%02X\n",
               TCA9548_I2C_ADDR);
        return TCA9548_ERR_COMM;
    }

    /* 清除快取，確保第一次 SelectChannel 一定會寫入 */
    s_current_mask = 0xFF;

    printf("[TCA9548] Init OK at 0x%02X\n\n", TCA9548_I2C_ADDR);
    return TCA9548_OK;
}


/**
  * @brief  選擇單一通道（0~7），會關閉其他通道
  * @param  ch  通道編號（0~7）
  * @retval TCA9548_OK           成功
  * @retval TCA9548_ERR_INVALID_CH  通道編號超出範圍
  * @retval TCA9548_ERR_TIMEOUT  I2C 逾時
  * @retval TCA9548_ERR_COMM     I2C 通訊失敗
  * @note   若目前通道已是 ch，會直接回傳 OK（快取命中，不重複寫入）。
  */
TCA9548_Status TCA9548_SelectChannel(uint8_t ch)
{
    if (ch > 7) {
        return TCA9548_ERR_INVALID_CH;
    }

    uint8_t mask = (uint8_t)(1u << ch);

    /* 快取命中：已經在該通道，跳過 I2C 交易 */
    if (mask == s_current_mask) {
        return TCA9548_OK;
    }

    HAL_StatusTypeDef st = HAL_I2C_Master_Transmit(
        hi2c_main,
        (uint16_t)(TCA9548_I2C_ADDR << 1),
        &mask,
        1,
        RTC_TIMEOUT);

    if (st == HAL_OK) {
        s_current_mask = mask;
        return TCA9548_OK;
    }

    /* 寫入失敗，清除快取，避免誤以為通道已切換 */
    s_current_mask = 0xFF;

    if (st == HAL_TIMEOUT) {
        return TCA9548_ERR_TIMEOUT;
    }
    return TCA9548_ERR_COMM;
}

/**
  * @brief  關閉所有通道（隔離 downstream）
  * @retval TCA9548_OK        成功
  * @retval TCA9548_ERR_COMM  通訊失敗
  * @note   寫入 0x00 到控制暫存器，所有通道關閉。
  */
TCA9548_Status TCA9548_DisableAll(void)
{
    uint8_t mask = 0x00;

    HAL_StatusTypeDef st = HAL_I2C_Master_Transmit(
        hi2c_main,
        (uint16_t)(TCA9548_I2C_ADDR << 1),
        &mask,
        1,
        RTC_TIMEOUT);

    s_current_mask = 0x00;

    if (st == HAL_OK) {
        return TCA9548_OK;
    }
    return TCA9548_ERR_COMM;
}

/**
  * @brief  讀回目前通道設定（除錯用）
  * @param  mask  輸出：目前控制暫存器的值（bit0~bit7 對應通道 0~7）
  * @retval TCA9548_OK        成功
  * @retval TCA9548_ERR_COMM  通訊失敗
  * @note   讀回的是「TCA9548A 內部的控制暫存器值」，不是快取。
  */
TCA9548_Status TCA9548_GetChannel(uint8_t *mask)
{
    if (mask == NULL) {
        return TCA9548_ERR_INVALID_CH;
    }

    HAL_StatusTypeDef st = HAL_I2C_Master_Receive(
        hi2c_main,
        (uint16_t)(TCA9548_I2C_ADDR << 1),
        mask,
        1,
        RTC_TIMEOUT);

    if (st == HAL_OK) {
        return TCA9548_OK;
    }
    return TCA9548_ERR_COMM;
}



// 定義一個靜態變數，用來記住「目前 TCA9548 停留在哪個通道」
// 0xFF 代表未知或尚未切換
static uint8_t current_active_channel = 0xFF;

/**
 * @brief 根據設備位址，自動切換 TCA9548 通道
 * @param target_addr 目標設備的 7-bit 位址 (例如 PCA9685 是 0x40)
 * @retval HAL_OK 切換成功 / HAL_ERROR 找不到設備或切換失敗
 */
HAL_StatusTypeDef TCA9548_SelectByAddress(uint8_t target_addr) {
    // 1. 走訪掃描陣列，尋找位址
    for (uint8_t i = 0; i < g_scan_count; i++) {
        if (g_scan_result[i].Peripheral_address == target_addr) {
            uint8_t target_ch = g_scan_result[i].TCA9548_CH;

            // 2. 如果目前已經在這個通道，直接回傳成功，省下一次 I2C 通訊
            if (current_active_channel == target_ch) {
                return HAL_OK;
            }

            // 3. 如果不在，就切換通道
            if (TCA9548_SelectChannel(target_ch) == TCA9548_OK) {
                current_active_channel = target_ch; // 更新記憶
                //printf("[MUX] 自動切換至 CH%u (目標: 0x%02X)\n", target_ch, target_addr);
                return HAL_OK;
            } else {
                printf("[MUX] 切換至 CH%u 失敗！\n", target_ch);
                current_active_channel = 0xFF; // 狀態未知
                return HAL_ERROR;
            }
        }
    }

    // 找不到
    printf("[MUX] 錯誤：掃描清單中找不到 0x%02X\n", target_addr);
    return HAL_ERROR;
}
