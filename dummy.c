//dummy function, to keep shdsl driver file not changed.
#include "ifx_types.h"
#include "lib_sysdep.h"

IFX_int32_t IFXOS_DrvSelectQueueInit(
               IFXOS_drvSelectQueue_t  *pDrvSelectQueue)
{
    return 0;
}

IFX_void_t IFXOS_DrvSelectQueueWakeUp(
               IFXOS_drvSelectQueue_t  *pDrvSelectQueue,
               IFX_uint32_t            drvSelType)
{
    return;
}

IFX_int32_t IFXOS_DrvSelectQueueAddTask(
               IFXOS_drvSelectOSArg_t  *pDrvSelectOsArg,
               IFXOS_drvSelectQueue_t  *pDrvSelectQueue,
               IFXOS_drvSelectTable_t  *pDrvSelectTable)
{
    return 0;
}

IFX_int_t IFXOS_EventInit(
               IFXOS_event_t  *pEventId)
{
    return 0;
}

IFX_int_t IFXOS_EventDelete(
               IFXOS_event_t  *pEventId)
{
    return 0;
}
IFX_int_t IFXOS_EventWakeUp(
               IFXOS_event_t  *pEventId)
{
    return 0;
}

IFX_int_t IFXOS_EventWait(
               IFXOS_event_t  *pEventId,
               IFX_uint32_t   waitTime_ms,
               IFX_int32_t    *pRetCode)
{
    return 0;
}
//irq
void IFXOS_IRQ_ENABLE(unsigned int x) 
{return;}

void IFXOS_IRQ_DISABLE(unsigned int x) 
{return;}



