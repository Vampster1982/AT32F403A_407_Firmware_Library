/**
  **************************************************************************
  * @file     main.c
  * @version  v2.0.4
  * @date     2021-11-26
  * @brief    main program
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to 
  * download from Artery official website is the copyrighted work of Artery. 
  * Artery authorizes customers to use, copy, and distribute the BSP 
  * software and its related documentation for the purpose of design and 
  * development in conjunction with Artery microcontrollers. Use of the 
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
#include "at32_emac.h"
#include "stdio.h"
#include "netconf.h"
#include "iperf.h"

/** @addtogroup AT32F407_periph_examples
  * @{
  */
  
/** @addtogroup 407_EMAC_iperf EMAC_iperf
  * @{
  */

#define DELAY                            100
#define FAST                             1
#define SLOW                             4

uint8_t g_speed = FAST;

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  error_status status;
  
  system_clock_config();

  at32_board_init();
  
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

  delay_init();

  status = emac_system_init();

  while(status == ERROR);
  
  tcpip_stack_init();
  
  iperf_init();
  
  for(;;)
  {
    at32_led_toggle(LED2);
    delay_ms(g_speed * DELAY);
    at32_led_toggle(LED3);
    delay_ms(g_speed * DELAY);
    at32_led_toggle(LED4);
    delay_ms(g_speed * DELAY);
  }
}

/**
  * @}
  */ 

/**
  * @}
  */ 
