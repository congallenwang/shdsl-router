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
   Main routine for U-Boot / Linux based environment.
*/

#include "lib_sysdep.h"
//#include "prj_version.h"
#include "board.h"
//#include "lib_soc4e_map.h"
//#include "lib_menu.h"
#include "drv_pef24628e_interface.h"

#include "stdio.h"


static BOARD_Configuration_t DeviceConfiguration =
{
   PEF24628E_IF_MPI,
   TRUE,
   4,
   PEF24628E_MAX_DEV_NUMBER,
   PEF24628E_MAX_LINE_NUMBER,   
};

LOCAL int InitBoard(void);
LOCAL int ShutdownBoard(void);

int usrApp(void);

//#ifdef UBOOT
/* The No_OS_Init() will be called by the main() function.
   Afterwards the No_OS_Init() will execute
   - No_OS_InitBoard()
   - usrApp()
   - No_OS_ShutdownBoard()
*/
extern void No_OS_Init(void);

int No_OS_InitBoard(void)
{
   //return InitBoard();
   return 0;
}

int No_OS_ShutdownBoard(void)
{
   //return ShutdownBoard();
   return 0;
}

//#endif

#if 0

   
#endif

/** what string support, version string */
//LOCAL const char soc4e_compact_whatversion[] = PRJ_SOC4E_COMPACT_WHAT_STR;

/**
   entry point of the boot loader
*/
#if 0
void main(MAINARGS)
{
   app_startup(argv);

   if (get_version () != XF_VERSION) {
      PRINTF("Wrong XF_VERSION. Please re-compile with actual u-boot sources\n\r");
      PRINTF("Example expects ABI version %d\n\r", XF_VERSION);
      PRINTF("Actual U-Boot ABI version %d\n\r", (int)get_version());
      return;
   }

   No_OS_Init();
}
#endif /* UBOOT */

#ifdef LINUX
int main(int argc, char *argv[])
{

   //prepare app driver wrapper
   No_OS_Init(); 

   //shdsl init
   usrApp();  
   
   return 0;   
} 
#endif


/**
   the user application
   
   \return
   - 0 on success
   - -1 on failure
*/
int usrApp(void)
{
   int key;
   BOOL bRestart = FALSE;
   BOOL status;
   BOOL loop_active;
   UINT8 device;

RESTART:
   bRestart = FALSE;
   
   printf("Interface mode: %d\n\r", DeviceConfiguration.nInterfaceMode);
   printf("Polling mode:   %d\n\r", DeviceConfiguration.bPollingMode);
   printf("SCI clock:      %d\n\r", DeviceConfiguration.nSciClock);


   status = soc4e_drv_init(&DeviceConfiguration);

   if (status != TRUE)
   {
      printf("soc4e_drv_init() failed ... \n\r");
      //goto ERROR_EXIT;
   }

#if 0
   if (soc4e_lib_init(&DeviceConfiguration) != TRUE)
   {
      printf("soc4e_lib_init() failed ... \n\r");
      goto ERROR_EXIT;
   }

   PRINTF("Press 'H' to get help ... \n\r");
   loop_active = TRUE;
   do
   {
      while (!tstc())
      {
         if (!board_hardware_poll())
         {
            /* hardware problem ? Try restart. */
            bRestart = TRUE;
            goto ERROR_EXIT;
         }
         for (device=0; device<DeviceConfiguration.nMaxDevNumber; device++)
         {
            if (!soc4e_drv_poll(device, 0))
            {
               /* hardware problem ? Try restart. */
               bRestart = TRUE;
               goto ERROR_EXIT;
            }
         }
         if (!soc4e_lib_poll(&DeviceConfiguration))
         {
            /* library problem ? Try restart. */
            bRestart = TRUE;
            goto ERROR_EXIT;
         }
         if (!lib_menu_poll())
         {
            /* menu problem ? Try restart. */
            bRestart = TRUE;
            goto ERROR_EXIT;
         }
      }
   }
   while (loop_active);

ERROR_EXIT:

   printf("\r\n");

   if (soc4e_drv_exit() != TRUE)
      return -1;

   if (bRestart== TRUE)
      goto RESTART;
#endif
   return 0;
}

