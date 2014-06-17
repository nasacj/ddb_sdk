
#ifndef _BUS_DRV_IF_H
#define _BUS_DRV_IF_H

#include <windows.h>
#include <winioctl.h>

/* Bus driver type. used in ClientInfo key to detect which driver loaded the SmsGenDrv */
typedef enum
{
	BD_TYPE_SDIO,
	BD_TYPE_SPI,
	BD_TYPE_HIF

} BD_TYPE_E;

/* Bus driver prefix */
#define SIANO_SDIO_PREFIX	TEXT("SDI")
#define SIANO_SPI_PREFIX	TEXT("SPI")
#define SIANO_HIF_PREFIX	TEXT("SHI")

#define SMS_GEN_DRIVER_REGISTRY_PATH TEXT("Drivers\\Siano\\SMS1000\\SmsGenDrv")

/* callback to be registered in SIANO_BD_IOCTL_REGISTER_READ_CB */
typedef void (*BD_ReadCBFunc) (DWORD hContext, void* pBuffer, UINT32 BufSize);

typedef struct BD_IOCTL_REGISTER_READ_PARAMS_S
{
	BD_ReadCBFunc	pBD_ReadCBFunc;
	DWORD			hContext;

} BD_IOCTL_REGISTER_READ_PARAMS_ST, *PBD_IOCTL_REGISTER_READ_PARAMS_ST;

/* IOCTLS */

#define SIANO_MAKE_IOCTL(index) \
      CTL_CODE(40000, 2048 + (index), METHOD_BUFFERED, FILE_ANY_ACCESS)

/* Description : register callback to be called after driver read buffer from the device */
/* input  : pointer to PBD_IOCTL_REGISTER_READ_PARAMS_ST structure */
/* output : none */
#define SIANO_BD_IOCTL_REGISTER_READ_CB					SIANO_MAKE_IOCTL(0)

/* Description : write FW buffer to the device */
/* input  : buffer holds the FW to be written */
/* output : none */
#define SIANO_BD_IOCTL_WRITE_FW_BUFF_TO_DEVICE			SIANO_MAKE_IOCTL(1)

#define SIANO_BD_IOCTL_RESET_DEVICE							SIANO_MAKE_IOCTL(2)

#define SIANO_BD_IOCTL_POWERUP_DEVICE						SIANO_MAKE_IOCTL(3)

#define SIANO_BD_IOCTL_POWERDOWN_DEVICE					SIANO_MAKE_IOCTL(4)

#define SIANO_BD_IOCTL_RESET_LOW_DEVICE					SIANO_MAKE_IOCTL(5)

#define SIANO_BD_IOCTL_RESET_HIGH_DEVICE					SIANO_MAKE_IOCTL(6)
#endif
