//dummy function, to keep shdsl driver file not changed.
#include "ifx_types.h"
#include "lib_sysdep.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>


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

static void *g_address;

int map(char *fname, unsigned int offset, unsigned int len)
{
        FILE *f;

        if (!(f = fopen(fname, "r+w"))) {
                perror("fopen");
                return -EIO;
        }
        //g_address=mmap(0, len, PROT_READ, MAP_FILE | MAP_PRIVATE, fileno(f), offset);
        g_address =  mmap(0, len, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(f), offset);
        if (g_address == (void *)-1) {
                perror("mmap: ");
                fclose(f);
                return -EFAULT;
        }
        fclose(f);
        return 0;
}

#define BASE_ADDRESS 0x10000000

int reg_init()
{
       map("/dev/mem", BASE_ADDRESS, 0x1000000);
}


void reg_write(unsigned char off, unsigned char v)
{
      // int offset = (start_address) & 0xffff0000;
      // map("/dev/mem", offset, 0x1000000);      
     volatile unsigned char addr;
      
     addr = (off-0x10000000) ^ 0b11;
     //printf("[w]:0x%x(0x%x),0x%x\r\n",off,addr,v);
     *((volatile unsigned char *)g_address + addr) = (unsigned char)v;
}

unsigned char reg_read(unsigned char off)
{
     volatile unsigned char addr;
     unsigned char v;
     
     addr = (off-0x10000000) ^ 0b11;
     v = *((volatile unsigned char *)g_address + addr);
     
     //printf("[r]:0x%x(0x%x),0x%x\r\n",off,addr,v);
     return v;
}
