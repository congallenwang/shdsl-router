/******************************************************************************

                               Copyright (c) 2011
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/


#ifndef _DRV_PEF24628E_INTERFACE_H
#define _DRV_PEF24628E_INTERFACE_H

/** \defgroup DRV_PEF24628E_INTERFACE Device Driver User Application Interface */
/*@{*/
/**
   \file
   Interface to user application of the device driver
*/

#ifndef PEF24628E_MAX_DEV_NUMBER
/** maximum instances of Socrates-4e devices */
#define PEF24628E_MAX_DEV_NUMBER           1
#endif

/**
   Define GMXD debug levels for ioctl interface
*/
#ifndef PEF24628E_PRN_LEVEL_OFF
#define PEF24628E_PRN_LEVEL_OFF       4
#endif

#ifndef PEF24628E_PRN_LEVEL_HIGH
#define PEF24628E_PRN_LEVEL_HIGH      3
#endif

#ifndef PEF24628E_PRN_LEVEL_NORMAL
#define PEF24628E_PRN_LEVEL_NORMAL    2
#endif

#ifndef PEF24628E_PRN_LEVEL_LOW
#define PEF24628E_PRN_LEVEL_LOW       1
#endif

#define _IO(x,y) (x+y)

/* FIO modes up to 46 allocated by OS (see ioLib.h) */

/** magic number */
#define PEF24628E_IOC_MAGIC 'U'
/** Changes the level of debug outputs.

   \param UINT    The following values are valid:
                  - 1 PEF24628E_PRN_LEVEL_LOW
                  - 2 PEF24628E_PRN_LEVEL_NORMAL
                  - 3 PEF24628E_PRN_LEVEL_HIGH
                  - 4 PEF24628E_PRN_LEVEL_OFF

   \retval 0      if success
   \retval -1     in case of an error.

   \remarks
   \code
   ioctl( fd, FIO_PEF24628E_DEBUGLEVEL, DBG_LEVEL_NORMAL );
   \endcode
   \ingroup DRV_PEF24628E_INTERFACE
*/
#define FIO_PEF24628E_DEBUGLEVEL       _IO(PEF24628E_IOC_MAGIC, 101)

/** Returns the version information.

   \param char*   The parameter is a pointer to a string.

   \retval 0      if success
   \retval -1     in case of an error.

   \remarks Version string value will be set into the parameter.
   \code
   char verBuf[81] ;
   memset(&verBuf, 0x0, sizeof(verBuf));
   if ( ioctl(fd, FIO_PEF24628E_GET_VERSION, (int)&verBuf) < 0 ) {
      printf("Error while request PEF24628E version." CRLF);
   } else {
      printf("PEF24628E Version = %s" CRLF,verBuf);
   }
   \endcode

   \ingroup DRV_PEF24628E_INTERFACE
*/
#define FIO_PEF24628E_GET_VERSION      _IO(PEF24628E_IOC_MAGIC, 102)

/** Set configuration options for device.

   \param PEF24628E_CONFIG_t* The parameter points to a PEF24628E_CONFIG_t structure.

   \retval 0      if success
   \retval -1     in case of an error.

   \remarks Not implemented.
   \code
   PEF24628E_CONFIG_t config;

   !!! FILL CONFIGURATION INFO !!!

   if ( ioctl(fd, FIO_PEF24628E_SET_CONFIG, (int)&config) < 0 ) {
      printf("Device configuration failed" CRLF);
   } else {
      printf("Device configuation successful" CRLF);
   }
   \endcode
   \ingroup DRV_PEF24628E_INTERFACE
*/
#define FIO_PEF24628E_SET_CONFIG       _IO(PEF24628E_IOC_MAGIC, 111)
/** Get configuration options for device

   \param PEF24628E_CONFIG_t* The parameter points to a PEF24628E_CONFIG_t structure.

   \retval 0      if success
   \retval -1     in case of an error.

   \remarks Not implemented.
   \code
   PEF24628E_CONFIG_t config;
   if ( ioctl(fd, FIO_PEF24628E_GET_CONFIG, (int)&config) < 0 ) {
      printf("Device configuration failed" CRLF);
   } else {
      !!! PRINT CONFIGURATION INFO !!!
      printf("Device configuation successful" CRLF);
   }
   \endcode
   \ingroup DRV_PEF24628E_INTERFACE
*/
#define FIO_PEF24628E_GET_CONFIG       _IO(PEF24628E_IOC_MAGIC, 112)

/** Download firmware from the specified address.

   \param PEF24628E_DOWNLOAD_FW_t* The parameter points to a PEF24628E_DOWNLOAD_FW_t structure.
   The core type should be \ref PEF24628E_Core_t type.

   \retval 0      if success
   \retval -1     in case of an error.

   \remarks
   \code
   PEF24628E_DOWNLOAD_FW_t fw_info;

   fw_info.core = PEF24628E_DSL;
   fw_info.pFwImage = (UINT8 HUGE *)(0x40080000 + 4);
   fw_info.size_byte = *(UINT32 HUGE *)0x40080000;

   if ( ioctl(fd, FIO_PEF24628E_DOWNLOAD_FW, (int)&fw_info) < 0 ) {
      printf("Download firmware failed" CRLF);
   } else {
      printf("Download firmware successful" CRLF);
   }
   \endcode
   \ingroup DRV_PEF24628E_INTERFACE
*/
#define FIO_PEF24628E_DOWNLOAD_FW      _IO(PEF24628E_IOC_MAGIC, 113)
/** Write a register.

   \param PEF24628E_REG_IO_t* The parameter points to a \ref PEF24628E_REG_IO_t structure.

   \return 0 if success, otherwise -1 in case of an error.

   \remarks
   \code
   PEF24628E_REG_IO_t reg_io;

   reg_io.addr = 0x08;
   reg_io.value = 0x12345678;

   if ( ioctl(fd, FIO_PEF24628E_SET_REG, (int)&reg_io) < 0 ) {
      printf("Set register value failed" CRLF);
   } else {
      printf("Set register value successful" CRLF);
   }
   \endcode
   \ingroup DRV_PEF24628E_INTERFACE
*/
#define FIO_PEF24628E_SET_REG          _IO(PEF24628E_IOC_MAGIC, 114)

/** Read a register.

   \param PEF24628E_REG_IO_t* The parameter points to a PEF24628E_REG_IO_t structure.

   \retval 0      if success
   \retval -1     in case of an error.

   \remarks
   \code
   PEF24628E_REG_IO_t reg_io;

   reg_io.addr = 0x08;
   if ( ioctl(fd, FIO_PEF24628E_GET_REG, (int)&reg_io) < 0 ) {
      printf("Get register value failed" CRLF);
   } else {
      printf("Get register value successful (0x%08X)" CRLF, reg_io.value);
   }
   \endcode
   \ingroup DRV_PEF24628E_INTERFACE
*/

#define FIO_PEF24628E_GET_REG          _IO(PEF24628E_IOC_MAGIC, 115)
/** Set the interface to use.

   \param \ref PEF24628E_IF_t    The following values are valid:
                                 - 0 PEF24628E_IF_SCI
                                 - 1 PEF24628E_IF_MPI

   \retval 0      if success
   \retval -1     in case of an error.

   \remarks Should be called before the init device ioctl function.
   \code
   if ( ioctl( fd, FIO_PEF24628E_SET_IF, PEF24628E_IF_MPI ) < 0 ) {
      printf("Interface set failed" CRLF);
   } else {
      printf("Interface set successfull" CRLF);
   }

   \endcode
   \ingroup DRV_PEF24628E_INTERFACE
*/
#define FIO_PEF24628E_SET_IF           _IO(PEF24628E_IOC_MAGIC, 116)

/** Get the used interface.

   \param PEF24628E_IF_t The parameter points to a PEF24628E_IF_t type.

   \retval 0      if success
   \retval -1     in case of an error.

   \remarks
   \code
   PEF24628E_IF_t iface;
   if ( ioctl( fd, FIO_PEF24628E_GET_IF, (int)&iface ) < 0 ) {
      printf("Interface get failed" CRLF);
   } else {
      printf("Active interface is %s" CRLF, (iface == PEF24628E_IF_MPI):"MPI"?"SCI");
   }

   \endcode
   \ingroup DRV_PEF24628E_INTERFACE
*/
#define FIO_PEF24628E_GET_IF           _IO(PEF24628E_IOC_MAGIC, 117)
/** Init device: Set base address and irq number.
   \param PEF24628E_DEV_INIT_t* The parameter points to a PEF24628E_DEV_INIT_t structure.

   \retval 0      if success
   \retval -1     in case of an error.

   \remarks Should be called after the interface was selected.
   \code
   PEF24628E_DEV_INIT_t init;

   // example configuration for MPI interface mode
   // in polling mode the irq_vector should be set to zero
   init.irq_vector = 4;
   init.reg_offset = 0xC0200000;
   init.sci_clock = 0;
   init.sci_device_address = 0xFF;


   // example configuration for MPI interface mode
   // in polling mode the irq_vector should be set to zero
   init.irq_vector = 4;                 // IRQ set to 4
   init.reg_offset = 0xC0200000;        // MPI register offset is 0xC0200000
   init.sci_clock = 0;                  // SCI clock generated by SOC4E (0 == 1/64 system clock)
   init.sci_device_address = 0xFF;  // not used

   // example configuration for SCI interface mode
   init.irq_vector = 0;                 // IRQ not used
   init.reg_offset = 0;                 // MPI register not used
   init.sci_clock = 1000000;        // 1 MHz
   init.sci_device_address = 0x01;  // SOC4E device address is set to 1

   if ( ioctl(fd, FIO_PEF24628E_DEV_INIT, (int)&init) < 0 ) {
      printf("Device init failed" CRLF);
   } else {
      printf("Device init successful" CRLF);
   }
   \endcode
   \ingroup DRV_PEF24628E_INTERFACE
*/
#define FIO_PEF24628E_DEV_INIT         _IO(PEF24628E_IOC_MAGIC, 118)

/** Check availiable data.

   \param none

   \retval 0      if success
   \retval -1     in case of an error.

   \remarks
   \code
   if ( ioctl( fd[ 0 ], FIO_PEF24628E_POLL_CHECK, 0 ) == 0 ) {
      printf("No data for reading" CRLF);
   }
   \endcode
   \ingroup DRV_PEF24628E_INTERFACE
*/
#define FIO_PEF24628E_POLL_CHECK       _IO(PEF24628E_IOC_MAGIC, 119)

/** Send AUX message

   \param PEF24628E_REG_IO_t* The parameter points to a PEF24628E_REG_IO_t structure.

   \retval 0      if success
   \retval -1     in case of an error.

   \remarks
   \code
   PEF24628E_REG_IO_t aux_io;

   // AUX command ID: Write SCI interface mode - half duplex
   aux_io.addr = CMD_WR_REG_AUX_SCI_IF_MODE;
   aux_io.value = 0x3;
   if ( ioctl(fd, FIO_PEF24628E_WRITE_AUX, (int)&aux_io) < 0 ) {
      printf("Set AUX value failed" CRLF);
   } else {
      printf("Set AUX value successful" CRLF);
   }
   \endcode
   \ingroup DRV_PEF24628E_INTERFACE
*/
#define FIO_PEF24628E_WRITE_AUX        _IO(PEF24628E_IOC_MAGIC, 120)

/** Read AUX message

   \param PEF24628E_REG_IO_t* The parameter points to a PEF24628E_REG_IO_t structure.

   \retval 0      if success
   \retval -1     in case of an error.

   \remarks
   \code
   PEF24628E_REG_IO_t aux_io;

   // AUX command ID: Read SCI interface mode - half duplex
   aux_io.addr = CMD_RD_REG_AUX_SCI_IF_MODE;
   if ( ioctl(fd, FIO_PEF24628E_READ_AUX, (int)&aux_io) < 0 ) {
      printf("Read AUX value failed" CRLF);
   } else {
      printf("Read AUX value successful (0x%08X)" CRLF, aux_io.value);
   }
   \endcode
   \ingroup DRV_PEF24628E_INTERFACE
*/
#define FIO_PEF24628E_READ_AUX         _IO(PEF24628E_IOC_MAGIC, 121)

/** SCI monitor control

   \param BOOL    The following values are valid:
                  - 0   SCI Monitor off
                  - 1   SCI Monitor on

   \retval 0      if success
   \retval -1     in case of an error.

   \remarks
   \code
   ioctl( fd, FIO_PEF24628E_SCI_MONITOR, 1 );
   \endcode
   \ingroup DRV_PEF24628E_INTERFACE
*/
#define FIO_PEF24628E_SCI_MONITOR       _IO(PEF24628E_IOC_MAGIC, 122)

/** Getting the driver's internal statistics

   \param PEF24628E_STAISTICS_t* The parameter points to a PEF24628E_STAISTICS_t structure.

   \retval 0      if success
   \retval -1     in case of an error.

   \remarks
   \code
   PEF24628E_STAISTICS_t drv_stat;

   // Read the driver statistics
   memset(&drv_stat, 0, sizeof(PEF24628E_STAISTICS_t));
   if ( ioctl(fd, FIO_PEF24628E_STATISTICS, (int)&drv_stat) < 0 ) {
      printf("Read statistics failed" CRLF);
   } else {
      printf("Read statistics successful (0x%08X)" CRLF, drv_stat.uRxFifoFull);
   }
   \endcode
   \ingroup DRV_PEF24628E_INTERFACE
*/
#define FIO_PEF24628E_GET_STATISTICS       _IO(PEF24628E_IOC_MAGIC, 123)
/** Clearing the driver's internal statistics

   \param none.

   \retval 0      if success
   \retval -1     in case of an error.

   \remarks
   \code

   // Clear the driver statistics
   if ( ioctl(fd, FIO_PEF24628E_STATISTICS, 0) < 0 ) {
      printf("The statistics was not cleared" CRLF);
   } else {
      printf("The statistics was cleared successful" CRLF);
   }
   \endcode
   \ingroup DRV_PEF24628E_INTERFACE
*/
#define FIO_PEF24628E_CLR_STATISTICS       _IO(PEF24628E_IOC_MAGIC, 124)

/** Shutdown device: delete all internal buffers and stops device.

   \retval 0      if success
   \retval -1     in case of an error.

   \remarks Should be called before closing the device.
   \code

   if ( ioctl(fd, FIO_PEF24628E_DEV_SHUTDOWN, 0) < 0 ) {
      printf("Device shutdown failed" CRLF);
   } else {
      printf("Device shutdown successful" CRLF);
   }
   \endcode
   \ingroup DRV_PEF24628E_INTERFACE
*/
#define FIO_PEF24628E_DEV_SHUTDOWN      _IO(PEF24628E_IOC_MAGIC, 125)
#define FIO_PEF24628E_READ_IDC_RS       _IO(PEF24628E_IOC_MAGIC, 126)
#define FIO_PEF24628E_WRITE_IDC_RS      _IO(PEF24628E_IOC_MAGIC, 127)

#define FIO_PEF24628E_READ_SDFE_RS      _IO(PEF24628E_IOC_MAGIC, 129)
#define FIO_PEF24628E_WRITE_SDFE_RS     _IO(PEF24628E_IOC_MAGIC, 130)

/* Silent startup config ioctl */
#define FIO_PEF24628E_SILENT_STARTUP_CONFIG       _IO(PEF24628E_IOC_MAGIC, 131)

#define FIO_PEF24628E_DBG_TRC_INIT      _IO(PEF24628E_IOC_MAGIC, 140)
#define FIO_PEF24628E_DBG_TRC_START     _IO(PEF24628E_IOC_MAGIC, 141)
#define FIO_PEF24628E_DBG_TRC_STOP      _IO(PEF24628E_IOC_MAGIC, 142)
#define FIO_PEF24628E_DBG_TRC_DUMP      _IO(PEF24628E_IOC_MAGIC, 144)
#define FIO_PEF24628E_DBG_TRC_DUMP_FW   _IO(PEF24628E_IOC_MAGIC, 145)
#define FIO_PEF24628E_DBG_TRC_FILTER_SET _IO(PEF24628E_IOC_MAGIC, 146)

#ifndef PEF24628E_USE_SCI_MONITOR
   /** Enable the SCI monitor */
#define PEF24628E_USE_SCI_MONITOR   1
/** The last byte of the message read by read() indicates monitor data */
#define PEF24628E_SCI_MONITOR_DATA  0xA1
/** The last byte of the message read by read() indicates normal data */
#define PEF24628E_ORDINAL_DATA      0xA0
#endif
/** Available Interfaces to use */
typedef enum
{
   /** SCI interface */
   PEF24628E_IF_SCI,
   /** MPI interface */
   PEF24628E_IF_MPI,
   PEF24628E_IF_LAST
} PEF24628E_IF_t;

/** Available cores */
typedef enum
{
   /** IDC core */
   PEF24628E_IDC,
   /** DSL core */
   PEF24628E_DSL
} PEF24628E_Core_t;

/** Structure for configuration data of the device,
   used for ioctl \ref FIO_PEF24628E_SET_CONFIG and \ref FIO_PEF24628E_GET_CONFIG
   \ingroup DRV_PEF24628E_INTERFACE */
typedef struct
{
   /* TODO: add configurable parameters */
   int dummy;
} PEF24628E_CONFIG_t;

/** Structure for device initialization, used for ioctl \ref FIO_PEF24628E_DEV_INIT
   \ingroup DRV_PEF24628E_INTERFACE */
typedef struct
{
   /** physical base address of the device */
   unsigned int reg_offset;
   /**
      interrupt number
      The IRQ number should be set to zero for polling mode
   */
   int irq_vector;
   /**
      Clock of the SCI interface. In case that the MPI interface is used
      this clock will be generated in the device (default value 0 is equal
      to 1/64 system clock.
      In case of the SCI interface mode the clock should be generated by
      external host. The value should be the clock in Hz. This value will
      be used for the HDLC driver configuration.
   */
   int sci_clock;
   /**
      The device address which should be used in case of SCI mode.
   */
   unsigned int sci_device_address;
} PEF24628E_DEV_INIT_t;

/** Structure for firmware download, used for ioctl \ref FIO_PEF24628E_DOWNLOAD_FW
   \ingroup DRV_PEF24628E_INTERFACE */
typedef struct
{
   /** core selector */
   PEF24628E_Core_t core;
   /** pointer to the firmware image */
   unsigned char *pFwImage;
   /** size of the firmware image */
   unsigned long size_byte;
} PEF24628E_DOWNLOAD_FW_t;

/** Structure for register access, used for ioctl \ref FIO_PEF24628E_SET_REG and
   \ref FIO_PEF24628E_GET_REG \ingroup DRV_PEF24628E_INTERFACE */
typedef struct
{
   /** register address */
   unsigned int addr;
   /** register value */
   unsigned int value;
} PEF24628E_REG_IO_t;

/** Structure for register access, used for ioctl \ref FIO_PEF24628E_GET_STATISTICS
    \ingroup DRV_PEF24628E_INTERFACE */
typedef struct
{
   /** cdm's/ack's matching counters, failures rx fifo and overflow flag */
   unsigned int uStatistics;

   /** last sent cmd message id */
   unsigned int uLastCmdId;

   /** last received ack message id */
   unsigned int uLastAckId;

} PEF24628E_STAISTICS_t;

int Pef24628e_DevCreate ( void );
int Pef24628e_DevDelete ( void );

#ifdef LINUX
int init_module ( void );
#endif 
/* */

/*@}*/
#endif /* _DRV_PEF24628E_INTERFACE_H */
