/*******************************************************************************
       Copyright (c) 2000, Infineon Technologies.  All rights reserved.

                               No Warranty
   Because the program is licensed free of charge, there is no warranty for
   the program, to the extent permitted by applicable law.  Except when
   otherwise stated in writing the copyright holders and/or other parties
   provide the program "as is" without warranty of any kind, either
   expressed or implied, including, but not limited to, the implied
   warranties of merchantability and fitness for a particular purpose. The
   entire risk as to the quality and performance of the program is with
   you.  should the program prove defective, you assume the cost of all
   necessary servicing, repair or correction.

   In no event unless required by applicable law or agreed to in writing
   will any copyright holder, or any other party who may modify and/or
   redistribute the program as permitted above, be liable to you for
   damages, including any general, special, incidental or consequential
   damages arising out of the use or inability to use the program
   (including but not limited to loss of data or data being rendered
   inaccurate or losses sustained by you or third parties or a failure of
   the program to operate with any other programs), even if such holder or
   other party has been advised of the possibility of such damages.
********************************************************************************
   Module      : $RCSfile: sys_noOSLib.c,v $
   Date        : $Date: 2004/05/18 11:30:18 $
   Description : This file contains the implementations of library functions
                 like delay, timer, interrupt handler functions
   Remarks     : still under construction
*******************************************************************************/

/** \file
    \brief No OS Library functions definitions.
*/

/* ============================= */
/* Includes                      */
/* ============================= */
#include "ifx_types.h"
#include "lib_sysdep.h"

//#include "sys_HAL.h"
//#include "sys_OS_AL.h"
#include "sys_noOSLib.h"



/* ============================= */
/* Local defines and macros      */
/* ============================= */

/* reset event */
#define RESET_EVENT              NULL
/* use HW Timer 0 */
#define NO_OS_TICK_TIMER         0
/* 10 ms tick period (10 000 us)*/
#define NO_OS_TICKPERIOD_US      10000
/* device name maximum length */
#define NO_OS_MAXDNLEN           50
/* Max files descriptors number */
#define NO_OS_MAXFD              50

/* ============================= */
/* Local structure               */
/* ============================= */

/** device table structure. */
typedef struct
{

   BOOL nInUse;                    /**< in use status. */
   CHAR rootPath [NO_OS_MAXDNLEN]; /**< root string buffer. */
   INT  nDrvNo;                    /**< driver number. */
} NO_OS_DEVS;

/** driver table structure. */
typedef struct
{
   BOOL        nInUse;    /**< in use status. */
   No_OS_DrvOp drvOps;    /**< driver operations. */
} NO_OS_DRV;

/** file descriptor table structure. */
typedef struct
{
   INT   nDevNum;   /**< device number. */
   VOID* pPrivate;  /**< driver private pointer. */
} NO_OS_FD;

/* ============================= */
/* Local variable declaration    */
/* ============================= */

/* tick counter */
LOCAL VUINT32     nTickCnt;
/* Maintain separate ms counter for noOS to prevent UINT32 overflow
   during calulation of elapsed ms from tick counter in No_OS_GetTickms */
LOCAL VUINT32     nTickMs;
/* loop count per microsecond */
LOCAL VUINT32     nLoopUsCnt;
/* timers counter */
LOCAL UINT8       nUserTimerCnt;
/* events counter */
LOCAL INT         No_OS_EventCnt;
/* timer installer status */
LOCAL BOOL        bTimerTickInstalled;
/* disable irq counters */
LOCAL VUINT8      nDisableIntCnt    [MAX_IRQ_NUM];
/* events list */
LOCAL INT         No_OS_EventList   [NO_OS_MAXEVT];
/* drivers table */
LOCAL NO_OS_DRV   No_OS_DrvTable    [NO_OS_MAXDRIVERS];
/* devices table */
LOCAL NO_OS_DEVS  No_OS_DevTable    [NO_OS_MAXDEVS];
/* file descriptors table */
LOCAL NO_OS_FD    No_OS_Fd          [NO_OS_MAXFD];

/* ============================= */
/* Global variable definition    */
/* ============================= */

/* definition of global errno variable */
INT            No_OS_Errno;

/* ============================= */
/* Global function declaration   */
/* ============================= */

/* ============================= */
/* Local function declaration    */
/* ============================= */

LOCAL VOID No_OS_InitVar        (VOID);
LOCAL INT  No_OS_CalibUsecDelay (UINT32 nUsec);
LOCAL INT  No_OS_TimerTickInit  (VOID);
LOCAL VOID No_OS_TickIntHandler (INT nTimerNo);

/* ============================= */
/* Local function definition     */
/* ============================= */

/*******************************************************************************
Description:
   initializes all internal variables
Arguments:
   none
Return Value:
   none
Remarks:
   This function should be called at first by OS initialization
*******************************************************************************/
LOCAL VOID No_OS_InitVar (VOID)
{
   INT i;

   nTickCnt         = 0;
   nTickMs          = 0;
   nLoopUsCnt       = 1;
   nUserTimerCnt    = 0;
   No_OS_EventCnt   = 0;
   bTimerTickInstalled = FALSE;
   for (i = 0; i < MAX_IRQ_NUM ; i++)
      nDisableIntCnt [i]   = 0;
   for (i = 0; i < NO_OS_MAXEVT ; i++)
      No_OS_EventList [i] = FALSE;
   for (i = 0; i < NO_OS_MAXDRIVERS; i ++)
      No_OS_DrvTable [i].nInUse = FALSE;
   for (i = 0; i < NO_OS_MAXDEVS; i ++)
      No_OS_DevTable[i].nInUse = FALSE;
   for (i = 0; i < NO_OS_MAXFD; i ++)
   {
      No_OS_Fd[i].nDevNum  = (-1);
      No_OS_Fd[i].pPrivate = NULL;
   }
}

/*******************************************************************************
Description:
   calibrates the micro second delay
Arguments:
   nUsec - micro seconds for delay calibration
Return Value:
   none
Remarks:
-  This function should be called at initialization time to set the internal
   us loop count nLoopUsCnt
-  This function is still under construction...
-  The delay calibration is actually not optimal (30 % deviation for time >= 1 ms)
*******************************************************************************/
LOCAL INT No_OS_CalibUsecDelay (UINT32 nUsec)
{
   if (nLoopUsCnt == 0)
   {
      VUINT32 nMaxLoop, nTickStart = 0, nTickStop = 0;

      for (nLoopUsCnt = 1; (nLoopUsCnt < 0x1000 && nTickStart == nTickStop);
          nLoopUsCnt <<=1)
      {
         /* wait for clock turn over */
         for (nTickStop = nTickStart = nTickCnt;
              nTickStart == nTickStop;
              nTickStart = nTickCnt);
         /* delay */
         No_OS_CalibUsecDelay (3 * NO_OS_TICKPERIOD_US);
         nTickStop = nTickCnt;
      }
      /* Get bottom value for delay */
      nMaxLoop = nLoopUsCnt >> 1;
      nTickStart = 0;
      nTickStop  = 0;
      if (nLoopUsCnt < 4)
      {
         nLoopUsCnt = 4;
      }
      for (nLoopUsCnt >>= 2;
           ((nLoopUsCnt < nMaxLoop) && (nTickStop == nTickStart));
           nLoopUsCnt++)
      {
         /* wait for clock turn over */
         for (nTickStop = nTickStart = nTickCnt;
              (nTickStart == nTickStop);
              nTickStart = nTickCnt);
         /* delay */
         No_OS_CalibUsecDelay (3 * NO_OS_TICKPERIOD_US);
         nTickStop = nTickCnt;
      }
      nLoopUsCnt /= 3;
   }

   No_OS_Wait_us (nUsec);

   return (SUCCESS);
}

/*******************************************************************************
Description:
   timer tick interrupt handler
Arguments:
   not_used
Return Value:
   none
Remarks:        this handler increments the tick counter
*******************************************************************************/
LOCAL VOID No_OS_TickIntHandler (INT nTimerNo)
{
   /*printf (".");*/
   /* increment ticks */
   nTickCnt++;
   /* increment ms counter */
   /* (Maintain separate ms counter for noOS to prevent UINT32 overflow
   during calulation of elapsed ms from tick counter in No_OS_GetTickms )*/
   nTickMs += (NO_OS_TICKPERIOD_US / 1000);
   /* reschedule */
   //HAL_RescheduleTimer (nTimerNo);
}

/*******************************************************************************
Description:
   Initializes the timer for tick generation
Arguments:
   none
Return Value:
   SUCCESS or ERR
Remarks:
   Timer 0 is used for the tick generation. The initialization will be
   done only if the tick count is 0
*******************************************************************************/
LOCAL INT No_OS_TimerTickInit ()
{
   INT nRet, nTimerVec;

   if (!bTimerTickInstalled)
   {
//      nRet = HAL_SetupTimer (NO_OS_TICK_TIMER, NO_OS_TICKPERIOD_US, &nTimerVec);
//      if (nRet == SUCCESS)
//         OS_AL_InstallIntHandler(nTimerVec,No_OS_TickIntHandler, NO_OS_TICK_TIMER);
//      if (nRet == SUCCESS)
//         nRet = HAL_StartTimer (NO_OS_TICK_TIMER);
//      if (nRet == SUCCESS)
      {
         bTimerTickInstalled = TRUE;
         printf ("Tick Timer\t\t: 10 ms\n\r");
      }
   }


   return (nRet);
}

/* ============================= */
/* Global function definition    */
/* ============================= */

/******************************************************************************/
/** \brief Entry point of the No OS system.

    This function
    - initializes Hardware and OS layers
    - installs the timer ticks interrupt with 10 ms resolution,
    - calibrates the micro second delay,
    - installs the driver and initializes the plattform,
    - calls the wrapper for user application,
    - shutdown the plattform at end of the user application.

    \see HAL_Init()
    \see OS_AL_Init()
    \see No_OS_InitBoard()
    \see usrApp()
    \see No_OS_ShutdownBoard()
*/
/******************************************************************************/
VOID No_OS_Init()
{
   INT nRet;

   printf ("\nNo OS System Initialization ...\n\r");

   /* do OS init first, since all memory gets cleaned here */
   //OS_AL_Init ();

   /* initialize local variables */
   No_OS_InitVar ();

   /* do hardware init */
   //nRet = HAL_Init ();

   /* initialize timer for tick generation  */
   if (nRet == SUCCESS)
      nRet = No_OS_TimerTickInit ();
   /* calibrate usec delay */
   if (nRet == SUCCESS)
   {
      nRet = No_OS_CalibUsecDelay (1);
   }
   printf ("delay calibration\t: 1 us\n\r");
   printf ("System up.\r\n\nGoing to run user application ...\n\n\r");
   /* install driver */
   if (nRet == SUCCESS)
      nRet = No_OS_InitBoard ();
   /* start user application */
   if (nRet == SUCCESS)
      nRet = usrApp ();
   /* shutdown board hardware */
   if (nRet == SUCCESS)
         nRet = No_OS_ShutdownBoard ();
   /* This function never returns */
   printf ("\nPlattform shut down.\nSystem down.\n\r");
   while (1);
}

/******************************************************************************/
/** \brief Gives the memory base address of the system uC.

    \return
       Hardware base address
    \see HAL_GetBaseAddr()
*/
/******************************************************************************/
UINT No_OS_GetHwBaseAddr (VOID)
{
   //return HAL_GetBaseAddr ();
}

/******************************************************************************/
/** \brief Install the interrupt handler.

    \param irq     -  irq line
    \param handler -  ISR function
    \param data    -  ISR argument
    \return
       SUCCESS or ERR
    \see OS_AL_InstallIntHandler()
*/
/******************************************************************************/
INT No_OS_InstallIntHandler (INT irq, VOID (*handler)(INT), INT data)
{
   //return OS_AL_InstallIntHandler (irq, handler, data);
   return 0;
}

/******************************************************************************/
/** \brief Uninstall the \a irq line interrupt handler.

    \param irq   -  irq line
    \see OS_AL_UninstallIntHandler()
*/
/******************************************************************************/
VOID No_OS_UninstallIntHandler (INT irq)
{
  // OS_AL_UninstallIntHandler (irq);
}

/******************************************************************************/
/** \brief Enables the interrupt on line irq.

    \param irq   -  irq line
    \return
       disable irq count.
    \see HAL_IntEnable()
    \remark
       The interrupts will be enabled only if disabled before. Otherwise
       the disable irq counter will be decremented.
*/
/******************************************************************************/
INT No_OS_IntEnable (INT irq)
{
   if (nDisableIntCnt [irq] != 0x00)
      nDisableIntCnt [irq]--;
//   if (nDisableIntCnt [irq] == 0x00)
//      HAL_IntEnable (irq);

   return nDisableIntCnt [irq];
}

/******************************************************************************/
/** \brief Disables the interrupt on line irq.

    \param irq   -  irq line
    \see HAL_IntDisable()
    \remark
       The interrupts will be disabled only if enabled before. Otherwise
       the disable irq counter will be incremented.
*/
/******************************************************************************/
VOID No_OS_IntDisable (INT irq)
{
//   if (nDisableIntCnt [irq] == 0x00)
//      HAL_IntDisable (irq);
   if (nDisableIntCnt [irq] != 0xFF)
      nDisableIntCnt [irq]++;
}


/******************************************************************************/
/** \brief Lock mechanism for general processor interrupts

    \return
    lock id

    \see HAL_LockInt()

    \remark
    use this function to protect share variables against interrupts
*/
/******************************************************************************/
INT No_OS_LockInt()
{
   //return HAL_LockInt();
   return 0;
}

/******************************************************************************/
/** \brief Lock mechanism for general processor interrupts

    \param lock   -  lock id

    \see HAL_UnlockInt()

    \remark
    use this function to protect share variables against interrupts
*/
/******************************************************************************/
VOID No_OS_UnlockInt(INT lock)
{
   //HAL_UnlockInt((UINT)lock);
   return;
}

/******************************************************************************/
/** \brief Create a timer with the given parameters and installs the timer handler.

    \param nTimeout  - timer timeout.
    \param timerfunc - timer handler.
    \param data      - argument of timer handler.
    \return
       Timer Id of ERR if no timer available.
    \remark
       Only hardware timers are used actually.
    \see HAL_SetupTimer()
    \see OS_AL_InstallIntHandler()
*/
/* FIXME currently limited to available HW timers.
       should use one HW Timer for all SW Timers
   if (!HW_TIMER1_installed)
   {
      Hal_Setuptimer 1
      int handler
      starttimer 1
   }
   register timerfunc with inthandler
   inthandler decrements intervall, checks if interval is expired and calls
   software handler
   inthandler example from ct uc project
   if(bTimerClick)
   {
      bTimerClick = 0;
      for(i = 0; i < (sizeof(TimerA0) / sizeof(stCyclFunc)); i++)
      {
         if(abTimerTick[i] == 0)
         {
            abTimerTick[i] = TimerA0[i].bTimeStamp;
            TimerA0[i].fFunc();
         }
         else
         {
            abTimerTick[i]--;
         }
      }
   }
*/
/******************************************************************************/
INT No_OS_CreateTimer (UINT nTimeout, VOID (*timerfunc)(INT), INT data)
{
   INT nRet, nTimerId, nTimerVec;

   /* No timer more available */
   if (nUserTimerCnt >= MAX_USER_TIMERS)
      return ERR;

   nTimerId = (nUserTimerCnt + 1);
   /* setup timer and install callback */
   nRet=SUCCESS;
//   nRet = HAL_SetupTimer (nTimerId, (nTimeout * 1000), &nTimerVec);
   if (nRet == SUCCESS)
//      nRet = OS_AL_InstallIntHandler(nTimerVec, timerfunc, data);
//   if (nRet == SUCCESS)
      nUserTimerCnt ++;
   else
      nTimerId = ERR;

   return nTimerId;
}

/******************************************************************************/
/** \brief Starts a specific Timer.

    \param nTimerId - timer id
    \return
      SUCCESS or ERR
    \see HAL_StartTimer()
*/
/******************************************************************************/
INT No_OS_StartTimer (INT nTimerId)
{
//   return HAL_StartTimer (nTimerId);
}

/******************************************************************************/
/** \brief Stops a specific Timer.

    \param nTimerId - timer id
    \return
      SUCCESS or ERR
    \see HAL_StopTimer()
*/
/******************************************************************************/
INT No_OS_StopTimer (INT nTimerId)
{
//   return HAL_StopTimer (nTimerId);
    return 0;
}

/******************************************************************************/
/** \brief Modifies the timeout of a specific Timer.

    \param nTimerId     - id of timer to modify
    \param nNewTimeout  - new timeout.
    \return
       Id of modified timer or ERR.
    \see HAL_DeleteTimer()
    \see HAL_SetupTimer()
    \see HAL_StartTimer()
*/
/******************************************************************************/
INT No_OS_ModifyTimer (INT nTimerId, UINT nNewTimeout)
{
   INT nRet;

   /* delete previous timer with this id */
   //HAL_DeleteTimer (nTimerId, NULL);
   /* setup timer onece more */
   //nRet = HAL_SetupTimer (nTimerId, (nNewTimeout * 1000), NULL);
   nRet=SUCCESS;
   /* start timer again */
//   if (nRet == SUCCESS)
//      nRet = HAL_StartTimer (nTimerId);
   if (nRet == ERR)
      nTimerId = ERR;

   return (nTimerId);
}

/******************************************************************************/
/** \brief Reschedules a specific Timer.

    \param nTimerId - timer id
    \see HAL_RescheduleTimer()
    \remark
       Use this function for periodical timers.
*/
/******************************************************************************/
VOID No_OS_ReschedTimer(INT nTimerId)
{
   //HAL_RescheduleTimer (nTimerId);
}

/******************************************************************************/
/** \brief Deletes a specific Timer and uninstall the timer handler.

    \param nTimerId - timer id
    \see HAL_DeleteTimer()
    \see OS_AL_UninstallIntHandler()
*/
/******************************************************************************/
VOID No_OS_DeleteTimer (INT nTimerId)
{
   INT nTimerVec;

   /* check if timer id is known */
   if (nTimerId < 1 || nTimerId > MAX_USER_TIMERS)
      return;

   nUserTimerCnt --;
   //HAL_DeleteTimer (nTimerId, &nTimerVec);
   //OS_AL_UninstallIntHandler (nTimerVec);
}

/******************************************************************************/
/** \brief Initializes an event or a semaphore.

    \param event  - reference to the event to initialize.
    \param bState - initial state (TRUE or FALSE).
    \remark Events are initialized with FALSE, semaphores with TRUE.
*/
/******************************************************************************/
VOID No_OS_InitEvent (NO_OS_EVENT *event, BOOL bState)
{
   *event = RESET_EVENT;
   if (No_OS_EventCnt < NO_OS_MAXEVT)
   {
      No_OS_EventList [No_OS_EventCnt] = bState;
      *event = (NO_OS_EVENT)&No_OS_EventList [No_OS_EventCnt];
      No_OS_EventCnt ++;
   }
}

/******************************************************************************/
/** \brief Deletes event/semaphore.

    \param event - reference to the event to delete.
*/
/******************************************************************************/
VOID No_OS_DeleteEvent (NO_OS_EVENT *event)
{
   *event = RESET_EVENT;
   No_OS_EventCnt --;
}

/******************************************************************************/
/** \brief Clears event/semaphore.

    \param event - reference to the event to delete.
*/
/******************************************************************************/
VOID No_OS_ClearEvent (NO_OS_EVENT event)
{
   *event = FALSE;
}

/******************************************************************************/
/** \brief Waits within timeout for a specific event with a condition.

    \param event     - event to wait on.
    \param bCond     - condition.
    \param nTimeout  - timeout.
    \return
       TRUE if event occurs with condition, else FALSE.
    \remark
       This function waits for an event which could be woken up by the interrupt
       routine for Example:.
*/
/******************************************************************************/
BOOL No_OS_WaitEvent (NO_OS_EVENT event, BOOL bCond, UINT nTimeout)
{
   BOOL bRet = FALSE;

   if (bCond == FALSE)
   {
      while ((*event == FALSE) && (nTimeout != 0) && (bCond == FALSE))
      {
         No_OS_Wait_ms (1);
         nTimeout--;
      };
   }
   if (bCond == TRUE)
   {
      bRet = TRUE;
      *event = FALSE;
   }

   return bRet;
}

/******************************************************************************/
/** \brief Wakes up a specific event.

    \param event - reference to the event to wake up.
    \remark
       This function can be called in an interrupt routine to
       wake up an event the task code is waiting for.
*/
/******************************************************************************/
VOID No_OS_WakeupEvent (NO_OS_EVENT event)
{
   *event = TRUE;
}

/******************************************************************************/
/** \brief Sleeps on an event without a condition.

    \param event     - event to sleep on.
    \param nTimeout  - timeout.
    \return
       TRUE if event occurs, else FALSE.
    \remark
       Events could be woken up from the interrupt routine for example.
*/
/******************************************************************************/
INT No_OS_SleepOnEvent (NO_OS_EVENT event, INT nTimeout)
{
   INT nRet = SUCCESS;

   while ((*event == FALSE) && (nTimeout != 0))
   {
      No_OS_Wait_ms (1);
      if (nTimeout != WAIT_FOREVER)
         nTimeout--;
   };
   if (*event == TRUE)
      *event = FALSE;
   else
      nRet = ERR;

   return nRet;
}

/******************************************************************************/
/** \brief Waits microseconds.

    \param u_sec - number of microseconds to wait.
    \remarks
       - The micro second delay calibration is done at initialization time.
       - The processor will actively wait by executing a NOP operation
       during the time to wait. So this wait is BLOCKING
*/
/******************************************************************************/
VOID No_OS_Wait_us (UINT u_sec)
{
   UINT32 i, j;

   for (i = 0; i < u_sec ; i++)
   {
      for(j = 0; j < nLoopUsCnt; j++);
   }
}

/******************************************************************************/
/** \brief Waits milliseconds.

    \param m_sec - number of milliseconds to wait.
    \remark
       This wait is BLOCKING and uses the No_OS_Wait_us implementation.
*/
/******************************************************************************/
VOID No_OS_Wait_ms (UINT m_sec)
{
  No_OS_Wait_us (m_sec*1000);
}

/******************************************************************************/
/** \brief Waits milliseconds.

    \param m_sec - number of milliseconds to wait.
    \remark
       This wait should be NOT BLOCKING, but in the No OS single task environment,
       it is implemented as BLOCKING.
*/
/******************************************************************************/
VOID No_OS_Wait (UINT m_sec)
{
  No_OS_Wait_us (m_sec*1000);
}

/******************************************************************************/
/** \brief Gives the system clock rate.

    \return
       Clock Rate.
    \remark
       The No OS system generates 100 ticks/second
*/
/******************************************************************************/
INT No_OS_ClkRateGet(VOID)
{
   return (1000000 / NO_OS_TICKPERIOD_US);
}

/******************************************************************************/
/** \brief Gives the system tick count.

    \return tick count since system start.
*/
/******************************************************************************/
UINT No_OS_GetTickCnt (VOID)
{
   return (nTickCnt);
}

/******************************************************************************/
/** \brief Gives the system tick count in milliseconds.

    \return
       tick count since system start in millisecond resolution.
    \remark
       Suitable function to use for timestamp and time consumption measurement.
*/
/******************************************************************************/
UINT No_OS_GetTickms (VOID)
{
  return (nTickMs);
}

/******************************************************************************/
/** \brief Installs device driver methods.

    \param  pDrvOp - driver operations.
    \return
       SUCCESS or ERR.
    \remark
       This function should be called by the driver to register its methods
*/
/******************************************************************************/
INT  No_OS_InstallDriver (No_OS_DrvOp *pDrvOp)
{
   INT i, nRet = ERR;

   for (i = 0; i < NO_OS_MAXDRIVERS; i ++)
   {
      /* assign next free entry in driver table to this driver */
      if (No_OS_DrvTable[i].nInUse == FALSE)
      {
         No_OS_DrvTable [i].nInUse       = TRUE;
         No_OS_DrvTable [i].drvOps.open  = pDrvOp->open;
         No_OS_DrvTable [i].drvOps.close = pDrvOp->close;
         No_OS_DrvTable [i].drvOps.ioctl = pDrvOp->ioctl;
         No_OS_DrvTable [i].drvOps.read  = pDrvOp->read;
         No_OS_DrvTable [i].drvOps.write = pDrvOp->write;
         nRet = i;
         break;
      }
   }

   return (nRet);
}

/******************************************************************************/
/** \brief Add a device to the device list.

    \param rootPath - root path of the device (e.g. /dev/GEMINAXD/0, /dev/vin)
    \param nDrvNo   - Number of driver to use for this device.
    \return
       SUCCESS or ERR
    \remark
       This function should be called by the driver to register a device.
*/
/******************************************************************************/
INT  No_OS_AddDevice (CHAR* rootPath, INT nDrvNo)
{
   INT i, nRet = ERR;

   if (strlen (rootPath) <= NO_OS_MAXDNLEN)
   {
      /* assign next free entry in device table to this device */
      for (i = 0; i < NO_OS_MAXDEVS; i ++)
      {
         if (No_OS_DevTable [i].nInUse == FALSE)
         {
            No_OS_DevTable [i].nInUse  = TRUE;
            strncpy (No_OS_DevTable [i].rootPath, rootPath, NO_OS_MAXDNLEN);
            No_OS_DevTable [i].nDrvNo = nDrvNo;
            nRet = SUCCESS;
            break;
         }
      }
   }
   /*else nRet = ERR_DEV_NAME; FIXME */

   return (nRet);
}

/******************************************************************************/
/** \brief No OS Method to open a device in application code.

    \param path   - device path.
    \param flags  - flags needed (default 0).
    \return
       Device file descriptor or ERR if open failed.

    \b Example\b :
    \code
    INT nFd;
    nFd = open ("/dev/vin10", 0);
    if (nFd < 0)
    {
      printf (" error wile opening device /dev/vin10\n");
    }
    \endcode
    \remark
      - the parameter flags is actually not evaluated.
      - This interface is actually improved to open many devices from
        application code.
*/
/******************************************************************************/
#ifdef DONT_USE_NO_OS_PROTOTYPES
INT  open  (CHAR* path, INT flags)
#else
INT  No_OS_Open  (CHAR* path, INT flags)
#endif
{
   VOID *pPrivate;
   NO_OS_OPEN pOpen;
   INT i,j, nRet = ERR;
   INT len1, len2;

   if (path != NULL)
   {
      for (i = 0; i < NO_OS_MAXDEVS; i ++)
      {
         /* last device in device table, exit */
         if (No_OS_DevTable[i].rootPath[0] == 0)
            break;

         len1 = strlen(path);
         len2 = strlen(No_OS_DevTable[i].rootPath);

         /* check device name */
         if (!strncmp(path, No_OS_DevTable[i].rootPath, (len1<=len2?len1:len2)))
         {
            /* Device found in device table */
            pOpen = No_OS_DrvTable[No_OS_DevTable[i].nDrvNo].drvOps.open;
            if (pOpen != NULL)
            {
               /* call open function of device driver */
               pPrivate = (VOID*) pOpen (path,flags);
               /* check if valid ptr */
               if ((INT)pPrivate != ERR)
               {
                  for (j = 0; j < NO_OS_MAXFD; j++)
                  {
                     /* assign next free filedescriptor to device */
                     if (No_OS_Fd[j].nDevNum == (-1))
                     {
                        No_OS_Fd [j].nDevNum = i;
                        No_OS_Fd [j].pPrivate = pPrivate;
                        nRet = j;
                        break;
                     }
                  }
                  break;
               }
               else
                  break;
            }
         }
      }
   }

   return (nRet);
}

/******************************************************************************/
/** \brief No OS ioctl Method.

    \param fd  - file descriptor.
    \param cmd - io command.
    \param arg - io argument.
    \return
       SUCCESS or ERR.

    \b Example\b :
    \code
    INT nRet;
    VINETIC_IO_INIT ioInit;

    memset (&ioInit, 0, sizeof (ioInit));
    nRet = ioctl (nFd, FIO_VINETIC_INIT, (UINT)&ioInit);
    if (nRet == ERR)
    {
      printf ("Vinetic Initialization failed\n");
    }
    \endcode
    \see open ()
    \remark
       This interface is actually improved to control many devices from
       application code.
*/
/******************************************************************************/
#ifdef DONT_USE_NO_OS_PROTOTYPES
INT  ioctl (INT fd, UINT cmd, UINT arg)
#else
INT  No_OS_Ioctl (INT fd, UINT cmd, UINT arg)
#endif
{
   NO_OS_IOCTL pIoctl;
   VOID* pPrivate;
   INT nDevNum, nRet = ERR;

   if (fd >= 0)
   {
      /* obtain device number and private data pointer */
      nDevNum  = No_OS_Fd[fd].nDevNum;
      pPrivate = No_OS_Fd[fd].pPrivate;
      /* call appropriate ioctl if correct devnum */
      if ((nDevNum >= 0) && (nDevNum < NO_OS_MAXDEVS))
      {
         pIoctl = No_OS_DrvTable[No_OS_DevTable[nDevNum].nDrvNo].drvOps.ioctl;
         if (pIoctl != NULL)
         {
            nRet = pIoctl (pPrivate, cmd, arg);
         }
      }
   }

   return (nRet);
}

/******************************************************************************/
/** \brief No OS Method to close a device in application code.

    \param fd     - file descriptor.
    \return
       SUCCESS or ERR.

    \b Example\b :
    \code
    INT nRet;

    nRet = close (nFd);
    if (nRet == ERR)
    {
      printf (" error wile closing device /dev/vin10\n");
    }
    \endcode
    \see open ()
    \remark
       This interface is actually improved to close many devices from
       application code.
*/
/******************************************************************************/
#ifdef DONT_USE_NO_OS_PROTOTYPES
INT  close (INT fd)
#else
INT  No_OS_Close (INT fd)
#endif
{
   NO_OS_CLOSE pClose;
   VOID* pPrivate;
   INT nDevNum, nRet = ERR;

   if (fd >= 0)
   {
      /* obtain device number and private data pointer */
      nDevNum  = No_OS_Fd[fd].nDevNum;
      pPrivate = No_OS_Fd[fd].pPrivate;
      /* call appropriate close function if correct devnum */
      if ((nDevNum >= 0) && (nDevNum < NO_OS_MAXDEVS))
      {
         pClose = No_OS_DrvTable[No_OS_DevTable[fd].nDrvNo].drvOps.close;
         if (pClose != NULL)
         {
            nRet = pClose(pPrivate);
            /* clear entry in file descriptor list */
            No_OS_Fd[fd].nDevNum = (-1);
            No_OS_Fd[fd].pPrivate = NULL;
         }
      }
   }

   return (nRet);
}

/******************************************************************************/
/** \brief No OS Method to read a device from application code.

    \param fd     - file descriptor.
    \param buffer - read buffer.
    \param len    - size of byte to read.
    \return
       byte size read or ERR.

    \b Example\b :
    \code
    INT nRet;
    CHAR buf [MAX_BYTE];

    nRet = read(nFd, buf, MAX_BYTE);
    if (nRet != MAX_BYTE)
    {
      printf ("couldn't read all data or error occured\n");
    }
    \endcode
    \see open ()
    \remark
       This interface is actually improved to read many devices from
       application code.
*/
/******************************************************************************/
#ifdef DONT_USE_NO_OS_PROTOTYPES
INT  read (INT fd, CHAR* buffer, INT len)
#else
INT  No_OS_Read  (INT fd, CHAR* buffer, INT len)
#endif
{
   NO_OS_READ pRead;
   VOID* pPrivate;
   INT nDevNum, nRet = ERR;

   if (fd >= 0)
   {
      /* obtain device number and private data pointer */
      nDevNum  = No_OS_Fd[fd].nDevNum;
      pPrivate = No_OS_Fd[fd].pPrivate;
      /* call appropriate read function if correct device number */
      if ((nDevNum >= 0) && (nDevNum < NO_OS_MAXDEVS))
      {
         pRead = No_OS_DrvTable[No_OS_DevTable[fd].nDrvNo].drvOps.read;
         if (pRead != NULL)
         {
            nRet = pRead(pPrivate, buffer, len);
         }
      }
   }

   return (nRet);
}

/******************************************************************************/
/** \brief No OS Method to write on a device from application code.

    \param fd     - file descriptor.
    \param buffer - write buffer.
    \param len    - size of byte to write.
    \return
       byte size written or ERR.

    \b Example\b :
    \code
    INT nRet;

    nRet = write (nFd, gBuf, sizeof (gBuf));
    if (nRet != sizeof (gBuf))
    {
      printf ("couldn't write all data or error occured\n");
    }
    \endcode
    \see open ()
    \remark
       This interface is actually improved to write on many devices from
       application code.
*/
/******************************************************************************/
#ifdef DONT_USE_NO_OS_PROTOTYPES
INT  write (INT fd, CHAR* buffer, INT len)
#else
INT  No_OS_Write (INT fd, CHAR* buffer, INT len)
#endif
{
   NO_OS_WRITE pWrite;
   VOID* pPrivate;
   INT nDevNum, nRet = ERR;

   if (fd >= 0)
   {
      /* obtain device number and private data pointer */
      nDevNum  = No_OS_Fd[fd].nDevNum;
      pPrivate = No_OS_Fd[fd].pPrivate;
      /* call appropriate write function if correct device number */
      if ((nDevNum >= 0) && (nDevNum < NO_OS_MAXDEVS))
      {
         pWrite = No_OS_DrvTable[No_OS_DevTable[fd].nDrvNo].drvOps.write;
         if (pWrite != NULL)
         {
            nRet = pWrite (pPrivate, buffer, len);
         }
      }
   }

   return (nRet);
}

/******************************************************************************/
/** \brief Remove Device from the device list.

    \param rootPath    -  root path of the device (e.g. /dev/GEMINAXD/0, /dev/vin)
    \return
       SUCCESS or ERR
    \remarks
       This function should be called by the driver to unregister a device.
*/
/******************************************************************************/
INT  No_OS_DeleteDevice     (CHAR * rootPath)
{
   INT i, nRet = ERR;

   if (strlen (rootPath) <= NO_OS_MAXDNLEN)
   {
      for (i = 0; i < NO_OS_MAXDEVS; i ++)
      {
         /* last device in device table, exit */
         if (No_OS_DevTable[i].rootPath[0] == 0)
            break;
         /* check rootpath and clear entry if */
         if (!strncmp(rootPath, No_OS_DevTable[i].rootPath, NO_OS_MAXDNLEN))
         {
            /* Device found in device table */
            if (No_OS_DevTable [i].nInUse == TRUE)
            {
               No_OS_DevTable [i].nInUse  = FALSE;
               No_OS_DevTable [i].rootPath[0] = 0;
               No_OS_DevTable [i].nDrvNo = 0;
               nRet = SUCCESS;
               break;
            }
         }
      }
   }
   /*else nRet = ERR_DEV_NAME; FIXME */

   return (nRet);
}

/*******************************************************************************
Description:
   Remove a device driver from the driver list of the noOS
Arguments:
   nDrvNo   - Number of device driver
Return Value:
   none
Remarks:
 ******************************************************************************/
/** \brief No OS Method used by a driver to unregister itself.

    \param nDrvNo - driver number.
    \remarks
       This function should be called by the driver when it is time
       to unregister.
*/
/******************************************************************************/
VOID No_OS_UninstallDriver  (INT nDrvNo)
{
   INT i;

   for (i = 0; i < NO_OS_MAXDRIVERS; i ++)
   {
      if (No_OS_DrvTable[i].nInUse == TRUE)
      {
         No_OS_DrvTable [i].nInUse       =  FALSE;
         No_OS_DrvTable [i].drvOps.open  =  NULL;
         No_OS_DrvTable [i].drvOps.close =  NULL;
         No_OS_DrvTable [i].drvOps.ioctl =  NULL;
         No_OS_DrvTable [i].drvOps.read  =  NULL;
         No_OS_DrvTable [i].drvOps.write =  NULL;
         break;
      }
   }
}

/*******************************************************************************
   $Log: sys_noOSLib.c,v $
   Revision 1.9  2004/05/18 11:30:18  tauche
   functions HAL_LockInt() and HAL_UnlockInt() replaced by No_OS_LockInt and No_OS_UnlockInt() implemented in sys_noOSLib.c

   Revision 1.8  2004/03/16 17:51:01  tauche
   comment in comment removed

   Revision 1.7  2004/03/11 22:34:55  tauche
   comment in comment removed

   Revision 1.6  2004/02/18 10:30:22  kamdem
   little changes in comments.
   Non_OS.fg generates a document under ../doc/html with infineon logo.

   Revision 1.5  2004/02/18 10:12:57  kamdem
   C-Files commented in doxygen style.
   sys_OS_AL_userOS.c and sys_HAL_userHW.c should be used by customer as template for his implementation.

   Revision 1.4  2004/02/10 16:08:28  kamdem
   No os doxygen config file uses now relatives paths.
   Document can always be generated by executing :
   ${DOXIGEN_INSTALL_PATH}/doxygen/bin/doxygen.exe Non_OS.cfg

   Revision 1.3  2004/02/09 10:43:15  kamdem
   cosmetic changes

   Revision 1.2  2004/02/09 10:35:35  kamdem
   include adapted

   Revision 1.1  2004/02/09 09:49:54  tauche
   Moved noOS files to new location in system/noOS

   Revision 1.1  2004/02/09 09:17:30  tauche
   Moved noOS files to new location in system/noOS

   Revision 1.27  2004/02/04 14:19:19  kamdem
   Improvements in the No OS Library :

   - if the target driver also defines one of the user interface
     macros "open", "close", ..., for any reason,
     -DDONT_USE_NO_OS_PROTOTYPES
      should be added to the compiler defines to avoid compiler
      errors

   - Drivers using this OS - file should be compiled with additional
      compiler switch -DNO_OS

   - More comments added

   Revision 1.26  2004/02/03 10:27:24  tauche
   redefinitions removed

   Revision 1.25  2004/02/03 10:22:13  kamdem
   clean copy

   Revision 1.24  2004/02/03 10:18:26  tauche
   No_OS_Open() stop searching for device if last device in table is reached.
   No_OS_DevDelete() fixed

   Revision 1.23  2004/02/02 18:27:28  kamdem
   conflict solved

   Revision 1.22  2004/02/02 17:56:25  tauche
   User Interface for device driver handling (open,close,...) implemented and tested

   Revision 1.21  2004/01/30 18:04:52  kamdem
   more comments

   Revision 1.20  2004/01/30 15:25:28  kamdem
   more corrections in documentation text.
   prints added to keep tack of system life.

   Revision 1.19  2004/01/29 19:18:20  kamdem
   sys_drv_noOSLib.c : all variables used for delay calibration are volatile

   sys_drv_noOSLib.h/sys_HAL.h/sys_OS_AL.h :

   doxygen comments added for interface document generation.

   Revision 1.18  2004/01/28 17:54:55  kamdem
   Lib.h/c : Interface relevant info hidden in c-file
   Other  : implementation of _IO changed.

   Revision 1.17  2004/01/27 19:23:37  kamdem
   No_OS_DeleteTimer : Timer 1 couldn't get deleted, what causes a machine
   check error

   Revision 1.16  2004/01/27 13:13:20  kamdem
   * sys_drv_noOS.h : new defines added for _IOW and _IOR
   * sys_drv_debug.h :  DRV_LOG (x) calls printf
   * sys_drv_noOSLib.c : error corrected

   Revision 1.15  2004/01/27 11:39:09  kamdem
   conflicts solved

   Revision 1.14  2004/01/26 17:30:38  tauche
   Fixed No_OS_SleepOnEvent()

   Revision 1.13  2004/01/23 19:53:53  kamdem
   improvments in the library. Use compiler switch
   NON_OS_LITTLE_ENDIAN or NON_OS_BIG_ENDIAN to determine your endianess

   Revision 1.12  2004/01/23 12:24:08  kamdem
   new boolean parameter added to No_OS_InitEvent :
   - for semaphores, initialioze with TRUE
   - for events, initialize with FALSE

   Revision 1.9  2004/01/20 12:56:44  kamdem
   Events macros reviewed : Init_Event initializes the queue with
   one event from the event list

   Revision 1.8  2004/01/20 09:10:20  kamdem
   Share with Stefan T.  Just tring to compile

   Revision 1.7  2004/01/19 17:48:35  kamdem
   temporary check in for share with stefan T

   Revision 1.6  2004/01/16 19:08:13  tauche
   No_OS_tickGet() implemented

   Revision 1.4  2004/01/15 17:04:38  kamdem
   Hardware functions for disable/enable interrupts
*******************************************************************************/
