#ifndef _DRV_PEF24628E_OSMAP_H
#define _DRV_PEF24628E_OSMAP_H
/******************************************************************************

                               Copyright (c) 2011
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

/* ==========================================================================
   Description : IFX OS mapping of the used driver OS functions.
   ========================================================================== */

/** \file
   Mapping of the OS functions used by the  driver.
*/

#ifdef __cplusplus
extern "C"
{
#endif

/* ============================================================================
   Includes
   ========================================================================= */

#if defined(LINUX)
#  ifdef __KERNEL__
#     include <linux/kernel.h>
#  endif

#elif defined(VXWORKS)
#  include <vxworks.h>
#  include <stdio.h>
#  include <stdlib.h>
#  include <string.h>
#  include <ioctl.h>
#  include <iosLib.h>

#elif defined(ECOS)
#  include <stdio.h>
#  include <stdlib.h>
#  include <time.h>
#  include <string.h>
#  include <sys/ioctl.h>

#elif defined(WIN32)
#  include <stdio.h>
#  include <string.h>

#else
//#  error "PEF24628E driver - no OS specified"
#endif

#include "ifx_types.h"
#include "lib_sysdep.h"

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
//#include "ifxos_interrupt.h"

/* ============================================================================
   Function map - Standard Functions.
   ========================================================================= */
#define SOC4E_SPrintf                    sprintf
#define SOC4E_StrNCpy                    strncpy
#define SOC4E_StrLen                     strlen
#define SOC4E_MemSet                     memset
#define SOC4E_MemCpy                     memcpy
#define SOC4E_MemCmp                     memcmp

/* ============================================================================
   common defines - ifxos_common.h"
   ========================================================================= */
//#include "ifxos_linux_drv.h"
//#include "ifxos_common.h"

//#define SOC4E_BYTE_ORDER                 IFXOS_BYTE_ORDER
//#define SOC4E_LITTLE_ENDIAN              IFXOS_LITTLE_ENDIAN
//#define SOC4E_BIG_ENDIAN                 IFXOS_BIG_ENDIAN


/* ============================================================================
   Function map - IFXOS Printout and debug.
   ========================================================================= */
//#include "ifxos_print.h"
//#include "ifxos_debug.h"

#define SOC4E_CRLF                       "\r\n"
#define SOC4E_PRINT_INT_DBG              printf

#if 0
#if defined(LINUX)
/* Use clean printk in the trace */
#  define DRV_PEF_24628E_DBG_TRC_LINUX_USE_PRINTK 1
#else
#  define DRV_PEF_24628E_DBG_TRC_LINUX_USE_PRINTK 0
#endif
#else
#define DRV_PEF_24628E_DBG_TRC_LINUX_USE_PRINTK 0
#endif

#if (DRV_PEF_24628E_DBG_TRC_LINUX_USE_PRINTK == 1)
   #define SOC4E_PRINT_USR_DBG              printk
#else
   #define SOC4E_PRINT_USR_DBG              printf
#endif

//#define SOC4E_PRINT_INT_ERR              IFXOS_ERR_PRINT_INT
//#define SOC4E_PRINT_USR_ERR              IFXOS_ERR_PRINT_USR
#define TRACE                            IFXOS_PRN_USR_DBG_NL
#define LOG                              IFXOS_PRN_INT_DBG_NL

#define SOC4E_PRN_USR_MODULE_DECL        IFXOS_PRN_USR_MODULE_DECL
#define SOC4E_PRN_INT_MODULE_DECL        IFXOS_PRN_INT_MODULE_DECL

#define SOC4E_PRN_USR_LEVEL_SET          IFXOS_PRN_USR_LEVEL_SET
#define SOC4E_PRN_INT_LEVEL_SET          IFXOS_PRN_INT_LEVEL_SET

#define SOC4E_PRN_USR_MODULE_CREATE      IFXOS_PRN_USR_MODULE_CREATE
#define SOC4E_PRN_INT_MODULE_CREATE      IFXOS_PRN_INT_MODULE_CREATE

#define DBG_LEVEL_LOW                    IFXOS_PRN_LEVEL_LOW
#define DBG_LEVEL_NORMAL                 IFXOS_PRN_LEVEL_NORMAL
#define DBG_LEVEL_HIGH                   IFXOS_PRN_LEVEL_HIGH
#define DBG_LEVEL_OFF                    IFXOS_PRN_LEVEL_OFF

#define SetTraceLevel                    IFXOS_PRN_USR_LEVEL_SET
#define SetLogLevel                      IFXOS_PRN_INT_LEVEL_SET

/*
   Function map - Driver User Space data exchange.
*/
/* ============================================================================
   Function map - Driver User Space data exchange.
   ========================================================================= */
//#include "ifxos_copy_user_space.h"

//#define SOC4E_CopyFromUser               IFXOS_CpyFromUser

/* ============================================================================
   Function map - Memory Functions.
   ========================================================================= */
//#include "ifxos_memory_alloc.h"

#define SOC4E_MemAlloc                   malloc
#define SOC4E_MemFree                    free


/* ============================================================================
   Function map - Time and Wait Functions and Defines.
   ========================================================================= */
//#include "ifxos_time.h"

#define SOC4E_timeMS_t                   IFX_timeMS_t

#define SOC4E_USecSleep                  IFXOS_USecSleep
#define SOC4E_MSecSleep                  IFXOS_MSecSleep
//#define SOC4E_ElapsedTimeMSecGet         IFXOS_ElapsedTimeMSecGet
//#define SOC4E_SysTimeGet				 IFXOS_SysTimeGet
//#define SOC4E_ElapsedTimeMSecGet 		 IFXOS_ElapsedTimeMSecGet

/* ============================================================================
   Function map - driver select part (only for types, not used).
   ========================================================================= */

//#include "ifxos_select.h"

#define SOC4E_selectQueue_t              IFXOS_drvSelectQueue_t
#define SOC4E_select_OSArg_t             IFXOS_drvSelectOSArg_t
#define SOC4E_selectTable_t              IFXOS_drvSelectTable_t
#define SOC4E_SelectQueueInit            IFXOS_DrvSelectQueueInit
#define SOC4E_SelectQueueWakeUp          IFXOS_DrvSelectQueueWakeUp
#define SOC4E_SelectQueueAddTask         IFXOS_DrvSelectQueueAddTask
#define SOC4E_SEL_WAKEUP_TYPE_RD         IFXOS_DRV_SEL_WAKEUP_TYPE_RD

//#include "ifxos_event.h"
#define SOC4E_event_t                    IFXOS_event_t
#define SOC4E_EventInit                  IFXOS_EventInit  
#define SOC4E_EventDelete                IFXOS_EventDelete
#define SOC4E_EventWakeUp                IFXOS_EventWakeUp
#define SOC4E_EventWait_timeout(_event_, _time_to_wait_) IFXOS_EventWait(_event_, _time_to_wait_, IFX_NULL)

/* ============================================================================
   Function map - interrupt control part 
   ========================================================================= */
//#include <linux/interrupt.h>

#define SOC4E_IRQ_ENABLE				 IFXOS_IRQ_ENABLE
#define SOC4E_IRQ_DISABLE				 IFXOS_IRQ_DISABLE

#ifdef __cplusplus
}
#endif

#endif   /* _DRV_PEF24628E_OSMAP_H */
