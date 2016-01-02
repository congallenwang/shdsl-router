/** \file
   Common definitions for all boards.
*/

#ifndef _board_h
#define _board_h

typedef struct
{
   /** device interface should be used */
   UINT32 nInterfaceMode;
   /** if set the polling mode instead of the interrupt mode is used */
   BOOL   bPollingMode;
   /** clock of the SCI interface, used for the HDLC driver */
   UINT32 nSciClock;
   /** maximum number of devices should be accessed */
   UINT32 nMaxDevNumber;
   /** maximum lines of devices should be accessed */
   UINT32 nMaxLineNumber;
} BOARD_Configuration_t;

/**
   Maximum number of lines per device
*/
#ifndef PEF24628E_MAX_LINES_PER_DEVICE
#define PEF24628E_MAX_LINES_PER_DEVICE 4
#endif

/**
   Maximum of DSL lines.
*/
#define PEF24628E_MAX_LINE_NUMBER      (PEF24628E_MAX_LINES_PER_DEVICE*PEF24628E_MAX_DEV_NUMBER)

#define IRQ_INIT    0

#define BASE_ADDRESS_INIT  {0x10000000}

#endif
