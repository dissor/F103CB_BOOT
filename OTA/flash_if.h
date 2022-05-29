#ifndef __FLASH_IF_H
#define __FLASH_IF_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Error code */
enum
{
  FLASHIF_OK = 0,
  FLASHIF_ERASEKO,
  FLASHIF_WRITINGCTRL_ERROR,
  FLASHIF_WRITING_ERROR,
  FLASHIF_PROTECTION_ERRROR
};

/* protection type */
enum{
  FLASHIF_PROTECTION_NONE         = 0,
  FLASHIF_PROTECTION_PCROPENABLED = 0x1,
  FLASHIF_PROTECTION_WRPENABLED   = 0x2,
  FLASHIF_PROTECTION_RDPENABLED   = 0x4,
};

/* protection update */
enum {
	FLASHIF_WRP_ENABLE,
	FLASHIF_WRP_DISABLE
};


#define FLASH_PAGE_STEP         FLASH_PAGE_SIZE

#define USER_FLASH_ADDRESS      (uint32_t)(FLASH_BASE + FLASH_PAGE_STEP*7)        // 0x08001C00
#define APPLICATION_ADDRESS     (uint32_t)(USER_FLASH_ADDRESS + FLASH_PAGE_STEP)  // 0x08002000


/* Exported functions ------------------------------------------------------- */
void FLASH_If_Init(void);
uint32_t FLASH_If_Erase(uint32_t StartSector, uint32_t Size);
uint32_t FLASH_If_Write(uint32_t destination, uint32_t *p_source, uint32_t length);

#endif  /* __FLASH_IF_H */
