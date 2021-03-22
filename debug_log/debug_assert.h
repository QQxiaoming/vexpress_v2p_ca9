#ifndef _DEBUG_ASSERT_H_
#define _DEBUG_ASSERT_H_

#include "debug_log.h"
#include "cmsis_compiler.h"

#if CONFIG_DEBUG_EN
#define DEBUG_ASSERT(x,msg)                                                                                                    \
    if( ( x ) == 0 )                                                                                                        \
    {                                                                                                                       \
        debug_logerr(LOG_ASSERT,"%s",msg);                                                                                     \
        debug_logassert(LOG_ASSERT,"ASSERT ERROR : file \"%s\" Line %d\n",__FILE__,__LINE__); \
        while(1)  __BKPT(0);                                                                                                \
    }
#else
#define DEBUG_ASSERT(x,msg) do{}while(0)
#endif

#endif /* _DEBUG_ASSERT_H_ */
