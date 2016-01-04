/******************************************************************************

                               Copyright (c) 2011
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

/** \defgroup DRV_PEF24628E_NO_OS Device Driver NoOS Adaptation */
/*@{*/
/**
   \file 
   Adapation of NoOS system.
*/
#include "lib_sysdep.h"
#include "sys_noOSLib.h"

#include "drv_api.h"
#include "drv_pef24628e_interface.h"
#include "drv_pef24628e_api.h"

//#ifdef NO_OS
#ifdef LINUX

LOCAL int Pef24628eDrvNum = 0;

LOCAL void Pef24628e_Int ( PEF24628E_DEV_t * pDev );
LOCAL INT Pef24628e_Open ( CHAR * pPath, INT flags );
LOCAL INT Pef24628e_Close ( PEF24628E_DEV_t * pDev );
LOCAL INT Pef24628e_Read ( PEF24628E_DEV_t * pDev, CHAR * pDst, INT nLength );
LOCAL INT Pef24628e_Write ( PEF24628E_DEV_t * pDev, CHAR * pSrc, INT nLength );
LOCAL INT Pef24628e_IoCtl ( PEF24628E_DEV_t * pDev, UINT nCmd, UINT arg );

typedef VOID(*IntHandler_t)(INT);

/**
   Open a PEF24628E device.
   Function to obtain a filedescriptor to a PEF24628E device in the noOS.

   \param pPath   - Device name string used during open() call.
   \param flags   - Additional flags (unused).

   \return
   - Pointer to device specific structure (OS-internally)
   - ERR in case of failure.
*/
LOCAL INT Pef24628e_Open ( CHAR * pPath, INT flags )
{
   PEF24628E_DEV_t *pDev = NULL;

   TRACE ( PEF24628E_DRV, DBG_LEVEL_LOW, ( PREFIX "open '%s'" CRLF, pPath ) );

   /* get the device structure */
   if ( memcmp ( pPath, DRV_PEF24628E_NAME, strlen ( DRV_PEF24628E_NAME ) ) == 0 )
   {
      pPath += strlen ( pPath ) - 1;

      printf ( "Search for device #%c" CRLF, *pPath );
      /* FIXME: currently only one digit */
      if ( ( *pPath >= 0x30 ) && ( *pPath <= 0x39 ) )
      {
         printf ( "Found device #%c(%d)" CRLF, *pPath, *pPath - 0x30 );
         pDev = Pef24628e_Devices[*pPath - 0x30];
      }
      else
      {
         return ( ERROR );
      }
   }

   if ( !pDev )
      return ( ERROR );

   if ( pDev->bOpen == IFX_TRUE )
   {
      return ( ERROR );
   }

   pDev->bOpen = IFX_TRUE;
   return ( int ) pDev;
}

/**
   Function to close an opened filedescriptor in the noOS.

   \param   pDev  - device specific structure.

   \return
   - SUCCESS
   - ERROR
*/
LOCAL INT Pef24628e_Close ( PEF24628E_DEV_t * pDev )
{
   TRACE ( PEF24628E_DRV, DBG_LEVEL_LOW, ( PREFIX "close" CRLF ) );

   if ( pDev->bOpen == IFX_TRUE )
   {
      return ( ERROR );
   }

   pDev->bOpen = IFX_FALSE;
   pDev->bInit = IFX_FALSE;

   if ( pDev->irq_vector )
   {
      No_OS_IntDisable ( pDev->irq_vector );
      No_OS_UninstallIntHandler ( pDev->irq_vector );
   }

   if ( pDev->TxFifo.pData )
      SOC4E_MemFree ( (unsigned char *)pDev->TxFifo.pData );

   if ( pDev->FifoBuffer )
      SOC4E_MemFree ( pDev->FifoBuffer );

   return ( SUCCESS );
}


/**
   Read a message from the device
   
   \param pDev    - device structure
   \param pDst    - pointer to message buffer
   \param nLength - buffer length

\return
   - length of read message
   - ERROR
*/
LOCAL INT Pef24628e_Read ( PEF24628E_DEV_t * pDev, CHAR * pDst, INT nLength )
{
   IFX_int32_t nBytes = 0;

   //TRACE ( PEF24628E_DRV, DBG_LEVEL_LOW, ( PREFIX "read" CRLF ) );

   nBytes = Pef24628e_Get_IDC_Msg ( pDev, (IFX_uint8_t *)pDst, nLength );

   if ( nBytes < 0 )
   {
      return ERROR;
   }

   return nBytes;
}


/**
   Write a message to the device

   \param pDev    - device structure
   \param pSrc    - pointer to message buffer
   \param nLength - message length
   
\return
   - number of sent bytes or error
   - ERROR
*/
LOCAL INT Pef24628e_Write ( PEF24628E_DEV_t * pDev, CHAR * pSrc, INT nLength )
{
   IFX_int32_t nBytes = 0;
   //TRACE ( PEF24628E_DRV, DBG_LEVEL_LOW, ( PREFIX "write" CRLF ) );

   /* Send complete frame */
   nBytes = Pef24628e_Send_IDC_Msg ( pDev, (IFX_uint8_t*)pSrc, nLength );

   if ( nBytes < 0 )
   {
      return ERROR;
   }

   /* Wait for response */

   return nBytes;
}

/**
   NoOS implementation of I/O-control interface for PEF24628E.

   \param pDev    - device specific structure
   \param nCmd    - I/O-control command (s. drv_pef24628e_interface.h)
   \param arg     - Optional argument depending of I/O-control command

   \return
   - OK
   - ERROR

   \remark
   If a function call returns with failure, the error status code is
     stored in the error status variable of the device.
*/
LOCAL INT Pef24628e_IoCtl ( PEF24628E_DEV_t * pDev, UINT nCmd, UINT arg )
{
   IFX_int32_t ret = OK;
   PEF24628E_REG_IO_t *pReg_Io;
   PEF24628E_DOWNLOAD_FW_t *pFWDownload;
   PEF24628E_DEV_INIT_t *pDevCfg;

   switch ( nCmd )
   {
   case FIO_PEF24628E_DEBUGLEVEL:
      if ( ( arg >= DBG_LEVEL_LOW ) && ( arg <= DBG_LEVEL_OFF ) )
      {
         TRACE ( PEF24628E_DRV, DBG_LEVEL_HIGH, ( PREFIX "DebugLevel %d" CRLF, ( int ) arg ) );
         SetTraceLevel ( PEF24628E_DRV, ( IFX_uint32_t ) arg );
         SetLogLevel ( PEF24628E_DRV, ( IFX_uint32_t ) arg );
         TRACE ( PEF24628E_DRV, DBG_LEVEL_HIGH,
                 ( PREFIX "DebugLevel Check - DBG_LEVEL_HIGH" CRLF ) );
         TRACE ( PEF24628E_DRV, DBG_LEVEL_NORMAL,
                 ( PREFIX "DebugLevel Check - DBG_LEVEL_NORMAL" CRLF ) );
         TRACE ( PEF24628E_DRV, DBG_LEVEL_LOW, ( PREFIX "DebugLevel Check - DBG_LEVEL_LOW" CRLF ) );
      }
      break;

   case FIO_PEF24628E_GET_VERSION:
      strncpy ( ( char * ) arg, DRV_PEF24628E_VER_STR, 80 );
      break;

   case FIO_PEF24628E_SET_CONFIG:
      Pef24628e_Set_Config ( pDev, ( PEF24628E_CONFIG_t * ) arg );
      break;

   case FIO_PEF24628E_GET_CONFIG:
      Pef24628e_Get_Config ( pDev, ( PEF24628E_CONFIG_t * ) arg );
      break;

   case FIO_PEF24628E_SET_IF:
      Pef24628e_Set_If ( pDev, ( PEF24628E_IF_t ) arg );
      break;

   case FIO_PEF24628E_GET_IF:
      Pef24628e_Get_If ( pDev, ( PEF24628E_IF_t * ) arg );
      break;

   case FIO_PEF24628E_SET_REG:
      pReg_Io = ( PEF24628E_REG_IO_t * ) arg;
      if ( !Pef24628e_Set_Register ( pDev, pReg_Io->addr, pReg_Io->value ) )
         ret = ERROR;
      break;

   case FIO_PEF24628E_GET_REG:
      pReg_Io = ( PEF24628E_REG_IO_t * ) arg;
      if ( !Pef24628e_Get_Register ( pDev, pReg_Io->addr, ( IFX_uint32_t * ) & pReg_Io->value ) )
         ret = ERROR;
      break;

   case FIO_PEF24628E_DOWNLOAD_FW:
      pFWDownload = ( PEF24628E_DOWNLOAD_FW_t * ) arg;
      switch ( pFWDownload->core )
      {
      case PEF24628E_IDC:
         if ( !Pef24628e_IDC_Download ( pDev, pFWDownload->pFwImage, pFWDownload->size_byte ) )
            ret = ERROR;
         if ( !Pef24628e_IDC_Start ( pDev ) )
            ret = ERROR;
         break;

      case PEF24628E_DSL:
         if ( !Pef24628e_DSL_Download ( pDev, pFWDownload->pFwImage, pFWDownload->size_byte ) )
            ret = ERROR;
         break;

      default:
         break;
      }
      break;

   case FIO_PEF24628E_DEV_INIT:
      if ( !pDev->bInit )
      {
         pDevCfg = ( PEF24628E_DEV_INIT_t * ) arg;
         pDev->irq_vector = pDevCfg->irq_vector;

         if ( pDevCfg->irq_vector && No_OS_InstallIntHandler ( pDevCfg->irq_vector,
                                                               ( IntHandler_t )Pef24628e_Int,
                                                               ( int ) pDev ) == ERROR )
         {
            LOG ( PEF24628E_DRV, DBG_LEVEL_HIGH, ( ERR_PREFIX "cannot connect interrupt." CRLF ) );
            return ( ERROR );
         }

         if ( Pef24628e_Init ( pDev, pDevCfg ) == IFX_FALSE )
         {
            LOG ( PEF24628E_DRV, DBG_LEVEL_HIGH,
                  ( ERR_PREFIX "unable to initialize device." CRLF ) );
            return ( ERROR );
         }

         if ( pDevCfg->irq_vector )
         {
            No_OS_IntEnable ( pDevCfg->irq_vector );
         }
         
         /* set SCI interface to half duplex (for a working collision detection) */
         Pef24628e_Send_Aux_Msg ( pDev, CMD_WR_REG_AUX_SCI_IF_MODE, 0x0A /*0x03*/ );
                  
      }
      break;

   case FIO_PEF24628E_POLL_CHECK:
      ret = Pef24628e_Poll_Check ( pDev );
      break;

   case FIO_PEF24628E_WRITE_AUX:
      if ( !pDev->bInit )
      {
         pReg_Io = ( PEF24628E_REG_IO_t * ) arg;
         if ( !Pef24628e_Send_Aux_Msg
              ( pDev, ( IFX_int8_t ) ( pReg_Io->addr ), ( IFX_int8_t ) ( pReg_Io->value ) ) )
            ret = ERROR;
      }
      break;

   case FIO_PEF24628E_READ_AUX:
      pReg_Io = ( PEF24628E_REG_IO_t * ) arg;
      if ( !Pef24628e_Send_Aux_Msg
           ( pDev, ( IFX_int8_t ) ( pReg_Io->addr ), ( IFX_int8_t ) ( pReg_Io->value ) ) )
         ret = ERROR;
      pReg_Io->value = pDev->last_aux_value;
      break;

#if PEF24628E_USE_SCI_MONITOR == 1
   case FIO_PEF24628E_SCI_MONITOR:
      {
         if ( arg )
         {
            /* Set SCI monitor to promiscious mode */
            /* bit positions of addr are not compared, if they are set to '1' in mask */
            pDev->bSCIMonitor = Pef24628e_Set_Address_Filter ( pDev, 0xFFFF, 0xFFFF );
         }
         else
         {
            /* Set normal mode */
            Pef24628e_Set_Address_Filter ( pDev, 0x00FF, 0xF3FF );
            pDev->bSCIMonitor = IFX_FALSE;
         }
      }
      break;
#endif

   default:
      TRACE ( PEF24628E_DRV, DBG_LEVEL_HIGH,
              ( ERR_PREFIX "Unknown IoCtl (0x%08X)." CRLF, ( unsigned int ) nCmd ) );
      ret = ERROR;
   }

   return ret;
}

/**
   OS specific interrupt handler
*/
LOCAL void Pef24628e_Int ( PEF24628E_DEV_t * pDev )
{
   Pef24628e_IntHandler ( pDev );
}

/**
   Initialization of the driver

   \return
   - OK 
   - ERROR
*/
INT Pef24628e_DevCreate ( void )
{
   PEF24628E_DEV_t *pDevice = NULL;
   char buf[64];
   IFX_uint32_t dev_no;
   No_OS_DrvOp DrvOp;

   DrvOp.open  = ( NO_OS_OPEN ) Pef24628e_Open;
   DrvOp.close = ( NO_OS_CLOSE ) Pef24628e_Close;
   DrvOp.ioctl = ( NO_OS_IOCTL ) Pef24628e_IoCtl;
   DrvOp.read  = ( NO_OS_READ ) Pef24628e_Read;
   DrvOp.write = ( NO_OS_WRITE ) Pef24628e_Write;

   SetTraceLevel ( PEF24628E_DRV, DBG_LEVEL_LOW );
   SetLogLevel ( PEF24628E_DRV, DBG_LEVEL_HIGH );

   printf( "%s" CRLF, &PEF24628E_WHATVERSION[4] );
   printf( "(c) Copyright 2012, Lantiq Deutschland GmbH\n\n\r" );

   /* Driver already installed */
   if ( Pef24628eDrvNum <= 0 )
   {
      /* add driver to driver table */
      Pef24628eDrvNum = No_OS_InstallDriver ( &DrvOp );

      if ( Pef24628eDrvNum == ERROR )
      {
         LOG ( PEF24628E_DRV, DBG_LEVEL_HIGH, ( ERR_PREFIX "unable to install the driver." CRLF ) );
         return ( ERROR );
      }
   }
   else
   {
      LOG ( PEF24628E_DRV, DBG_LEVEL_HIGH, ( ERR_PREFIX "driver already installed." CRLF ) );
      return ( ERROR );
   }

   for ( dev_no = 0; dev_no < PEF24628E_MAX_DEV_NUMBER; dev_no++ )
   {
      pDevice = ( PEF24628E_DEV_t * ) malloc ( sizeof ( PEF24628E_DEV_t ) );

      if ( !pDevice )
      {
         LOG ( PEF24628E_DRV, DBG_LEVEL_HIGH,
               ( ERR_PREFIX "no memory for device structure." CRLF ) );
         return ( ERROR );
      }

      memset ( ( char * ) pDevice, 0, sizeof ( PEF24628E_DEV_t ) );

      Pef24628e_Devices[dev_no] = pDevice;

      /* memorize the number */
      pDevice->uc_index = dev_no;

      pDevice->SciRxState = SCI_RX_IDLE;

      sprintf ( buf, "%s_%d", DRV_PEF24628E_NAME, ( int ) dev_no );

      /* Add device to the noOS device list */
      if ( No_OS_AddDevice ( buf, Pef24628eDrvNum ) == ERROR )
      {
         LOG ( PEF24628E_DRV, DBG_LEVEL_HIGH, ( ERR_PREFIX "unable to create device." CRLF ) );
         goto PEF24628E_DEV_ERROR;
      }
      else
        printf("driver %s registered\r\n",buf);
   }

   return ( OK );

 PEF24628E_DEV_ERROR:

   Pef24628e_DevDelete (  );

   return ( ERROR );
}

/**
   Remove the driver from the system.

   \return
   - OK
*/
INT Pef24628e_DevDelete ( void )
{
   IFX_uint32_t dev_no;
   char buf[64];

   for ( dev_no = 0; dev_no < PEF24628E_MAX_DEV_NUMBER; dev_no++ )
   {
      if ( Pef24628e_Devices[dev_no] != NULL )
      {
         /* Disable interrupt */
         if ( Pef24628e_Devices[dev_no]->irq_vector )
         {
            No_OS_IntDisable ( Pef24628e_Devices[dev_no]->irq_vector );
            No_OS_UninstallIntHandler ( Pef24628e_Devices[dev_no]->irq_vector );
         }

         /* remove device */
         sprintf ( buf, "%s/%d", DRV_PEF24628E_NAME, ( int ) dev_no );

         No_OS_DeleteDevice ( buf );
         free ( Pef24628e_Devices[dev_no] );
         Pef24628e_Devices[dev_no] = NULL;
      }
   }

   /* remove the driver also */
   No_OS_UninstallDriver ( Pef24628eDrvNum );
   Pef24628eDrvNum = 0;

   return ( OK );
}

#endif /* NO_OS */
