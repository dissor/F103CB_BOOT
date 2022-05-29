#include "iap.h"

UserData_t UserData;
qlz_t qlz;

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  while (1)
  {
    memcpy(&UserData, (void *)USER_FLASH_ADDRESS, sizeof(UserData));

    if (UserData.OtaSize != 0xFFFF)
    {
      printf("\nhello QuickLz IAP!\n");

      QlzIapLoop();

      FLASH_If_Erase(USER_FLASH_ADDRESS, FLASH_PAGE_SIZE);
      UserData.OtaSize = 0xFFFF;
      if (FLASHIF_OK != FLASH_If_Write(USER_FLASH_ADDRESS, (uint32_t *)&UserData, sizeof(UserData)))
      {
        HAL_NVIC_SystemReset();
      }

      JumpToApp();
    }
    else
    {
      JumpToApp();
    }
  }
}

void JumpToApp(void)
{
  typedef void (*pFunction)(void);
  uint32_t JumpAddress;
  pFunction JumpToApplication;

  if (((*(__IO uint32_t *)APPLICATION_ADDRESS) & 0x2FFE0000) == 0x20000000)
  {
    __disable_irq();
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;

    for (uint8_t i = 0; i < 8; i++)
    {
      NVIC->ICER[i] = 0xFFFFFFFF;
      NVIC->ICPR[i] = 0xFFFFFFFF;
    }

    __enable_irq();

    /* Jump to user application */
    JumpAddress = *(__IO uint32_t *)(APPLICATION_ADDRESS + 4);
    JumpToApplication = (pFunction)JumpAddress;
    /* Initialize user application's Stack Pointer */
    __set_MSP(*(__IO uint32_t *)APPLICATION_ADDRESS);
    JumpToApplication();
  }
}

void QlzIapLoop(void)
{
  uint16_t PageNum = (UserData.OtaSize % FLASH_PAGE_SIZE == 0) ? UserData.OtaSize / FLASH_PAGE_SIZE : UserData.OtaSize / FLASH_PAGE_SIZE + 1;
  uint32_t QlzAddr = FLASH_BASE + (128 - PageNum) * 1024;
  uint8_t *ptr = (uint8_t *)QlzAddr;
  uint16_t cnt = 0;

  UserData.CodeSize = 0;

#ifdef DEF_QLZ_MALLOC
  //      qlz.p_state_decompress = (qlz_state_decompress*) malloc(sizeof(qlz_state_decompress));

  //      if(qlz.p_state_decompress == NULL)
  //      {
  //        printf("ERROR: qlz.p_state_decompress malloc\n");
  //      }

  //      memset(qlz.p_state_decompress, 0, sizeof(qlz_state_decompress));

  qlz.p_compressed = (uint8_t *)malloc(1024 + 400);

  if (qlz.p_compressed == NULL)
  {
    printf("ERROR: qlz.p_compressed malloc\n");
  }

  memset(qlz.p_compressed, 0, 1024 + 400);

  qlz.p_decompressed = (uint8_t *)malloc(1024);

  if (qlz.p_decompressed == NULL)
  {
    printf("ERROR: qlz.p_decompressed malloc\n");
  }

  memset(qlz.p_decompressed, 0, 1024);
#endif

  while (1)
  {
    memcpy(qlz.p_compressed, ptr, 9);
    ptr += 9;

    qlz.c_size = qlz_size_compressed((const char *)qlz.p_compressed);

    if ((qlz.c_size == 0) || (qlz.c_size == 0xFFFFFFFF))
    {
      printf("ERROR: qlz.c_size:%d\n", qlz.c_size);
      break;
    }

    memcpy(qlz.p_compressed + 9, ptr, qlz.c_size - 9);
    ptr += qlz.c_size - 9;
    qlz.d_size = qlz_decompress((const char *)qlz.p_compressed, qlz.p_decompressed, &qlz.p_state_decompress);
    printf("%u bytes decompressed into %u.\n", (unsigned int)qlz.c_size, (unsigned int)qlz.d_size);
    UserData.CodeSize += qlz.d_size;

    FLASH_If_Erase(APPLICATION_ADDRESS + cnt * FLASH_PAGE_SIZE, FLASH_PAGE_SIZE);

    if (FLASHIF_OK != FLASH_If_Write(APPLICATION_ADDRESS + cnt * FLASH_PAGE_SIZE, (uint32_t *)qlz.p_decompressed, (qlz.d_size % sizeof(uint32_t)) ? qlz.d_size / sizeof(uint32_t) + 1 : qlz.d_size / sizeof(uint32_t)))
    {
      HAL_NVIC_SystemReset();
    }

    cnt++;

    if (ptr >= (uint8_t *)QlzAddr + UserData.OtaSize)
    {
      printf("FINISH: qlz\n");
      break;
    }
  }
}

int fputc(int ch, FILE *f)
{
  SEGGER_RTT_PutChar(0, ch);
  return ch;
}
