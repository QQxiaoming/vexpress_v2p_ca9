#include <stdarg.h>
#include <stdio.h>
#include "debug_log.h"
#include "v2p_ca9_system.h"
#include <stdbool.h>
#include "v2p_uart.h"

#if CONFIG_DEBUG_LOG_EN

char _getchar(void)
{
    char ch;
    pl01x_getc(PERIPH_PA_TO_VA(V2P_CA9_MP_UART0_BASE),&ch,0xFFFFFFFF);
    return ch;
}

void _putchar(char ch)
{
    if ('\n' == ch)
    {
        pl01x_putc(PERIPH_PA_TO_VA(V2P_CA9_MP_UART0_BASE),'\r');
    }
    pl01x_putc(PERIPH_PA_TO_VA(V2P_CA9_MP_UART0_BASE),ch);
    return;
}

/**
  * @功能:字符串打印
  * @参数: 1.str字符串的首地址
  * @返回值:打印的字符个数
  */
int _puts(char *str)
{
    int counter = 0;
    if (!str)
    {
        return 0;
    }
    while (*str && (counter < UART_LOG_BUFF_SIZE))
    {
        _putchar(*str++);
        counter++;
    }
    return counter;
}

#if 0
static char printk_string[UART_LOG_BUFF_SIZE] = {0};
int _printf(char* format,...)
{
    va_list args;
    int plen;
    va_start(args, format);
    plen = vsnprintf(printk_string, sizeof(printk_string)/sizeof(char) - 1, format, args);
    _puts(printk_string);
    va_end(args);
    return plen;
}
#else

#include <stdio.h>

typedef char * myva_list;
#define myva_start(ap,p) (ap = (char *) (&(p)+1))
#define myva_arg(ap, type) ((type *) (ap += sizeof(type)))[-1]
#define myva_end(ap)

char *mystrcpy(char *dest, const char *src);
char *mystrcat(char *dest, const char *src);
static unsigned int mystrlen(const char *s);
static void *mymemset(void *s, int c, unsigned int count);
static void myitoa(unsigned int n, char *buf);
static int myatoi(char *pstr);
static void myxtoa(unsigned int n, char *buf);
static int myftoa(double num, int n,char *buf);
static int myisDigit(unsigned char c);

static void *mymemcpy(void *dest, const void *src, unsigned int count)
{
    uint8_t * _src = (uint8_t *)src;
    uint8_t * _dst = (uint8_t *)dest;
    for(int i = count-1; i >= 0 ;i--)
    {
        *(_dst+i) = *(_src+i);
    }
    return dest;
}

/*
 * 功能：整型(int) 转化成 字符型(char)
 * 注意：不用 % / 符号的话，只能正确打印:0...9的数字对应的字符'0'...'9'
 */
static void myitoa(unsigned int n, char *buf)
{
    int i;

    if (n < 10)
    {
        buf[0] = n + '0';
        buf[1] = '\0';
        return;
    }
    myitoa(n / 10, buf);

    for (i = 0; buf[i] != '\0'; i++)
        ;

    buf[i] = (n % 10) + '0';

    buf[i + 1] = '\0';
}

/*
 * 功能：字符型(char) 转化成 整型(int)
 */
static int myatoi(char *pstr)
{
    int int_ret = 0;
    int int_sign = 1; //正负号标示 1:正数 -1:负数

    if (*pstr == '\0') //判断指针是否为空
    {
        return -1;
    }
    while (((*pstr) == ' ') || ((*pstr) == '\n') || ((*pstr) == '\t') || ((*pstr) == '\b'))
    {
        pstr++; //跳过前面的空格字符
    }

    /*
        * 判断正负号
        * 如果是正号，指针指向下一个字符
        * 如果是符号，把符号标记为Integer_sign置-1，然后再把指针指向下一个字符
        */
    if (*pstr == '-')
    {
        int_sign = -1;
    }
    if (*pstr == '-' || *pstr == '+')
    {
        pstr++;
    }

    while (*pstr >= '0' && *pstr <= '9') //把数字字符串逐个转换成整数，并把最后转换好的整数赋给Ret_Integer
    {
        int_ret = int_ret * 10 + *pstr - '0';
        pstr++;
    }
    int_ret = int_sign * int_ret;

    return int_ret;
}

/*
 * 功能：16进制字(0x) 转化成 字符型(char)
 * 注意：不用 % / 符号的话，只能正确打印，0...9..15的数字,对应的'0'...'9''A'...'F'
 * 注意：由于编译问题，这个函数，暂时由uart_sendByte_hex()函数替代
 */
static void myxtoa(unsigned int n, char *buf)
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
    myxtoa(n / 16, buf);

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

static int myftoa(double num, int n,char *buf)
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
    sumI = (int)num;    //sumI is the part of int
    sumF = num - sumI;  //sumF is the part of float

    /*Int ===> String*/
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
    *(buf++) = '.';     //point

    /*Float ===> String*/
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

/*
 * 判断一个字符是否数字
 */
static int myisDigit(unsigned char c)
{
    if (c >= '0' && c <= '9')
        return 1;
    else
        return 0;
}

/*
 * 判断一个字符是否英文字母
 */
int myisLetter(unsigned char c)
{
    if (c >= 'a' && c <= 'z')
        return 1;
    else if (c >= 'A' && c <= 'Z')
        return 1;
    else
        return 0;
}

/**
 * mymemset - Fill a region of memory with the given value
 * @s: Pointer to the start of the area.
 * @c: The byte to fill the area with
 * @count: The size of the area.
 *
 * Do not use mymemset() to access IO space, use memset_io() instead.
 */
static void *mymemset(void *s, int c, unsigned int count)
{
    char *xs = (char *)s;

    while (count--)
        *xs++ = c;

    return s;
}

/**
 * mystrcpy - Copy a %NUL terminated string
 * @dest: Where to copy the string to
 * @src: Where to copy the string from
 */
char *mystrcpy(char *dest, const char *src)
{
    char *tmp = dest;

    while ((*dest++ = *src++) != '\0')
        /* nothing */;
    return tmp;
}

/**
 * mystrlen - Find the length of a string
 * @s: The string to be sized
 */
static unsigned int mystrlen(const char *s)
{
    const char *sc;

    for (sc = s; *sc != '\0'; ++sc)
        /* nothing */;
    return sc - s;
}

/**
 * mystrcat - Append one %NUL-terminated string to another
 * @dest: The string to be appended to
 * @src: The string to append to it
 */
char *mystrcat(char *dest, const char *src)
{
    char *tmp = dest;

    while (*dest)
        dest++;
    while ((*dest++ = *src++) != '\0')
        ;

    return tmp;
}

static char printk_string[UART_LOG_BUFF_SIZE] = {0};
/*
 * 功能：格式化打印一个字符串
 * 参数：格式化的字符串
 * 注意：这个是简易版本 (%02x 完成)
 * %-3s不行， %f也不行， %X不行
 */
int _printf(char *fmt, ...)
{
    char *str = printk_string;
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

    mymemset(printk_string, 0, UART_LOG_BUFF_SIZE);
    mymemset(buf, 0, sizeof(buf));
    mymemset(digit, 0, sizeof(digit));

    myva_list ap;

    myva_start(ap, fmt);

    while (*fmt != '\0')
    {
        if (*fmt == '%')
        {
            fmt++;
            switch (*fmt)
            {
            case 'd': /*整型*/
            {
                n = myva_arg(ap, int);
                if (n < 0)
                {
                    *str = '-';
                    str++;
                    n = -n;
                }
                myitoa(n, buf);
                mymemcpy(str, buf, mystrlen(buf));
                str += mystrlen(buf);
                break;
            }
            case 'c': /*字符型*/
            {
                c = myva_arg(ap, int);
                *str = c;
                str++;

                break;
            }
            case 'x': /*16进制*/
            {
                n = myva_arg(ap, int);
                myxtoa(n, buf);
                mymemcpy(str, buf, mystrlen(buf));
                str += mystrlen(buf);
                break;
            }
            case 'f': /*单精度浮点*/
            {
                f = myva_arg(ap, double);
                int ndigit = 0;
                float temp = (float)f;
                while(temp != (long)(temp))
                {
                    temp *= 10;
                    ndigit ++;
                }
                myftoa(f, ndigit, buf);
                mymemcpy(str, buf, mystrlen(buf));
                str += mystrlen(buf);
                break;
            }
            case 's': /*字符串*/
            {
                s = myva_arg(ap, char *);
                mymemcpy(str, s, mystrlen(s));
                str += mystrlen(s);
                break;
            }
            case '%': /*输出%*/
            {
                *str = '%';
                str++;

                break;
            }
            case '0': /*位不足的左补0*/
            {
                index = 0;
                num = 0;
                mymemset(digit, 0, sizeof(digit));

                while (1)
                {
                    fmt++;
                    ret = myisDigit(*fmt);
                    if (ret == 1) //是数字
                    {
                        digit[index] = *fmt;
                        index++;
                    }
                    else
                    {
                        num = myatoi(digit);
                        break;
                    }
                }
                switch (*fmt)
                {
                case 'd': /*整型*/
                {
                    n = myva_arg(ap, int);
                    if (n < 0)
                    {
                        *str = '-';
                        str++;
                        n = -n;
                    }
                    myitoa(n, buf);
                    len = mystrlen(buf);
                    if (len >= num)
                    {
                        mymemcpy(str, buf, mystrlen(buf));
                        str += mystrlen(buf);
                    }
                    else
                    {
                        mymemset(str, '0', num - len);
                        str += num - len;
                        mymemcpy(str, buf, mystrlen(buf));
                        str += mystrlen(buf);
                    }
                    break;
                }
                case 'x': /*16进制*/
                {
                    n = myva_arg(ap, int);
                    myxtoa(n, buf);
                    len = mystrlen(buf);
                    if (len >= num)
                    {
                        mymemcpy(str, buf, len);
                        str += len;
                    }
                    else
                    {
                        mymemset(str, '0', num - len);
                        str += num - len;
                        mymemcpy(str, buf, len);
                        str += len;
                    }
                    break;
                }
                case 's': /*字符串*/
                {
                    s = myva_arg(ap, char *);
                    len = mystrlen(s);
                    if (len >= num)
                    {
                        mymemcpy(str, s, mystrlen(s));
                        str += mystrlen(s);
                    }
                    else
                    {
                        mymemset(str, '0', num - len);
                        str += num - len;
                        mymemcpy(str, s, mystrlen(s));
                        str += mystrlen(s);
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

    myva_end(ap);
    
    
    _puts(printk_string);

    return count;
}
#endif
#else
int _printf(char *format, ...)
{
    return 0;
}
#endif
