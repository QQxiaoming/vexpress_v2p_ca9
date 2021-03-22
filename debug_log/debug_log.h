#ifndef _DEBUG_LOG_H_
#define _DEBUG_LOG_H_

#include "debug_log_config.h"
#include "debug_assert.h"

#ifdef __cplusplus
 extern "C" {
#endif

/* 
 * 本模块使用大量多次##重载符，MISR规则19.12中被视为警告，因为该重载符在标准多次使用时
 * 在标准c中宏展开顺序是UB行为，目前测试在IAR8.3和GCC8.2.X中展开处理结果正确，使用c-stat
 * 或pclint请注意。
 */


extern int _printf(char* format,...);
extern void uart_log_int_handler(void);
extern void uart_log_int_mode_init(void);

#define LOG_DEBUG(fmt,...) _printf(fmt,##__VA_ARGS__)


#if CONFIG_DEBUG_LOG_EN
/*need open SecureCRT ansi color*/
#define DEBUG_LOG(debug, comlevel, message, args...)                                          \
    do {                                                                                   \
        if((debug)<=(comlevel)) {                                                          \
            if((debug) & (DEBUG_LOG_ASSERT))LOG_DEBUG("\033[35m"message"\033[0m", ## args);   \
            if((debug) & (DEBUG_LOG_ERROR))LOG_DEBUG("\033[31m"message"\033[0m", ## args);    \
            if((debug) & (DEBUG_LOG_WARN))LOG_DEBUG("\033[33m"message"\033[0m", ## args);     \
            if((debug) & (DEBUG_LOG_INFO))LOG_DEBUG(message, ## args);                        \
            if((debug) & (DEBUG_LOG_DEBUG))LOG_DEBUG("\033[36m"message"\033[0m", ## args);    \
            if((debug) & (DEBUG_LOG_VERBOSE))LOG_DEBUG("\033[32m"message"\033[0m", ## args);  \
        }                                                                                  \
    } while(0)
#else
#define DEBUG_LOG(debug, message, args...)   do{}while(0)           
#endif

/*--------------------------------------------------------------*/
/* Additional user defined functions                            */
/*--------------------------------------------------------------*/
/**
 * @ingroup log
 * @{
 */
/** @brief 日志打印--详细 */
#define debug_logverbose(comlevel, message, args...) DEBUG_LOG(DEBUG_LOG_VERBOSE, comlevel, message, ## args)
/** @brief 日志打印--调试 */
#define debug_logdebug(comlevel, message, args...)   DEBUG_LOG(DEBUG_LOG_DEBUG, comlevel, message, ## args)
/** @brief 日志打印--信息 */
#define debug_loginfo(comlevel, message, args...)    DEBUG_LOG(DEBUG_LOG_INFO, comlevel, message, ## args)
/** @brief 日志打印--警告 */
#define debug_logwarn(comlevel, message, args...)    DEBUG_LOG(DEBUG_LOG_WARN, comlevel, message, ## args)
/** @brief 日志打印--错误 */
#define debug_logerr(comlevel, message, args...)     DEBUG_LOG(DEBUG_LOG_ERROR, comlevel, message, ## args)
/** @brief 日志打印--断言 */
#define debug_logassert(comlevel, message, args...)  DEBUG_LOG(DEBUG_LOG_ASSERT, comlevel, message, ## args)
/** @} */       

#ifdef __cplusplus
}
#endif

#endif
