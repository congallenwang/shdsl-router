#ifndef SYS_LIB_H
#define SYS_LIB_H

typedef int INT;
typedef char CHAR;
typedef void VOID;
typedef unsigned int UINT;
typedef unsigned char UCHAR;

#define PUBLIC
#define LOCAL

typedef int BOOL;
#define TRUE 1
#define FALSE 0

#define SUCCESS 0
#define ERR -1

#define OK 0
#define ERROR -1

typedef volatile unsigned int VUINT32;
typedef unsigned int UINT32;

typedef unsigned char UINT8;
typedef volatile unsigned char VUINT8;

#define NULL 0

#define MAX_IRQ_NUM 8
#define MAX_USER_TIMERS 10

//replace fucking ifx os wrapper
/** Level for debug printout, level OFF */
#define IFXOS_PRN_LEVEL_OFF                        4
/** Level for debug printout, level HIGH */
#define IFXOS_PRN_LEVEL_HIGH                       3
/** Level for debug printout, level NORMAL */
#define IFXOS_PRN_LEVEL_NORMAL                     2
/** Level for debug printout, level LOW */
#define IFXOS_PRN_LEVEL_LOW                        1

/** Level for error printout, level ERROR */
#define IFXOS_PRN_LEVEL_ERR                        IFXOS_PRN_LEVEL_HIGH
/** Level for error printout, level WARNING */
#define IFXOS_PRN_LEVEL_WRN                        IFXOS_PRN_LEVEL_NORMAL

//print
#define IFXOS_PRN_USR_DBG_NL(module_name, dbg_level, print_message) \
            do { \
               if ((dbg_level) >= IFXOS_PrnUsrModule_##module_name) \
                  { printf print_message ; }\
            } while(0)

   /** Declare a print level variable (user print) for a given module. */
#define IFXOS_PRN_USR_MODULE_DECL(module_name) \
               extern IFX_uint32_t IFXOS_PrnUsrModule_##module_name

   /** Create a debug level variable (user print) for a given module. */
#define IFXOS_PRN_USR_MODULE_CREATE(module_name, init_level) \
               IFX_uint32_t IFXOS_PrnUsrModule_##module_name = (init_level)

   /** Set the debug level (user print) for a given module. */
#define IFXOS_PRN_USR_LEVEL_SET(module_name, new_level) \
            { IFXOS_PrnUsrModule_##module_name = \
               (new_level>IFXOS_PRN_LEVEL_OFF) ?  IFXOS_PRN_LEVEL_OFF : \
               ((new_level<IFXOS_PRN_LEVEL_LOW) ? IFXOS_PRN_LEVEL_OFF :  (new_level)); }

   /** Get the debug level (user print) for a given module. */
#define IFXOS_PRN_USR_LEVEL_GET(module_name)\
            IFXOS_PrnUsrModule_##module_name
               

/** Debug printout macro, printout on interrupt level */
#define IFXOS_PRN_INT_DBG_NL(module_name, dbg_level, print_message) \
            do { \
               if ((dbg_level) >= IFXOS_PrnIntModule_##module_name) \
                  { printf print_message ; }\
            } while(0)

   /** Declare a debug level variable (interrupt print) for a given module. */
#define IFXOS_PRN_INT_MODULE_DECL(module_name) \
               extern IFX_uint32_t IFXOS_PrnIntModule_##module_name

   /** Create a debug level variable (interrupt print) for a given module. */
#define IFXOS_PRN_INT_MODULE_CREATE(module_name, init_level) \
               IFX_uint32_t IFXOS_PrnIntModule_##module_name = (init_level)

   /** Set the debug level (interrupt print) for a given module. */
#define IFXOS_PRN_INT_LEVEL_SET(module_name, new_level) \
            { IFXOS_PrnIntModule_##module_name = \
               (new_level>IFXOS_PRN_LEVEL_OFF) ?  IFXOS_PRN_LEVEL_OFF : \
               ((new_level<IFXOS_PRN_LEVEL_LOW) ? IFXOS_PRN_LEVEL_OFF :  (new_level)); }

/** Get the debug level (interrupt print) for a given module. */
#define IFXOS_PRN_INT_LEVEL_GET(module_name) \
            IFXOS_PrnIntModule_##module_name
            

//timer
typedef unsigned int   IFX_timeMS_t;
#define IFXOS_USecSleep(x)  usleep(x)
#define IFXOS_MSecSleep(x)  usleep(x*1000)

//select
typedef int   IFXOS_drvSelectQueue_t;
typedef int   IFXOS_drvSelectOSArg_t;
typedef int   IFXOS_drvSelectTable_t;

#define IFXOS_DRV_SEL_WAKEUP_TYPE_RD         1

//event
typedef int  IFXOS_event_t;



#endif