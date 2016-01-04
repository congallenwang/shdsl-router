#ifndef SYS_LIB_H
#define SYS_LIB_H


#define PUBLIC
#define LOCAL                       static

typedef int BOOL;
#define TRUE 1
#define FALSE 0

#define SUCCESS 0
#define ERR -1

#define OK 0
#define ERROR -1

typedef int INT;
typedef void VOID;
typedef unsigned int UINT;
typedef unsigned char UCHAR;
typedef char                        CHAR;
typedef signed char                 INT8;
typedef unsigned char               UINT8;
typedef signed char volatile        VINT8;
typedef unsigned char volatile      VUINT8;

typedef signed short                INT16;
typedef unsigned short              UINT16;
typedef signed short volatile       VINT16;
typedef unsigned short volatile     VUINT16;
typedef signed int                  INT32;
typedef unsigned int                UINT32;
typedef signed int volatile         VINT32;
typedef unsigned int volatile       VUINT32;

typedef char*                       LPSTR;
typedef const char*                 LPCSTR;

#define NULL 0

#define HUGE

#define SWAP_UINT32(x)  ( (((x) & 0xFF000000)>>24)  \
                        | (((x) & 0x00FF0000)>>8)   \
                        | (((x) & 0x0000FF00)<<8)   \
                        | (((x) & 0x000000FF)<<24))
#define SWAP_UINT16(x)  ( (((x) & 0xFF00)>>8) | (((x) & 0x00FF)<<8) )

#define IFX_LITTLE_ENDIAN 0
#define IFX_BIG_ENDIAN 1
#define IFX_BYTE_ORDER  IFX_BIG_ENDIAN

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
