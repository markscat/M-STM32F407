#ifndef LOG_H
#define LOG_H

#include <stdio.h> // for printf

// --- 日志级别控制 ---
// 在这里改变日志级别，0=关闭, 1=只打印错误, 2=打印错误和信息
#define LOG_LEVEL 2

// --- 日志宏定义 ---
#if (LOG_LEVEL >= 1)
  #define LOG_ERROR(format, ...) printf("[ERROR] %s:%d: " format "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
  #define LOG_ERROR(format, ...) ((void)0) // 如果级别不够，宏展开为空操作
#endif

#if (LOG_LEVEL >= 2)
  #define LOG_INFO(format, ...) printf("[INFO] " format "\r\n", ##__VA_ARGS__)
#else
  #define LOG_INFO(format, ...) ((void)0)
#endif

#endif // LOG_H
