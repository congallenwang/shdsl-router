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

#ifndef _DRV_API_H_PEF24628E
#define _DRV_API_H_PEF24628E

/* ============================= */
/* Global Defines                */
/* ============================= */

#if !defined(NO_OS) && !defined(EXCLUDE_SCC_HDLC)
   /** include SCC HDLC support */
//   #define INCLUDE_SCC_HDLC
#endif

/** include MPI support */
#define INCLUDE_MPI

#if !defined(INCLUDE_SCC_HDLC) && !defined(INCLUDE_MPI)
   #error One of the interfaces should be supported (MPI or SCI)
#endif

/* Traces */
/** enable traces */
#define ENABLE_TRACE
#define ENABLE_LOG

#if defined(VXWORKS) && defined(GENERIC_OS) && !defined(DONT_USE_XAPI_TRACE) && !(defined(LINUX) && !defined(__KERNEL__))
    /** common prefix for logging */
    #define _LOGGING_PREFIX __FILE__, __LINE__,
    #define  DSL_ASB_DRV_SHORT_NAME "DSLD"
#else
    /** common prefix for logging */
    #define _LOGGING_PREFIX
#endif

#ifdef LINUX
//#define LOCAL                       static

#include <linux/version.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0))
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33))
#   include <linux/utsrelease.h>
#else
//#   include <generated/utsrelease.h>
#endif
#endif
/*
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0))
#include <linux/autoconf.h>
#endif
*/

#endif

/* ============================= */
/* includes                      */
/* ============================= */

#include "drv_pef24628e_osmap.h"

#ifdef LINUX
   #define CRLF "\n"
#else
   #define CRLF "\n\r"
#endif

#endif /* _DRV_API_H_PEF24628E */
