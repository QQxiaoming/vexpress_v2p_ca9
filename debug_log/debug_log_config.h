#ifndef _DEBUG_LOG_CONFIG_H_
#define _DEBUG_LOG_CONFIG_H_


/* log打印等级 参考Andriod Logcat */
#define DEBUG_LOG_VERBOSE             (1 << 6)
#define DEBUG_LOG_DEBUG               (1 << 5)
#define DEBUG_LOG_INFO                (1 << 4)
#define DEBUG_LOG_WARN                (1 << 3)
#define DEBUG_LOG_ERROR               (1 << 2)
#define DEBUG_LOG_ASSERT              (1 << 1)
#define DEBUG_LOG_NONE                (1 << 0)

/* 打印模块开关 */
#define CONFIG_DEBUG_LOG_EN            1
#define UART_LOG_BUFF_SIZE          1024

/* 打印等级 */
#define LOG_ASSERT                  DEBUG_LOG_ERROR
#define LOG_SYS_INFO                DEBUG_LOG_DEBUG

#endif
