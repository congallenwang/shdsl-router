/******************************************************************************

                               Copyright (c) 2005
                            Infineon Technologies AG
                  St. Martin Strasse 53; 81669 Munich, Germany

  THE DELIVERY OF THIS SOFTWARE AS WELL AS THE HEREBY GRANTED NON-EXCLUSIVE, 
  WORLDWIDE LICENSE TO USE, COPY, MODIFY, DISTRIBUTE AND SUBLICENSE THIS 
  SOFTWARE IS FREE OF CHARGE.

  THE LICENSED SOFTWARE IS PROVIDED "AS IS" AND INFINEON EXPRESSLY DISCLAIMS 
  ALL REPRESENTATIONS AND WARRANTIES, WHETHER EXPRESS OR IMPLIED, INCLUDING 
  WITHOUT LIMITATION, WARRANTIES OR REPRESENTATIONS OF WORKMANSHIP, 
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, DURABILITY, THAT THE 
  OPERATING OF THE LICENSED SOFTWARE WILL BE ERROR FREE OR FREE OF ANY THIRD 
  PARTY CLAIMS, INCLUDING WITHOUT LIMITATION CLAIMS OF THIRD PARTY INTELLECTUAL 
  PROPERTY INFRINGEMENT. 

  EXCEPT FOR ANY LIABILITY DUE TO WILLFUL ACTS OR GROSS NEGLIGENCE AND EXCEPT 
  FOR ANY PERSONAL INJURY INFINEON SHALL IN NO EVENT BE LIABLE FOR ANY CLAIM 
  OR DAMAGES OF ANY KIND, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
  DEALINGS IN THE SOFTWARE.

******************************************************************************/

/** \file
   Map the compact package calls to the driver calls.
*/

/* ============================= */
/* Includes                      */
/* ============================= */

#include "ifx_types.h"
#include "lib_sysdep.h"
#include "lib_soc4e_map.h"
#include "idc_onlinemessages.h"
#include "drv_pef24628e_osmap.h"

#include "board.h"
#include "sys_noOSLib.h"

#define PRINTF printf
#define WAIT usleep

IFXOS_PRN_USR_MODULE_CREATE(PEF24624_LIB,IFXOS_PRN_LEVEL_HIGH);

#ifdef UBOOT
extern CHAR *fw_sdfe4_filename;
extern const UINT8 fw_sdfe4_data[];
extern UINT32 fw_sdfe4_len;

extern CHAR *fw_idc_filename;
extern const UINT8 fw_idc_data[];
extern UINT32 fw_idc_len;
#endif

/** SOC4E device descriptors */
LOCAL int fd[ PEF24628E_MAX_DEV_NUMBER ];
LOCAL UINT8 irq[ PEF24628E_MAX_DEV_NUMBER ] = {IRQ_INIT};
LOCAL UINT32 base_address[ PEF24628E_MAX_DEV_NUMBER ] = BASE_ADDRESS_INIT;
/** Indicator that the message EVT_InitializationComplete was reveived */
LOCAL int evt_initializationcomplete[ PEF24628E_MAX_DEV_NUMBER ];
/** if set the SCI monitor is activ */
LOCAL BOOL sci_mon_state = FALSE;

#ifdef LINUX
static int soc4e_load_firmware
(
   CHAR const *pName,
   UINT8 **pFirmware, 
   UINT32 *nFirmwareSize
);
#endif


/* ============================= */
/* Global function definition    */
/* ============================= */
int reg_init();
VOID soc4e_sci_monitor_dump(const UINT8 *pData, UINT32 nLength);

BOOL soc4e_decode_upstream(UINT8 ch, UINT16 msg_id, SDFE4_t * pMsg, UINT16 nLength)
{
    return 0;
}

/**
   Initialize the required interfaces and starts the download of the firmware.
\return
   - TRUE    Success
   - FALSE   in case of error
*/
BOOL soc4e_drv_init(const BOARD_Configuration_t* pDeviceConfiguration)
{
   BOOL ret = TRUE;
   UINT8 device, i;
   CHAR buf[64];
   PEF24628E_DOWNLOAD_FW_t idc_fw;
   PEF24628E_DOWNLOAD_FW_t sdfe4_fw;
   PEF24628E_DEV_INIT_t    init;   
   PEF24628E_IF_t nInterfaceMode = pDeviceConfiguration->nInterfaceMode;
   
   idc_fw.core = PEF24628E_IDC;
   sdfe4_fw.core = PEF24628E_DSL;
#ifdef UBOOT
   idc_fw.pFwImage = (UINT8 HUGE *)fw_idc_data;
   idc_fw.size_byte = fw_idc_len;     
   sdfe4_fw.pFwImage = (UINT8 HUGE *)fw_sdfe4_data;
   sdfe4_fw.size_byte = fw_sdfe4_len;     
#else
   idc_fw.pFwImage = (UINT8 *)0;
   idc_fw.size_byte = 0;     
   sdfe4_fw.pFwImage = (UINT8 *)0;
   sdfe4_fw.size_byte = 0;     
#endif
   
   /* Create device driver */
   if ( Pef24628e_DevCreate() != 0 ) 
      return FALSE;

#ifdef LINUX
//   set_keypress();
//   echo_off();
#endif

   /* load DSL firmware */
#ifdef UBOOT
   if ( *(UINT32 HUGE *)DSL_FIRMWARE_FLASH_ADDRESS != 0xFFFFFFFF )
   {
      sdfe4_fw.pFwImage = (UINT8 HUGE *)(DSL_FIRMWARE_FLASH_ADDRESS+4);
      sdfe4_fw.size_byte = *(UINT32 HUGE *)DSL_FIRMWARE_FLASH_ADDRESS;
      PRINTF("SOC4E: DSL firmware download from flash: %d bytes, flash address 0x%08x\n\r", 
                                                  sdfe4_fw.size_byte, 
                                                  (UINT8 HUGE *)(sdfe4_fw.pFwImage));
   }
   if ( *(UINT32 HUGE *)IDC_FIRMWARE_FLASH_ADDRESS != 0xFFFFFFFF )
   {
      /* Load firmware */
      idc_fw.core = PEF24628E_IDC;
      idc_fw.pFwImage = (UINT8 HUGE *)(IDC_FIRMWARE_FLASH_ADDRESS+4);
      idc_fw.size_byte = *(UINT32 HUGE *)IDC_FIRMWARE_FLASH_ADDRESS;

      PRINTF("SOC4E: IDC firmware download from flash: %d bytes, flash address 0x%08x\n\r", 
                                                      idc_fw.size_byte, idc_fw.pFwImage);
   }                                                      
#endif

#ifdef LINUX
   if(soc4e_load_firmware("sdfe4.bin", &sdfe4_fw.pFwImage, &sdfe4_fw.size_byte))
   {
      PRINTF("SOC4E: DSL firmware download from %s: %lu bytes\n\r", 
                                                  "sdfe4.bin", 
                                                  sdfe4_fw.size_byte);
   }

   if(soc4e_load_firmware("idc.bin", &idc_fw.pFwImage, &idc_fw.size_byte))
   {
      PRINTF("SOC4E: IDC firmware download from %s: %lu bytes\n\r", 
                                                  "idc.bin", 
                                                  idc_fw.size_byte);
   }
#endif

   if ( nInterfaceMode == PEF24628E_IF_SCI ) {
      
#ifdef UBOOT
      /* PEF24628E_IF_SCI doesn't supported */
      PRINTF("SOC4E: PEF24628E_IF_SCI doesn't supported under U-BOOT, PEF24628E_IF_MPI will be used instead.\n\r");
      nInterfaceMode = PEF24628E_IF_MPI;
#else /* for VxVorks and LINUX */
      /* TODO: create hdlc device */
#endif
   }

   for ( device = 0 ; device < pDeviceConfiguration->nMaxDevNumber; device++ )
   {
      PRINTF("SOC4E[%02d]: open device %s_%d\n\r", device, PEF24628E_NAME, device);
      /* Open device driver */
      #ifdef LINUX
      /* prepare the name of the device */
      sprintf( buf,"%s_%d", PEF24628E_NAME, device);
      if ( ( fd[ device ] = No_OS_Open( buf, 1 )) == -1 ) {
      #endif   

#if 0
      #ifdef UBOOT
      /* prepare the name of the device */
      sprintf( buf,"%s_%d", PEF24628E_NAME, device);
      if ( ( fd[ device ] = open( buf, 0 )) == -1 ) {
         Pef24628e_DevDelete();
      #endif
#endif      
         /*ret = FALSE; ignore missing devices */
         continue;
      }
      
      PRINTF("SOC4E[%02d]: Select interface %d\n\r", device, nInterfaceMode);
      /* Select interface */
      if ( ioctl( fd[ device ], FIO_PEF24628E_SET_IF, (UINT32)nInterfaceMode ) )
      {
         PRINTF("SOC4E[%02d]: Interface selection has failed\n\r", device);
         ret = FALSE;
         continue;
      }

      init.irq_vector = 0;    
      init.reg_offset = 0;
      init.sci_clock = pDeviceConfiguration->nSciClock;
   
      if (nInterfaceMode == PEF24628E_IF_MPI) {
        
         if(!pDeviceConfiguration->bPollingMode)
         {
            init.irq_vector = irq[ device ];
         }  
         init.reg_offset = base_address[ device ];
      }
   
      PRINTF("SOC4E[%02d]: Init device driver, base 0x%x, irq %d\n\r", 
               device, init.reg_offset, init.irq_vector);

      //allen, bus init
      reg_init();
      
      if ( ioctl( fd[ device ], FIO_PEF24628E_DEV_INIT, (UINT32)&init ) )
      {
         PRINTF("SOC4E[%02d]: Device init failed\n\r", device);
         ret = FALSE;
         continue;
      }

      if ( ioctl( fd[ device ], FIO_PEF24628E_DOWNLOAD_FW, (UINT32)&sdfe4_fw ) )
      {
         PRINTF("SOC4E[%02d]: DSL download failed\n\r", device);
         ret = FALSE;
         continue;
      }
      if ( ioctl( fd[ device ], FIO_PEF24628E_DOWNLOAD_FW, (UINT32)&idc_fw ) )
      {
         PRINTF("SOC4E[%02d]: IDC download failed\n\r", device);
         ret = FALSE;
         continue;
      }
   }
   
   /* InventoryRequest */
   TRACE(PEF24624_LIB,DBG_LEVEL_HIGH,("CMD_INVENTORYREQUEST\n\r"));
   for (device=0; device<pDeviceConfiguration->nMaxDevNumber; device++)
   {
      if (soc4e_send_idc_msg(device, CMD_INVENTORYREQUEST, NULL, 0) == FALSE)
         TRACE(PEF24624_LIB,DBG_LEVEL_HIGH,
            ("soc4e_send_idc_msg() failed ... \n\r"));
   }         

   for(i=0;i<20;i++)
   {
      for (device=0; device<pDeviceConfiguration->nMaxDevNumber; device++)
      {
         soc4e_drv_poll(device, 0);
      }
      for (device=0; device<pDeviceConfiguration->nMaxDevNumber; device++)
      {
         if(evt_initializationcomplete[device] == 0)
         {
            break;
         }
      }
      if(device == pDeviceConfiguration->nMaxDevNumber)
      {
         ret = TRUE;
         break;
      }
      WAIT(100000);      
   }

   return ret;
}


/**
   Initialize the required interfaces and starts the download of the firmware.
\return
   - TRUE    Success
   - FALSE   in case of error
*/
BOOL soc4e_drv_exit(VOID)
{
   BOOL ret = TRUE;
   UINT8 device;
   
   /* Close device driver */
   for ( device = 0; device < PEF24628E_MAX_DEV_NUMBER; device++ )
   {
      if( fd[ device ] <= 0)
         continue;
         
      close( fd[ device ] );

      fd[ device ] = 0;
   }
   
   /* Delete device driver */
   if ( Pef24628e_DevDelete() != 0 ) 
      return FALSE;

#ifdef LINUX
//   reset_keypress();
//   echo_on();
#endif

   return ret;
}


/**
   Checks ack from AUX or RAMSHELL.

\return
   - TRUE    positive acknoledge received
   - FALSE   in case of error

\remarks
   Checking for ack will discard all unknown messages.
   This is only useful during startup/before active firmware.
*/
BOOL soc4e_check_aux_rs_ack(VOID)
{
   BOOL ret = TRUE;
   return ret;
}


/**
   Creates an AUX message from parameters and sends it to SCI.

\param  reg    register address in AUX
\param  param  value to set
\param  check  wait until ack received

\return
   - TRUE    Success
   - FALSE   in case of error
   
\remarks
   Checking for ack will discard all unknown messages.
   This is only useful during startup/before active firmware.
*/
BOOL soc4e_send_aux_msg(UINT8 device, UINT8 reg, UINT8 param, BOOL check)
{
   BOOL ret = TRUE;

/*
   PRINTF("SOC4E: SDFE4-AUX: Write aux reg:0x%04X param:0x%04X\n\r", reg, param);
*/
   return ret;
}


/**
   Creates a RAMSHELL message from parameters and sends it to SCI.
   Only used for firmware download.

\param opcd    operation code of ramshell message
\param param   value
\param pSrc    pointer to additional data
\param nLength length of additional data
\param check   wait until ack received

\return
   - TRUE    Success
   - FALSE   in case of error
   
\remarks
   Checking for ack will discard all unknown messages.
   This is only useful during startup/before active firmware.
*/
BOOL soc4e_send_rs_msg(UINT8 device, UINT8 opcd, UINT32 param, UINT8 *pSrc, INT16 nLength, BOOL check)
{
   BOOL ret = TRUE;

/*
   PRINTF("SOC4E: SDFE4-RS: Write rs opcd:0x%04X param:0x%04X\n\r", opcd, param);
*/
   return ret;
}


/**
   Creates a PamDSL message from parameters and sends it to SCI.

\param ch      channel number
\param msg_id  message id
\param pSrc    pointer to additional message params
\param nLength length of additional message params

\return
   - TRUE   Success
   - FALSE  in case of error
*/
BOOL soc4e_send_pamdsl_msg(UINT8 ch, UINT16 msg_id, VOID const * pSrc, UINT16 nLength)
{
   UINT8 msg[121*sizeof( UINT32 )];
   UINT16 tunnel_msg_id;
   UINT8 line = ch % PEF24628E_MAX_LINES_PER_DEVICE;   
   UINT8 device = ch / PEF24628E_MAX_LINES_PER_DEVICE;   
   
   tunnel_msg_id = CMD_TNL_PAMDSL_MSG_ID(line);
   
   TRACE(PEF24624_LIB, DBG_LEVEL_LOW,
      ("SOC4E[%02d-%02d]: write tunnel_msg_id: 0x%04X, msg id: 0x%04X, len:%d\n\r", 
         device, line, tunnel_msg_id, msg_id, nLength));

   msg[0] = msg_id & 0xFF;
   msg[1] = ( msg_id >> 8 ) & 0xFF;
   msg[2] = 0;
   msg[3] = 0;
   memcpy( &msg[4], pSrc, nLength );
         
   /* add padding bytes */                                              
   if(nLength & 0x0003)
   {
      nLength &= ~0x0003;
      nLength += 4;
   }

   if( soc4e_send_idc_msg( device, tunnel_msg_id, &msg[0], nLength + 4) == FALSE)
   {
      TRACE(PEF24624_LIB,DBG_LEVEL_HIGH,
         ("SOC4E[%02d-%02d]: SDFE4 msg id: 0x%04X failed\n\r", device, line, msg_id));
      return FALSE;
   }
   return TRUE;
}

/**
   Poll for messages received from SOC4E.
   
\param idc_msg_id_expected    message identifier which is expected to receive
   
\return
   - TRUE   Success
   - FALSE  in case of error
*/
BOOL soc4e_drv_poll( UINT8 device, UINT16 idc_msg_id_expected )
{
   UINT8 pBuf[PEF24628E_MSG_SIZE];
   UINT16 nBytes, m_id, idc_msg_id, len = 0;
   SDFE4_MessageHeader_t *pSDFE4_Header = (SDFE4_MessageHeader_t *)(pBuf + sizeof(IDC_MessageHeader_t));
   SDFE4_t *pSDFE4_Data = (SDFE4_t *)(pBuf + sizeof(IDC_MessageHeader_t) + sizeof(SDFE4_MessageHeader_t));  

   UINT32 retries = 100;
   UINT8 line = PEF24628E_MAX_LINES_PER_DEVICE * device;
   
   if(fd[ device ] <= 0)
      return FALSE;


   
   while (--retries)
   {
      WAIT(5000);

      if( ioctl( fd[ device ], FIO_PEF24628E_POLL_CHECK, 0 ) == 0)
      {
         if(idc_msg_id_expected == 0)
         {
            /* ony serving notifications, not waiting for special message */
            return TRUE;         
         }
      
         break;;
      }
   
      nBytes = No_OS_Read( fd[ device ], (INT8*) &pBuf[0], PEF24628E_MSG_SIZE );
      
      /* check for minimum size */
      if ( nBytes < 3 ) 
      {
         PRINTF("SOC4E[%02d]: IDC: Message length failed %d\n\r", device, nBytes);
         /*return FALSE;*/
         break;
      }

      if ( pBuf[ nBytes - 1 ] == 0xA1 ) {

         PRINTF("SOC4E[%02d]: SCI MONITOR: Message length = %d\n\r", device, nBytes );
         soc4e_sci_monitor_dump( (UINT8 *)&pBuf[0], nBytes);
         continue;
      } else if ( pBuf[ nBytes - 1 ] == 0xA0 ) {
         PRINTF("SOC4E[%02d]: ORDINAL MESSAGE: Message length = %d\n\r", device, nBytes );
      } else if ( sci_mon_state == TRUE ) {
         PRINTF("SOC4E[%02d]: ERROR: Message length = %d\n\r", device, nBytes );
         soc4e_sci_monitor_dump( (UINT8 *)&pBuf[0], nBytes);
      }

      idc_msg_id = ((pBuf[0] >> 5) & 0x07) | (pBuf[1] << 3);
      m_id = (pSDFE4_Header->header[1] << 8) | pSDFE4_Header->header[0];
      if(len > (sizeof(IDC_MessageHeader_t) + sizeof(SDFE4_MessageHeader_t) + 1))
         len = nBytes - 1 - sizeof(IDC_MessageHeader_t) - sizeof(SDFE4_MessageHeader_t); 
   
      /* ignore device address */
      switch ( idc_msg_id ) {
      
         case NFC_TNL_PMD_0_MESSAGE:
            PRINTF("SOC4E[%02d]: NFC_TNL_PMD_0_MESSAGE\n\r", device);
            soc4e_decode_upstream( line + 0, m_id, pSDFE4_Data, len);
            break;
              
         case ACK_TNL_PMD_0_MESSAGE:
            PRINTF("SOC4E[%02d]: ACK_TNL_PMD_0_MESSAGE\n\r", device);
            soc4e_decode_upstream( line + 0, m_id, pSDFE4_Data, len);
            break;

         case NFC_TNL_PMD_1_MESSAGE:
            PRINTF("SOC4E[%02d]: NFC_TNL_PMD_1_MESSAGE\n\r", device);
            soc4e_decode_upstream( line + 1, m_id, pSDFE4_Data, len);
            break;

         case ACK_TNL_PMD_1_MESSAGE:
            PRINTF("SOC4E[%02d]: ACK_TNL_PMD_1_MESSAGE\n\r", device);
            soc4e_decode_upstream( line + 1, m_id, pSDFE4_Data, len);
            break;

         case NFC_TNL_PMD_2_MESSAGE:
            PRINTF("SOC4E[%02d]: NFC_TNL_PMD_2_MESSAGE\n\r", device);
            soc4e_decode_upstream( line + 2, m_id, pSDFE4_Data, len);
            break;

         case ACK_TNL_PMD_2_MESSAGE:
            PRINTF("SOC4E[%02d]: ACK_TNL_PMD_2_MESSAGE\n\r", device);
            soc4e_decode_upstream( line + 2, m_id, pSDFE4_Data, len);
            break;

         case NFC_TNL_PMD_3_MESSAGE:
            PRINTF("SOC4E[%02d]: NFC_TNL_PMD_3_MESSAGE\n\r", device);
            soc4e_decode_upstream( line + 3, m_id, pSDFE4_Data, len);
            break;

         case ACK_TNL_PMD_3_MESSAGE:
            PRINTF("SOC4E[%02d]: ACK_TNL_PMD_3_MESSAGE\n\r", device);
            soc4e_decode_upstream( line + 3, m_id, pSDFE4_Data, len);
            break;

         case EVT_INITIALIZATIONCOMPLETE:
            PRINTF("SOC4E[%02d]: EVT_INITIALIZATIONCOMPLETE\n\r", device);
            evt_initializationcomplete[device] = 1;
            break;
         
         case ACK_MDIO_REGISTERMULTIPLEREAD:
            PRINTF("SOC4E[%02d]: ACK_MDIO_REGISTERMULTIPLEREAD\n\r", device);
            break;

         case ACK_INVENTORYREQUEST:
            PRINTF("SOC4E[%02d]: ACK_INVENTORYREQUEST\n\r", device);
            break;
            
         default :
/*
            PRINTF( "SOC4E[%02d]: received message id: %x, src:%02X, dest%02X, len %04X bytes\n\r"
                  , device, idc_msg_id, pBuf[0], pBuf[1], nBytes );
            soc4e_sci_monitor_dump( (UINT8 *)&pBuf[0], nBytes);
*/
            break;
      }
      
      if(idc_msg_id == (idc_msg_id_expected | 0x200))
      {
         return TRUE;         
      }

      if(idc_msg_id_expected == 0)
      {
         /* ony serving notifications, not waiting for special message */
         return TRUE;         
      }
   }
   
   PRINTF("SOC4E[%02d]: poll timeout: idc_msg_id_expected %04x\n\r", device, idc_msg_id_expected);
      
   return FALSE;
}

/**
   Creates a IDC message from parameters and sends it to SCI.

\param device     device index
\param msg_id     message id
\param pSrc       pointer to additional message params
\param nLength    length of additional message params

\return
   - TRUE      Success
   - FALSE     in case of error
*/
BOOL soc4e_send_idc_msg(UINT8 device, UINT16 msg_id, VOID const * pSrc, UINT16 nLength)
{
   UINT8 msg[sizeof(SDFE4_t) + 4];
   UINT16 tc_id=0xFFFF;
   INT32 nBytes;
   
   if(fd[ device ] <= 0)
   {
      PRINTF("SOC4E[%02d]: device not present\n\r", device);
      return FALSE;
   }
   
   /* prepare IDC header */
   
   /* the length should less or equal to 30 words */
   if ( nLength > 120 ) 
   {
      PRINTF("SOC4E[%02d]: IDC: invalid length: %d\n\r", device, nLength);
      return FALSE;
   }
   
   if(nLength & 0x0003)
   {
      PRINTF("SOC4E[%02d]: IDC: non aligned length: %d\n\r", device, nLength);
      return FALSE;
   }   
   
   *(msg + 0) =  ((nLength)/4) & 0x1F;  /* 0..4 Payload Length in 32bit words*/
   *(msg + 0) |= (msg_id & 0x07)<<5;    /* 5..15 MSGID */
   *(msg + 1) =  (msg_id >> 3) & 0xFF;
   *(msg + 2) =  tc_id & 0xFF;          /* 16..26 TCID */
   *(msg + 3) =  (tc_id>>8) & 0x07;
   *(msg + 3) |= (0x00 & 0x0F) << 3;    /* RC, usually 0 */    

   /* message params must be already in little endian, no conversion !!! */
   if(pSrc && nLength)
      memcpy(&msg[4], pSrc, nLength);

   nLength += 4; 
   /* write() should be used */
   nBytes = No_OS_Write( fd[ device ], (INT8 *) &msg[0], nLength);
   if ( nBytes != nLength )
   {
      PRINTF("SOC4E[%02d]: write of nBytes %d bytes failed, %d bytes written\n\r", 
               device, nLength + 4, nBytes);
      return FALSE;
   }
   
   if (soc4e_drv_poll(device, msg_id) == TRUE) 
   {
      return TRUE;
   }

   /* message not acknowledged */
   return FALSE;
} 

/**
   Switch the SCI monitor function on. Messages on the SCI
   bus will be printed to the console.

\return
   - TRUE      Success
   - FALSE     in case of error
*/
BOOL soc4e_sci_monitor_on( VOID )
{
   sci_mon_state = TRUE;
   if ( ioctl( fd[ 0 ], FIO_PEF24628E_SCI_MONITOR, (UINT32)sci_mon_state ) )
   {
      PRINTF("SOC4E[%02d]: SCI monitor switching failed\n\r", 0);
      return FALSE;
   }

   return TRUE;
}

/**
   Switch the SCI monitor function off.

\return
   - TRUE      Success
   - FALSE     in case of error
*/
BOOL soc4e_sci_monitor_off( VOID )
{
   sci_mon_state = FALSE;
   if ( ioctl( fd[ 0 ], FIO_PEF24628E_SCI_MONITOR, (UINT32)sci_mon_state ) )
   {
      PRINTF("SOC4E[%02d]: SCI monitor switching failed\n\r", 0);
      return FALSE;
   }

   return TRUE;
}

/**
   Dump SCI message the console.

\param Data    source pointer   
\param Length  data length  

*/
VOID soc4e_sci_monitor_dump(const UINT8 *pData, UINT32 nLength)
{
    UINT32 j;

/*    printf("\n\r");*/
/*    printf("Offset  Data \n\r");*/
/*    printf("======  ================================================= \n\r");*/
    for ( j = 0; j < nLength; j++ ) 
    {
        if ( j > 15 && j % 16 == 0 ) 
        {
            printf("\n\r");
        } 
        else 
            if ( j > 7 && j % 8 == 0 ) 
            {
                printf(": ");
            }
        if (j % 16 == 0) 
        {
            printf("0x%04X  ", j);
        }
        printf("%02X ", *pData++ );
    }
    printf("\n\r");

}
  
#ifdef LINUX

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#if 0
/** 
   Linux dummy
   This function not needed because the driver should be already loaded.
*/
int Pef24628e_DevCreate(void)
{
   return 0;
}

/** 
   Linux dummy
   This function not needed because the driver should be already loaded.
*/
int Pef24628e_DevDelete(void)
{
   return 0;
}

/** 
   Linux dummy
   Not used yet.
*/
int get_timer(int oops)
{
   return 0;
}
#endif

/**
   Load firmware from file system.
   
\param pName            file name
\param pFirmware        pointer to firmware data pointer
\param nFirmwareSize    resulting firmware size

\return
   - TRUE      Success
   - FALSE     in case of error
   
\remark
   Available under Linux only.
*/
static int soc4e_load_firmware(
   CHAR const *pName,
   UINT8 **pFirmware, 
   UINT32 *nFirmwareSize
)
{
   FILE *fwfd = NULL;
   struct stat fw_stat;
   UINT8 *ptr;
   size_t size, ret;

   TRACE( PEF24624_LIB ,DBG_LEVEL_HIGH,("SOC4E: loading firmware binary %s\n\r", pName));

   if (stat(pName, &fw_stat))
   {
      TRACE( PEF24624_LIB ,DBG_LEVEL_HIGH,("SOC4E: ERROR - Error getting statistics on %s\n\r", pName));
      /* return success even if no FW was found. 
      Devices may be operable without a FW from here */
      return TRUE;
   }

   size = fw_stat.st_size;

   if (size == 0)
   {
      TRACE( PEF24624_LIB ,DBG_LEVEL_HIGH,("SOC4E: DEVICE: ERROR -  File %s has zero size.\n\r", pName));
      return TRUE;
   }
   else
   {
      TRACE( PEF24624_LIB ,DBG_LEVEL_LOW,("SOC4E: firmware binary size %d bytes\n\r", (int)size));
   }

   fwfd = fopen(pName, "rb");
   if (fwfd == NULL)
   {
      TRACE( PEF24624_LIB ,DBG_LEVEL_HIGH,("SOC4E: ERROR - cannot open firmware binary %s.\n\r", pName));
      return FALSE;
   }

   ptr = (UINT8*)malloc(size);
   if (ptr == NULL)
   {
      TRACE( PEF24624_LIB ,DBG_LEVEL_HIGH,("SOC4E: ERROR - cannot allocate memory for firmware file\n\r"));
      return FALSE;
   }

   ret = fread(ptr, (size_t)1, size, fwfd);
   if(ret != size)
   {
      TRACE( PEF24624_LIB ,DBG_LEVEL_HIGH,("SOC4E: ERROR - cannot read firmware\n\r"));
      return FALSE;
   }

   fclose(fwfd);

   *nFirmwareSize = size;
   *pFirmware = ptr;

   return TRUE;
}
#endif
