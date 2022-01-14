/*
 * FreeRTOS Kernel V10.0.1
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*
 * NOTE:  This file uses a third party USB CDC driver.
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Example includes. */
#include "FreeRTOS_CLI.h"

/* Demo application includes. */
#include "serial.h"
#include "UARTCommandConsole.h"

/* Dimensions the buffer into which input characters are placed. */
#define cmdMAX_INPUT_SIZE		50

/* Dimentions a buffer to be used by the UART driver, if the UART driver uses a
buffer at all. */
#define cmdQUEUE_LENGTH			1024

/* DEL acts as a backspace. */
#define cmdASCII_DEL		( 0x7F )
#define cmdDEL_CHAR         "\b \b"

/* The maximum time to wait for the mutex that guards the UART to become
available. */
#define cmdMAX_MUTEX_WAIT		             pdMS_TO_TICKS( 300 )

#ifndef configCLI_BAUD_RATE
    #define configCLI_BAUD_RATE              115200
#endif

#ifndef configMAX_HISTORY_NUM
    #define  configMAX_HISTORY_NUM           5
#else
    #if (configMAX_HISTORY_NUM < 1)
        #undef configMAX_HISTORY_NUM
        #define  configMAX_HISTORY_NUM       1
    #endif
#endif

#ifndef configMAX_COMPLETION_NUM
    #define configMAX_COMPLETION_NUM         5
#else
    #if (configMAX_COMPLETION_NUM < 1)
        #undef configMAX_COMPLETION_NUM
        #define configMAX_COMPLETION_NUM     1
    #endif
#endif

#ifndef configUSE_ERR_KEY_SEND_RING
    #define configUSE_ERR_KEY_SEND_RING      0
#endif

#ifndef configUSE_SUPPOST_XTERM
    #define configUSE_SUPPOST_XTERM          0
#endif

/*-----------------------------------------------------------*/

/*
 * The task that implements the command console processing.
 */
static void prvUARTCommandConsoleTask( void *pvParameters );
void vUARTCommandConsoleStart( uint16_t usStackSize, UBaseType_t uxPriority );

/*-----------------------------------------------------------*/

/* Const messages output by the command console. */
const char * const pcWelcomeMessage = 
"\
          _______  _______  _______  _______   _____  \r\n\
|\\     /|/ ___   )(  ____ )(  ____ \\(  ___  ) / ___ \\ \r\n\
| )   ( |\\/   )  || (    )|| (    \\/| (   ) |( (   ) )\r\n\
| |   | |    /   )| (____)|| |      | (___) |( (___) |\r\n\
( (   ) )  _/   / |  _____)| |      |  ___  | \\____  |\r\n\
 \\ \\_/ /  /   _/  | (      | |      | (   ) |      ) |\r\n\
  \\   /  (   (__/\\| )      | (____/\\| )   ( |/\\____) )\r\n\
   \\_/   \\_______/|/       (_______/|/     \\|\\______/ \r\n\
2019 by 2014500726@smail.xtu.edu.cn\r\n\
Welcome debugging vexpress_v2p_ca9_project!";
const char * const pcEndOfOutputMessage = "\r\n\033[32mV2PCA9@FreeRTOS>\033[0m";
const char * const pcNewLine = "\r\n";

/* Used to guard access to the UART in case messages are sent to the UART from
more than one task. */
static SemaphoreHandle_t xTxMutex = NULL;

/* The handle to the UART port, which is not used by all ports. */
static xComPortHandle xPort = 0;

static char cHistoryInputString[configMAX_HISTORY_NUM][ cmdMAX_INPUT_SIZE ] = {0};

char cCompletionString[cmdMAX_INPUT_SIZE*configMAX_COMPLETION_NUM];

/*-----------------------------------------------------------*/
typedef char * cli_va_list;
#define cli_va_start(ap,p) (ap = (char *) (&(p)+1))
#define cli_va_arg(ap, type) ((type *) (ap += sizeof(type)))[-1]
#define cli_va_end(ap)

void *pvCliMemcpy(void *dest, const void *src, unsigned int count)
{
    uint8_t * _src = (uint8_t *)src;
    uint8_t * _dst = (uint8_t *)dest;
    for(int i = count-1; i >= 0 ;i--)
    {
        *(_dst+i) = *(_src+i);
    }
    return dest;
}

void vCliItoa(unsigned int n, char *buf)
{
    int i;

    if (n < 10)
    {
        buf[0] = n + '0';
        buf[1] = '\0';
        return;
    }
    vCliItoa(n / 10, buf);

    for (i = 0; buf[i] != '\0'; i++)
        ;

    buf[i] = (n % 10) + '0';

    buf[i + 1] = '\0';
}

int iCliAtoi(char *pstr)
{
    int int_ret = 0;
    int int_sign = 1;

    if (*pstr == '\0')
    {
        return -1;
    }
    while (((*pstr) == ' ') || ((*pstr) == '\n') || ((*pstr) == '\t') || ((*pstr) == '\b'))
    {
        pstr++;
    }

    if (*pstr == '-')
    {
        int_sign = -1;
    }
    if (*pstr == '-' || *pstr == '+')
    {
        pstr++;
    }

    while (*pstr >= '0' && *pstr <= '9')
    {
        int_ret = int_ret * 10 + *pstr - '0';
        pstr++;
    }
    int_ret = int_sign * int_ret;

    return int_ret;
}

void vCliXtoa(unsigned int n, char *buf)
{
    int i;
    if (n < 16)
    {
        if (n < 10)
        {
            buf[0] = n + '0';
        }
        else
        {
            buf[0] = n - 10 + 'a';
        }
        buf[1] = '\0';
        return;
    }
    vCliXtoa(n / 16, buf);

    for (i = 0; buf[i] != '\0'; i++)
        ;

    if ((n % 16) < 10)
    {
        buf[i] = (n % 16) + '0';
    }
    else
    {
        buf[i] = (n % 16) - 10 + 'a';
    }
    buf[i + 1] = '\0';
}

int iCliFtoa(double num, int n,char *buf)
{
    int     sumI;
    float   sumF;
    int     sign = 0;
    int     temp;
    int     count = 0;
    char *p;
    char *pp;

    if(buf == NULL) return -1;
    p = buf;

    /*Is less than 0*/
    if(num < 0)
    {
        sign = 1;
        num = 0 - num;
    }
    sumI = (int)num;
    sumF = num - sumI;

    do
    {
        temp = sumI % 10;
        *(buf++) = temp + '0';
    }while((sumI = sumI /10) != 0);

    if(sign == 1)
    {
        *(buf++) = '-';
    }
    pp = buf;
    pp--;
    while(p < pp)
    {
        *p = *p + *pp;
        *pp = *p - *pp;
        *p = *p -*pp;
        p++;
        pp--;
    }
    *(buf++) = '.';

    do
    {
        temp = (int)(sumF*10);
        *(buf++) = temp + '0';
        if((++count) == n)
            break;
        sumF = sumF*10 - temp;
    }while(!(sumF > -0.000001 && sumF < 0.000001));

    *buf ='\0';
    return 0;
}

int iCliIsDigit(unsigned char c)
{
    if (c >= '0' && c <= '9')
        return 1;
    else
        return 0;
}

void *pvCliMemset(void *s, int c, unsigned int count)
{
    char *xs = (char *)s;

    while (count--)
        *xs++ = c;

    return s;
}


int iCliStrcmp(const char* str1, const char* str2)
{
	int ret = 0;
	while(!(ret=*(unsigned char*)str1-*(unsigned char*)str2) && *str1)
	{
		str1++;
		str2++;
	}
 
	if (ret < 0)
	{
		return -1;
	}
	else if (ret > 0)
	{
		return 1;
	}
	return 0;
}

char *pcCliStrcpy(char *dest, const char *src)
{
    char *tmp = dest;

    while ((*dest++ = *src++) != '\0')
        /* nothing */;
    return tmp;
}

unsigned int uiCliStrlen(const char *s)
{
    const char *sc;

    for (sc = s; *sc != '\0'; ++sc)
        /* nothing */;
    return sc - s;
}

int iCliSprintf(char *str, char *fmt, ...)
{
    int count = 0;
    char c;
    char *s;
    int n;
    double f;

    int index = 0;
    int ret = 2;

    char buf[65];
    char digit[16];
    int num = 0;
    int len = 0;

    pvCliMemset(buf, 0, sizeof(buf));
    pvCliMemset(digit, 0, sizeof(digit));

    cli_va_list ap;

    cli_va_start(ap, fmt);

    while (*fmt != '\0')
    {
        if (*fmt == '%')
        {
            fmt++;
            switch (*fmt)
            {
            case 'd':
            {
                n = cli_va_arg(ap, int);
                if (n < 0)
                {
                    *str = '-';
                    str++;
                    n = -n;
                }
                vCliItoa(n, buf);
                pvCliMemcpy(str, buf, uiCliStrlen(buf));
                str += uiCliStrlen(buf);
                break;
            }
            case 'c':
            {
                c = cli_va_arg(ap, int);
                *str = c;
                str++;

                break;
            }
            case 'x':
            {
                n = cli_va_arg(ap, int);
                vCliXtoa(n, buf);
                pvCliMemcpy(str, buf, uiCliStrlen(buf));
                str += uiCliStrlen(buf);
                break;
            }
            case 'f':
            {
                f = cli_va_arg(ap, double);
                int ndigit = 0;
                float temp = (float)f;
                while(temp != (long)(temp))
                {
                    temp *= 10;
                    ndigit ++;
                }
                iCliFtoa(f, ndigit, buf);
                pvCliMemcpy(str, buf, uiCliStrlen(buf));
                str += uiCliStrlen(buf);
                break;
            }
            case 's':
            {
                s = cli_va_arg(ap, char *);
                pvCliMemcpy(str, s, uiCliStrlen(s));
                str += uiCliStrlen(s);
                break;
            }
            case '%':
            {
                *str = '%';
                str++;

                break;
            }
            case '0':
            {
                index = 0;
                num = 0;
                pvCliMemset(digit, 0, sizeof(digit));

                while (1)
                {
                    fmt++;
                    ret = iCliIsDigit(*fmt);
                    if (ret == 1)
                    {
                        digit[index] = *fmt;
                        index++;
                    }
                    else
                    {
                        num = iCliAtoi(digit);
                        break;
                    }
                }
                switch (*fmt)
                {
                case 'd':
                {
                    n = cli_va_arg(ap, int);
                    if (n < 0)
                    {
                        *str = '-';
                        str++;
                        n = -n;
                    }
                    vCliItoa(n, buf);
                    len = uiCliStrlen(buf);
                    if (len >= num)
                    {
                        pvCliMemcpy(str, buf, uiCliStrlen(buf));
                        str += uiCliStrlen(buf);
                    }
                    else
                    {
                        pvCliMemset(str, '0', num - len);
                        str += num - len;
                        pvCliMemcpy(str, buf, uiCliStrlen(buf));
                        str += uiCliStrlen(buf);
                    }
                    break;
                }
                case 'x':
                {
                    n = cli_va_arg(ap, int);
                    vCliXtoa(n, buf);
                    len = uiCliStrlen(buf);
                    if (len >= num)
                    {
                        pvCliMemcpy(str, buf, len);
                        str += len;
                    }
                    else
                    {
                        pvCliMemset(str, '0', num - len);
                        str += num - len;
                        pvCliMemcpy(str, buf, len);
                        str += len;
                    }
                    break;
                }
                case 's':
                {
                    s = cli_va_arg(ap, char *);
                    len = uiCliStrlen(s);
                    if (len >= num)
                    {
                        pvCliMemcpy(str, s, uiCliStrlen(s));
                        str += uiCliStrlen(s);
                    }
                    else
                    {
                        pvCliMemset(str, '0', num - len);
                        str += num - len;
                        pvCliMemcpy(str, s, uiCliStrlen(s));
                        str += uiCliStrlen(s);
                    }
                    break;
                }
                default:
                    break;
                }
            }
            default:
                break;
            }
        }
        else
        {
            *str = *fmt;
            str++;

            if (*fmt == '\n')
            {
            }
        }
        fmt++;
    }

    cli_va_end(ap);

    return count;
}

__attribute__((unused)) static void vCursorRight(int num)
{
    char cmdCursorRight[6];
    iCliSprintf(cmdCursorRight,"\033[%dD",num);
    vSerialPutString( xPort, (signed char *)cmdCursorRight, ( unsigned short ) uiCliStrlen( cmdCursorRight ) );	
}
/*-----------------------------------------------------------*/

__attribute__((unused)) static void vCursorLeft(int num)
{
    char cmdCursorLeft[6];
    iCliSprintf(cmdCursorLeft,"\033[%dC",num);
    vSerialPutString( xPort, (signed char *)cmdCursorLeft, ( unsigned short ) uiCliStrlen( cmdCursorLeft ) );	
}
/*-----------------------------------------------------------*/

void vUARTCommandConsoleInit( void )
{
    /* Initialise the UART. */
    xPort = xSerialPortInitMinimal( configCLI_BAUD_RATE, cmdQUEUE_LENGTH );

    /* Send the welcome message. */
	vSerialPutString( xPort, ( signed char * ) pcNewLine, ( unsigned short ) uiCliStrlen( pcNewLine ) );
    vSerialPutString( xPort, ( signed char * ) pcWelcomeMessage, ( unsigned short ) uiCliStrlen( pcWelcomeMessage ) );
    vSerialPutString( xPort, ( signed char * ) pcEndOfOutputMessage, ( unsigned short ) uiCliStrlen( pcEndOfOutputMessage ) );
}
/*-----------------------------------------------------------*/

void vUARTCommandConsoleStart( uint16_t usStackSize, UBaseType_t uxPriority )
{
    /* Create the semaphore used to access the UART Tx. */
    xTxMutex = xSemaphoreCreateMutex();
    configASSERT( xTxMutex );

    /* Create that task that handles the console itself. */
    xTaskCreate( 	prvUARTCommandConsoleTask,	/* The task that implements the command console. */
                    "CLI",						/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
                    usStackSize,				/* The size of the stack allocated to the task. */
                    NULL,						/* The parameter is not used, so NULL is passed. */
                    uxPriority,					/* The priority allocated to the task. */
                    NULL );						/* A handle is not required, so just pass NULL. */
}
/*-----------------------------------------------------------*/
static void prvUARTCommandConsoleTask( void *pvParameters )
{
signed char cRxedChar;
uint8_t ucInputIndex = 0;
char *pcOutputString;
static char cInputString[ cmdMAX_INPUT_SIZE ];
static char cInputStringBack[ cmdMAX_INPUT_SIZE ];
static uint8_t ucInputStringBackIndex = 0;
static uint8_t ucHistoryInputStringIndex = 0;
static uint8_t ucCtlInputStringIndex = 1;
BaseType_t xReturned;
int retcmp;
uint8_t ucFindCompletionNum = 0;
uint8_t ucCompletionlen;
uint8_t ucTempIndex;

    ( void ) pvParameters;

    /* Obtain the address of the output buffer.  Note there is no mutual
    exclusion on this buffer as it is assumed only one command console interface
    will be used at any one time. */
    pcOutputString = FreeRTOS_CLIGetOutputBuffer();

    for( ;; )
    {
        /* Wait for the next character.  The while loop is used in case
        INCLUDE_vTaskSuspend is not set to 1 - in which case portMAX_DELAY will
        be a genuine block time rather than an infinite block time. */
        while( xSerialGetChar( xPort, &cRxedChar, portMAX_DELAY ) != pdPASS );

        /* Ensure exclusive access to the UART Tx. */
        if( xSemaphoreTake( xTxMutex, cmdMAX_MUTEX_WAIT ) == pdPASS )
        {
            /* Was it the end of the line? */
            if( cRxedChar == '\n' || cRxedChar == '\r' )
            {
                /* Echo the character back. */
                xSerialPutChar( xPort, cRxedChar, portMAX_DELAY );

                /* Just to space the output from the input. */
                vSerialPutString( xPort, ( signed char * ) pcNewLine, ( unsigned short ) uiCliStrlen( pcNewLine ) );

                for(int i = ucInputStringBackIndex-1; i >= 0; i--)
                {
                    cInputString[ucInputIndex] = cInputStringBack[i];
                    ucInputIndex++;
                }
                cInputString[ucInputIndex] = '\0';
                ucInputStringBackIndex = 0;

                if( ucInputIndex != 0 )
                {
                    /* Pass the received command to the command interpreter.  The
                    command interpreter is called repeatedly until it returns
                    pdFALSE	(indicating there is no more output) as it might
                    generate more than one string. */
                    do
                    {
                        /* Get the next output string from the command interpreter. */
                        xReturned = FreeRTOS_CLIProcessCommand( cInputString, pcOutputString, configCOMMAND_INT_MAX_OUTPUT_SIZE );

                        /* Write the generated string to the UART. */
                        vSerialPutString( xPort, ( signed char * ) pcOutputString, ( unsigned short ) uiCliStrlen( pcOutputString ) );
                    } while( xReturned != pdFALSE );

                    /* All the strings generated by the input command have been
                    sent.  Clear the input string ready to receive the next command.
                    Remember the command that was just processed first in case it is
                    to be processed again. */
                    if(ucHistoryInputStringIndex >= 1)
                    {
                        retcmp = iCliStrcmp( cInputString, cHistoryInputString[ucHistoryInputStringIndex-1] );
                    }
                    else
                    {
                        retcmp = iCliStrcmp( cInputString, cHistoryInputString[configMAX_HISTORY_NUM-1] );
                    }
                    if(0 != retcmp)
                    {
                        pcCliStrcpy( cHistoryInputString[ucHistoryInputStringIndex], cInputString );
                        ucHistoryInputStringIndex++;
                        if(ucHistoryInputStringIndex >= configMAX_HISTORY_NUM)
                        {
                            ucHistoryInputStringIndex = 0;
                        }
                    }
                    vSerialPutString( xPort, ( signed char * ) pcEndOfOutputMessage, ( unsigned short ) uiCliStrlen( pcEndOfOutputMessage ) );
                }
                else
                {
                    vSerialPutString( xPort, ( signed char * ) pcEndOfOutputMessage+2, ( unsigned short ) uiCliStrlen( pcEndOfOutputMessage+2 ) );
                }
            
                ucInputIndex = 0;
                ucCtlInputStringIndex = 1;
                memset( cInputString, 0x00, cmdMAX_INPUT_SIZE );
            }
            else
            {
                if( cRxedChar == '\r' )
                {
                    /* Echo the character back. */
                    xSerialPutChar( xPort, cRxedChar, portMAX_DELAY );
                }
                else if( ( cRxedChar == '\b' ) || ( cRxedChar == cmdASCII_DEL ) )
                {   
                    /* BackSpace */
                    if( ucInputIndex > 0 )
                    {
                        vSerialPutString( xPort, (signed char *)"\033[K", ( unsigned short ) uiCliStrlen( "\033[K" ) );	
                        ucInputIndex--;
                        cInputString[ ucInputIndex ] = '\0';
                        vSerialPutString( xPort, (signed char *)cmdDEL_CHAR, ( unsigned short ) uiCliStrlen( cmdDEL_CHAR ) );						
                        
                        for(int i = ucInputStringBackIndex-1; i >= 0; i--)
                        {
                            xSerialPutChar( xPort, cInputStringBack[i],portMAX_DELAY);
                        }
                        if(ucInputStringBackIndex > 0)
                        {
                            vCursorRight(ucInputStringBackIndex);
                        }
                    }
                    else
                    {
                        #if configUSE_ERR_KEY_SEND_RING
                        xSerialPutChar( xPort, '\a', portMAX_DELAY );
                        #endif
                    }
                }
                else if( cRxedChar == '\t' )
                {
                    /* Tab */
                    ucInputStringBackIndex = 0;
                    vSerialPutString( xPort, (signed char *)"\033[K", ( unsigned short ) uiCliStrlen( "\033[K" ) );	

                    ucFindCompletionNum = configMAX_COMPLETION_NUM;
                    FreeRTOS_CLICompletionCommand(cInputString,ucInputIndex,cCompletionString,&ucFindCompletionNum,&ucCompletionlen);
                    if(ucFindCompletionNum == 0)
                    {
                        #if configUSE_ERR_KEY_SEND_RING
                        xSerialPutChar( xPort, '\a', portMAX_DELAY );
                        #endif
                    }
                    else if(ucFindCompletionNum == 1)
                    {
                        xSerialPutChar( xPort, '4', portMAX_DELAY );
                        if(ucInputIndex > 0)
                        {
                            vCursorRight(ucInputIndex);
                            vSerialPutString( xPort, (signed char *)"\033[K", ( unsigned short ) uiCliStrlen( "\033[K" ) );	
                        }

                        pcCliStrcpy( cInputString, cCompletionString );
                        vSerialPutString(xPort, (signed char *)cInputString, (unsigned short) uiCliStrlen(cInputString));
                        ucInputIndex = uiCliStrlen(cInputString);
                    }
                    else
                    {
                        vSerialPutString(xPort,(signed char *)pcNewLine, (unsigned short) uiCliStrlen(pcNewLine));
                        vSerialPutString(xPort,(signed char *)cCompletionString, (unsigned short) uiCliStrlen(cCompletionString));
                        if(ucFindCompletionNum >= configMAX_COMPLETION_NUM)
                        {
                            vSerialPutString( xPort, (signed char *)"\r\nNo more matches can be displayed, Please add configMAX_COMPLETION_NUM val!", ( unsigned short ) uiCliStrlen( "\r\nNo more matches can be displayed, Please add configMAX_COMPLETION_NUM val!" ) );
                        }

                        vSerialPutString( xPort, ( signed char * ) pcEndOfOutputMessage, ( unsigned short ) uiCliStrlen( pcEndOfOutputMessage ) );
                        if(ucInputIndex != 0)
                        {
                            cCompletionString[ucCompletionlen] = '\0';
                            pcCliStrcpy( cInputString, cCompletionString );
                            vSerialPutString(xPort, (signed char *)cInputString, (unsigned short) uiCliStrlen(cInputString));
                            ucInputIndex = uiCliStrlen(cInputString);
                        }
                    }
                    
                }
                else if( cRxedChar == 0x1b )
                {
                    while( xSerialGetChar( xPort, &cRxedChar, portMAX_DELAY ) != pdPASS );
                    if( cRxedChar == 0x5b )
                    {
                        while( xSerialGetChar( xPort, &cRxedChar, portMAX_DELAY ) != pdPASS );
                        switch(cRxedChar)
                        {
                            /* Top */
                            case 'A':
                            {
                                if(ucInputIndex > 0)
                                {
                                    vCursorRight(ucInputIndex);
                                    vSerialPutString( xPort, (signed char *)"\033[K", ( unsigned short ) uiCliStrlen( "\033[K" ) );	
                                }

                                if(ucHistoryInputStringIndex >= ucCtlInputStringIndex)
                                {
                                    pcCliStrcpy( cInputString, cHistoryInputString[ucHistoryInputStringIndex-ucCtlInputStringIndex] );
                                }
                                else
                                {
                                    pcCliStrcpy( cInputString,  cHistoryInputString[configMAX_HISTORY_NUM-ucCtlInputStringIndex+ucHistoryInputStringIndex] );
                                }
                                vSerialPutString(xPort, (signed char *)cInputString, (unsigned short) uiCliStrlen(cInputString));
                                ucInputIndex = uiCliStrlen(cInputString);
                                ucCtlInputStringIndex++;
                                if(ucCtlInputStringIndex > configMAX_HISTORY_NUM)
                                {
                                    ucCtlInputStringIndex = 1;
                                }
                                ucInputStringBackIndex = 0;
                                break;
                            }
                            case 'B':
                            {
                                if(ucInputIndex > 0)
                                {
                                    vCursorRight(ucInputIndex);
                                    vSerialPutString( xPort, (signed char *)"\033[K", ( unsigned short ) uiCliStrlen( "\033[K" ) );	
                                }

                                if(ucHistoryInputStringIndex >= ucCtlInputStringIndex)
                                {
                                    pcCliStrcpy( cInputString, cHistoryInputString[ucHistoryInputStringIndex-ucCtlInputStringIndex] );
                                }
                                else
                                {
                                    pcCliStrcpy( cInputString,  cHistoryInputString[configMAX_HISTORY_NUM-ucCtlInputStringIndex+ucHistoryInputStringIndex] );
                                }
                                vSerialPutString(xPort, (signed char *)cInputString, (unsigned short) uiCliStrlen(cInputString));
                                ucInputIndex = uiCliStrlen(cInputString);
                                ucCtlInputStringIndex--;
                                if(ucCtlInputStringIndex < 1)
                                {
                                    ucCtlInputStringIndex = configMAX_HISTORY_NUM;
                                }
                                ucInputStringBackIndex = 0;
                                break;
                            }
                            /* Right */
                            case 'C':
                            {
                                if(ucInputStringBackIndex > 0)
                                {
                                    xSerialPutChar( xPort, cInputStringBack[ucInputStringBackIndex-1], portMAX_DELAY );
                                    cInputString[ ucInputIndex] = cInputStringBack[ucInputStringBackIndex-1];
                                    ucInputStringBackIndex--;
                                    ucInputIndex++;
                                }
                                else
                                {
                                    #if configUSE_ERR_KEY_SEND_RING
                                    xSerialPutChar( xPort, '\a', portMAX_DELAY );
                                    #endif
                                }
                                break;
                            }
                            /* Left */ 
                            case 'D':
                            {
                                if(ucInputIndex > 0)
                                {
                                    xSerialPutChar( xPort, '\b', portMAX_DELAY );
                                    cInputStringBack[ucInputStringBackIndex] = cInputString[ ucInputIndex-1];
                                    ucInputStringBackIndex++;
                                    cInputString[ ucInputIndex ] = '\0';
                                    ucInputIndex--;
                                }
                                else
                                {
                                    #if configUSE_ERR_KEY_SEND_RING
                                    xSerialPutChar( xPort, '\a', portMAX_DELAY );
                                    #endif
                                }
                                break;
                            }
                            /* Delete */
                            case '3':
                            {
                                while( xSerialGetChar( xPort, &cRxedChar, portMAX_DELAY ) != pdPASS );
                                if( ucInputStringBackIndex > 0 )
                                {
                                    vSerialPutString( xPort, (signed char *)"\033[K", ( unsigned short ) uiCliStrlen( "\033[K" ) );	

                                    cInputStringBack[ ucInputStringBackIndex ] = '\0';
                                    ucInputStringBackIndex--;
                                    
                                    for(int i = ucInputStringBackIndex-1; i >= 0; i--)
                                    {
                                        xSerialPutChar( xPort, cInputStringBack[i],portMAX_DELAY);
                                    }
                                    if(ucInputStringBackIndex > 0)
                                    {
                                        vCursorRight(ucInputStringBackIndex);
                                    }
                                }
                                else
                                {
                                    #if configUSE_ERR_KEY_SEND_RING
                                    xSerialPutChar( xPort, '\a', portMAX_DELAY );
                                    #endif
                                }
                                break;
                            }
                            /* Home */
                            case '1':
                            {
                                while( xSerialGetChar( xPort, &cRxedChar, portMAX_DELAY ) != pdPASS );
                            }
                            #if configUSE_SUPPOST_XTERM
                            case 'H':
                            #endif
                            {
                                if(ucInputIndex > 0)
                                {
                                    vCursorRight(ucInputIndex);
                                    while(ucInputIndex > 0)
                                    {
                                        cInputStringBack[ucInputStringBackIndex] = cInputString[ ucInputIndex-1];
                                        ucInputStringBackIndex++;
                                        cInputString[ ucInputIndex ] = '\0';
                                        ucInputIndex--;
                                    }
                                }
                                else
                                {
                                    #if configUSE_ERR_KEY_SEND_RING
                                    xSerialPutChar( xPort, '\a', portMAX_DELAY );
                                    #endif
                                }
                                break;
                            }
                            /* End */
                            case '4':
                            {
                                while( xSerialGetChar( xPort, &cRxedChar, portMAX_DELAY ) != pdPASS );
                            }
                            #if configUSE_SUPPOST_XTERM
                            case 'F':
                            #endif
                            {
                                if(ucInputStringBackIndex > 0)
                                {
                                    ucTempIndex = ucInputIndex;
                                    while(ucInputStringBackIndex > 0)
                                    {
                                        cInputString[ ucInputIndex] = cInputStringBack[ucInputStringBackIndex-1];
                                        ucInputStringBackIndex--;
                                        ucInputIndex++;
                                    }
                                    vSerialPutString( xPort, (signed char *)cInputString+ucTempIndex, ( unsigned short ) uiCliStrlen( cInputString+ucTempIndex ) );	
                                }
                                else
                                {
                                    #if configUSE_ERR_KEY_SEND_RING
                                    xSerialPutChar( xPort, '\a', portMAX_DELAY );
                                    #endif
                                }
                                break;
                            }
                            default:
                            {
                                #if configUSE_ERR_KEY_SEND_RING
                                xSerialPutChar( xPort, '\a', portMAX_DELAY );
                                #endif
                                break;
                            }
                        }
                    }
                }
                else
                {
                    /* Echo the character back. */
                    xSerialPutChar( xPort, cRxedChar, portMAX_DELAY );
                    
                    for(int i = ucInputStringBackIndex-1; i >= 0; i--)
                    {
                        xSerialPutChar( xPort, cInputStringBack[i],portMAX_DELAY);
                    }
                    if(ucInputStringBackIndex > 0)
                    {
                        vCursorRight(ucInputStringBackIndex);
                    }
                    /* A character was entered.  Add it to the string entered so
                    far.  When a \n is entered the complete	string will be
                    passed to the command interpreter. */
                    if( ( cRxedChar >= ' ' ) && ( cRxedChar <= '~' ) )
                    {
                        if( ucInputIndex < cmdMAX_INPUT_SIZE )
                        {
                            cInputString[ ucInputIndex ] = cRxedChar;
                            ucInputIndex++;
                        }
                    }
                }
            }

            /* Must ensure to give the mutex back. */
            xSemaphoreGive( xTxMutex );
        }
    }
}
/*-----------------------------------------------------------*/

void vOutputString( const char * const pcMessage )
{
    if( xSemaphoreTake( xTxMutex, cmdMAX_MUTEX_WAIT ) == pdPASS )
    {
        vSerialPutString( xPort, ( signed char * ) pcMessage, ( unsigned short ) uiCliStrlen( pcMessage ) );
        xSemaphoreGive( xTxMutex );
    }
}
/*-----------------------------------------------------------*/
