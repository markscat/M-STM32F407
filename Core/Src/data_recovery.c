/*
 * data_recovery.c
 *
 *  Created on: Mar 16, 2025
 *      Author: user
 */


#include <I2C_Peripherals.h>
#include "data_recovery.h"
#include "error_handler.h"
#include <string.h>
#include <stdlib.h>


// 冗余存储配置
#define REDUNDANT_COPIES   3       // 冗余副本数量
#define BACKUP_BASE_ADDR   0x0800  // 备份区起始地址
#define DATA_SIGNATURE     0x55AA  // 数据有效性标记

typedef struct {
    uint16_t signature;    // 数据有效性标记
    uint16_t crc;          // 数据CRC校验码
    uint32_t version;      // 数据版本号
    uint8_t  data[];       // 可变长度数据
} BackupBlock;


static void Load_Default_Data(uint8_t *data, uint16_t size);
static void Restore_By_Voting(BackupBlock **copies,
                             uint8_t *data,
                             uint16_t size,
                             uint8_t valid_count);



/**
  * @brief 处理数据损坏事件
  * @param addr    主数据存储地址
  * @param data    数据缓冲区
  * @param size    数据长度
  * @retval 恢复结果 (true:成功 false:失败)
  */



/* 多副本投票恢复算法 */


static void Load_Default_Data(uint8_t *data, uint16_t size) {
  const uint8_t defaults[] = {0x00,0x01,0x02,0x03}; // 示例默认数据
  size = (size > sizeof(defaults)) ? sizeof(defaults) : size;
  memcpy(data, defaults, size);
}


bool Handle_Data_Corruption(uint16_t addr, uint8_t *data, uint16_t size) {
  BackupBlock *copies[REDUNDANT_COPIES];
  uint8_t valid = 0;

  // 读取备份数据
  for(int i=0; i<REDUNDANT_COPIES; i++) {
    copies[i] = malloc(sizeof(BackupBlock)+size);
    if(EEPROM_Read(BACKUP_BASE_ADDR+i*(sizeof(BackupBlock)+size),
                  (uint8_t*)copies[i], sizeof(BackupBlock)+size) == EEPROM_OK) {
      valid++;
    }
  }
  // 恢复处理
  if(valid == 0) {
    Load_Default_Data(data, size);
  } else {
    Restore_By_Voting(copies, data, size, valid);
  }

  // 清理资源
  for(int i=0; i<REDUNDANT_COPIES; i++) free(copies[i]);
  return (valid > 0);
}

void Restore_By_Voting(BackupBlock **copies,
                             uint8_t *data,
                             uint16_t size
							,uint8_t valid_count)  // 新增参数
{
    // 即使暂时不用valid_count，也需要保留参数占位
    //(void)valid_count;  // 避免未使用参数警告

    // 具体实现逻辑...
    for(int i=0; i<REDUNDANT_COPIES; i++) {
        if(copies[i]->signature == 0x55AA &&
           copies[i]->crc == Compute_CRC(copies[i]->data, size)) {
            memcpy(data, copies[i]->data, size);
            return;
        }
    }
}





#ifdef testVer
/* 更新所有备份副本 */
static void Update_All_Backups(uint16_t main_addr, uint8_t* data, uint16_t size) {
    BackupBlock new_backup = {
        .signature = DATA_SIGNATURE,
        .crc = Compute_CRC(data, size),
        .version = Get_Current_Version() + 1
    };
    memcpy(new_backup.data, data, size);

    for(int i=0; i<REDUNDANT_COPIES; i++) {
        uint16_t backup_addr = BACKUP_BASE_ADDR + i*(sizeof(BackupBlock)+size);
        EEPROM_Write(backup_addr, (uint8_t*)&new_backup, sizeof(BackupBlock)+size);
    }
}




// 版本管理函数
void Get_Current_Version(uint32_t version) {
  version = 0;
  EEPROM_Read(VERSION_STORE_ADDR, (uint8_t*)&version, sizeof(version));
  return version;
}

void Write_Global_Version(uint32_t version) {
  EEPROM_Write(VERSION_STORE_ADDR, (uint8_t*)&version, sizeof(version));
}




/*
bool Handle_Data_Corruption(uint16_t addr, uint8_t* data, uint16_t size) {
    BackupBlock* backups[REDUNDANT_COPIES];
    uint8_t valid_backups = 0;

    // 步骤1：读取所有备份副本
    for(int i=0; i<REDUNDANT_COPIES; i++) {
        uint16_t backup_addr = BACKUP_BASE_ADDR + i*(sizeof(BackupBlock)+size);
        backups[i] = (BackupBlock*)malloc(sizeof(BackupBlock)+size);

        if(EEPROM_Read(backup_addr, (uint8_t*)backups[i],
                      sizeof(BackupBlock)+size) == EEPROM_OK) {
            // 校验签名和CRC
            if(backups[i]->signature == DATA_SIGNATURE &&
               Compute_CRC(backups[i]->data, size) == backups[i]->crc) {
                valid_backups++;
            }
        }
    }

    // 步骤2：决策恢复策略
    if(valid_backups == 0) {
        // 无有效备份，恢复出厂设置
        Load_Default_Data(data, size);
        return false;
    }
    else if(valid_backups == 1) {
        // 单一有效副本直接恢复
        for(int i=0; i<REDUNDANT_COPIES; i++) {
            if(backups[i]->signature == DATA_SIGNATURE) {
                memcpy(data, backups[i]->data, size);
                break;
            }
        }
    }
    else {
        // 多副本投票机制
        Restore_By_Voting(backups, data, size, valid_backups);
    }

    // 步骤3：修复主存储区
    if(EEPROM_Write(addr, data, size) != EEPROM_OK) {
        return false;
    }

    // 步骤4：刷新备份
    Update_All_Backups(addr, data, size);

    // 清理资源
    for(int i=0; i<REDUNDANT_COPIES; i++) {
        free(backups[i]);
    }

    return true;
}
*/
/*
static void Restore_By_Voting(BackupBlock** backups, uint8_t* data,
                            uint16_t size, uint8_t valid_count) {
    uint32_t max_version = 0;
    uint8_t selected = 0;

    // 选择最新版本
    for(int i=0; i<REDUNDANT_COPIES; i++) {
        if(backups[i]->version > max_version) {
            max_version = backups[i]->version;
            selected = i;
        }
    }
    memcpy(data, backups[selected]->data, size);
}
*/
#endif
