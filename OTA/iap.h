#ifndef __IAP_H__
#define __IAP_H__

#include "main.h"
#include "gpio.h"

#include "flash_if.h"
#include "SEGGER_RTT.h"
#include "quicklz.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
  uint16_t OtaSize;
  uint32_t CodeSize;
} UserData_t;

//#define DEF_QLZ_MALLOC

typedef struct
{
  size_t c_size;
  size_t d_size;
#ifdef DEF_QLZ_MALLOC
  uint8_t *p_compressed;
  uint8_t *p_decompressed;
  //  qlz_state_decompress *p_state_decompress;
#else
  uint8_t p_compressed[1024 + 400];
  uint8_t p_decompressed[1024];
#endif
  qlz_state_decompress p_state_decompress;
} qlz_t;

extern void SystemClock_Config(void);

void JumpToApp(void);
void QlzIapLoop(void);

#endif /* __IAP_H__ */
