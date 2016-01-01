/******************************************************************************

                               Copyright (c) 2011
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

/**
   \file
   This file contains the defines, the structures declarations
                 the tables declarations and the global functions declarations.
   \remark
   Use compiler switch ENABLE_TRACE for trace output, for debugging purposes.
   Compiler switch for OS is needed. Use LINUX for linux and VXWORKS for
   VxWorks..
*/

#ifndef _DRV_PEF24628E_DBGTRACE_CONFIG_H_
#define _DRV_PEF24628E_DBGTRACE_CONFIG_H_

/** enable the #define DRV_DEBUG_TRACE_ENABLE to compile with driver integrated
    trace history */
/*#define DRV_DEBUG_TRACE_ENABLE */

#ifdef DRV_DEBUG_TRACE_ENABLE

/* AUTOINIT: Debug trace buffer automatically initialized during initialization of 
   driver in OS 
   Without autosinit, DBG trace has to be started manually */
#define DRV_PEF_24628E_DBG_TRC_AUTOINIT 1

/* AUTOSTART: Debug trace auto-starts tracing with initialization of driver in OS 
   Without autostart, DBG trace has to be started manually */
#define DRV_PEF_24628E_DBG_TRC_AUTOSTART 1

/* AUTOSTART: Debug trace automatically dumps upon certain exception situations
   Without autostart, DBG trace dump has to be started manually  / by explicit call */
#define DRV_PEF_24628E_DBG_TRC_AUTODUMP 1

/* Enable tracing at the boot time */
#define DRV_PEF_24628E_DBG_TRC_AT_BOOT 1

/* default number of trace buffer entries (circular buffer). Size of each ~160 bytes */ 
#define PEF24628_DBG_TRC_MAX_ENTRIES 10000

/* define default mask of enabled trace entry classes */
/*
  #define PEF24628_DBG_TRC_TYPE_CHIP    0x01  
  #define PEF24628_DBG_TRC_TYPE_DRV     0x02
  #define PEF24628_DBG_TRC_TYPE_IRQ     0x04
  #define PEF24628_DBG_TRC_TYPE_INFO    0x10
  #define PEF24628_DBG_TRC_TYPE_MSG     0x20
  #define PEF24628_DBG_TRC_TYPE_TRIGGER 0x80
*/
#define DRV_PEF_24628E_DBG_TRC_FILTER  	0xF8


#endif /* DRV_DEBUG_TRACE_ENABLE */

#endif /* _DRV_PEF24628E_DBGTRACE_CONFIG_H_ */
