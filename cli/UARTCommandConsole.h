/**
 * @file UARTCommandConsole.h
 * @author qiaoqiming
 * @brief 
 * @version 1.0
 * @date 2019-06-4
 */
#ifndef _UART_COMMAND_CONSOLE_H_
#define _UART_COMMAND_CONSOLE_H_

#include "serial.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#define printfk(fmt, ...)                                                               \
	do                                                                                  \
	{                                                                                   \
		extern const char *const pcEndOfOutputMessage;                                  \
		_printf("\r\033[K"fmt"%s", ##__VA_ARGS__, pcEndOfOutputMessage + 2);            \
	} while (0)

void vUARTCommandConsoleInit( void );
void vUARTCommandConsoleStart( uint16_t usStackSize, UBaseType_t uxPriority );

void *pvCliMemcpy(void *dest, const void *src, unsigned int count);
void vCliItoa(unsigned int n, char *buf);
int iCliAtoi(char *pstr);
void vCliXtoa(unsigned int n, char *buf);
int iCliFtoa(double num, int n,char *buf);
int iCliIsDigit(unsigned char c);
void *pvCliMemset(void *s, int c, unsigned int count);
int iCliStrcmp(const char* str1, const char* str2);
char *pcCliStrcpy(char *dest, const char *src);
unsigned int uiCliStrlen(const char *s);
int iCliSprintf(char *str, char *fmt, ...);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _UART_COMMAND_CONSOLE_H_ */
