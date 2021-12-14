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

/** @addtogroup AT32F407_periph_examples
  * @{
  */
  
/** @addtogroup 407_TMR_6_steps TMR_6_steps
  * @{
  */

gpio_init_type  gpio_init_struct = {0};
tmr_output_config_type tmr_output_struct;
tmr_brkdt_config_type tmr_brkdt_config_struct = {0};
crm_clocks_freq_type crm_clocks_freq_struct = {0};

__IO uint32_t step = 1;

/* systick configuration */
void systick_configuration(void);

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  system_clock_config();

  at32_board_init();

  /* get system clock */
  crm_clocks_freq_get(&crm_clocks_freq_struct);

  systick_configuration();

  /* turn led2/led3/led4 on */
  at32_led_on(LED2);
  at32_led_on(LED3);
  at32_led_on(LED4);

  /* enable tmr1/gpioa/gpiob clock */
  crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  /* timer2 output pin Configuration */
  gpio_init_struct.gpio_pins = GPIO_PINS_8 | GPIO_PINS_9 | GPIO_PINS_10;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOA, &gpio_init_struct);

  gpio_init_struct.gpio_pins = GPIO_PINS_13 | GPIO_PINS_14 | GPIO_PINS_15;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOB, &gpio_init_struct);

  gpio_init_struct.gpio_pins = GPIO_PINS_12;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOB, &gpio_init_struct);

  /*
  the tmr1 peripheral offers the possibility to program in advance the
  configuration for the next tmr1 outputs behaviour (step) and change the configuration
  of all the channels at the same time. this operation is possible when the hall
  event is used.
  the hall event can be generated by software by setting the hall bit in the tmr1_swevt
  register or by hardware (on trc rising edge).
  in this example, a software hall event is generated each 100 ms: using the systick
  interrupt.
  the tmr1 is configured in timing mode, each time a hall event occurs,
  a new tmr1 configuration will be set in advance.
  the following table  describes the tmr1 channels states:
              -----------------------------------------------
             | step1 | step2 | step3 | step4 | step5 | step6 |
   ----------------------------------------------------------
  |channel1  |   1   |   0   |   0   |   0   |   0   |   1   |
   ----------------------------------------------------------
  |channel1n |   0   |   0   |   1   |   1   |   0   |   0   |
   ----------------------------------------------------------
  |channel2  |   0   |   0   |   0   |   1   |   1   |   0   |
   ----------------------------------------------------------
  |channel2n |   1   |   1   |   0   |   0   |   0   |   0   |
   ----------------------------------------------------------
  |channel3  |   0   |   1   |   1   |   0   |   0   |   0   |
   ----------------------------------------------------------
  |channel3n |   0   |   0   |   0   |   0   |   1   |   1   |
   ----------------------------------------------------------
  */
  /* tmr1 configuration */
  /* time base configuration */
  tmr_base_init(TMR1, 4095, 0);
  tmr_cnt_dir_set(TMR1, TMR_COUNT_UP);

  /* channel 1, 2 and 3 configuration in output mode */
  tmr_output_default_para_init(&tmr_output_struct);
  tmr_output_struct.oc_mode = TMR_OUTPUT_CONTROL_OFF;
  tmr_output_struct.oc_output_state = TRUE;
  tmr_output_struct.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.oc_idle_state = TRUE;
  tmr_output_struct.occ_output_state = TRUE;
  tmr_output_struct.occ_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.occ_idle_state = TRUE;
  
  /* channel 1 */
  tmr_output_channel_config(TMR1, TMR_SELECT_CHANNEL_1, &tmr_output_struct);
  tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_1, 2047);
  
  /* channel 2 */
  tmr_output_channel_config(TMR1, TMR_SELECT_CHANNEL_2, &tmr_output_struct);
  tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_2, 1023);
  
  /* channel 3 */
  tmr_output_channel_config(TMR1, TMR_SELECT_CHANNEL_3, &tmr_output_struct);
  tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_3, 511);

  /* automatic output enable, break, dead time and lock configuration */
  tmr_brkdt_default_para_init(&tmr_brkdt_config_struct);
  tmr_brkdt_config_struct.brk_enable = TRUE;
  tmr_brkdt_config_struct.auto_output_enable = TRUE;
  tmr_brkdt_config_struct.deadtime = 0;
  tmr_brkdt_config_struct.fcsodis_state = TRUE;
  tmr_brkdt_config_struct.fcsoen_state = TRUE;
  tmr_brkdt_config_struct.brk_polarity = TMR_BRK_INPUT_ACTIVE_HIGH;
  tmr_brkdt_config_struct.wp_level = TMR_WP_OFF;
  tmr_brkdt_config(TMR1, &tmr_brkdt_config_struct);
  tmr_channel_buffer_enable(TMR1, TRUE);

  /* hall interrupt enable */
  tmr_interrupt_enable(TMR1, TMR_HALL_INT, TRUE);

  /* tmr1 hall interrupt nvic init */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(TMR1_TRG_HALL_TMR11_IRQn, 0, 0);

  /* tmr1 output enable */
  tmr_output_enable(TMR1, TRUE);
  
  /* enable tmr1 */
  tmr_counter_enable(TMR1, TRUE);

  while(1)
  {
  }
}
/**
  * @brief  configures the systick.
  * @param  none
  * @retval none
  */
void systick_configuration(void)
{
  /* setup systick timer for 20 msec interrupts  */
  if(SysTick_Config((crm_clocks_freq_struct.sclk_freq) / 50))
  {
    /* capture error */
    while(1);
  }
  NVIC_SetPriority(SysTick_IRQn, 0x0);
}

/**
  * @brief  this function handles tmr1 trigger and hall interrupts
  *   requests.
  * @param  none
  * @retval none
  */
void TMR1_TRG_HALL_TMR11_IRQHandler(void)
{
  /* clear tmr1 com pending bit */
  tmr_flag_clear(TMR1, TMR_HALL_FLAG);
  if(step == 1)
  {
    /* next step: step 2 configuration ---------------------------- */
    /*  channel3 configuration */
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_3, FALSE);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_3C, FALSE);

    /*  channel1 configuration */
    tmr_output_channel_mode_select(TMR1, TMR_SELECT_CHANNEL_1, TMR_OUTPUT_CONTROL_PWM_MODE_A);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_1, TRUE);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_1C, FALSE);

    /*  channel2 configuration */
    tmr_output_channel_mode_select(TMR1, TMR_SELECT_CHANNEL_2, TMR_OUTPUT_CONTROL_PWM_MODE_A);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_2, FALSE);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_2C, TRUE);
    step++;
  }
  else if(step == 2)
  {
    /* next step: step 3 configuration ---------------------------- */
    /*  channel2 configuration */
    tmr_output_channel_mode_select(TMR1, TMR_SELECT_CHANNEL_2, TMR_OUTPUT_CONTROL_PWM_MODE_A);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_2, FALSE);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_2C, TRUE);

    /*  channel3 configuration */
    tmr_output_channel_mode_select(TMR1, TMR_SELECT_CHANNEL_3, TMR_OUTPUT_CONTROL_PWM_MODE_A);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_3, TRUE);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_3C, FALSE);

    /*  channel1 configuration */
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_1, FALSE);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_1C, FALSE);
    step++;
  }
  else if(step == 3)
  {
    /* next step: step 4 configuration ---------------------------- */
    /*  channel3 configuration */
    tmr_output_channel_mode_select(TMR1, TMR_SELECT_CHANNEL_3, TMR_OUTPUT_CONTROL_PWM_MODE_A);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_3, TRUE);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_3C, FALSE);

    /*  channel2 configuration */
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_2, FALSE);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_2C, FALSE);

    /*  channel1 configuration */
    tmr_output_channel_mode_select(TMR1, TMR_SELECT_CHANNEL_1, TMR_OUTPUT_CONTROL_PWM_MODE_A);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_1, FALSE);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_1C, TRUE);
    step++;
  }
  else if(step == 4)
  {
    /* next step: step 5 configuration ---------------------------- */
    /*  channel3 configuration */
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_3, FALSE);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_3C, FALSE);

    /*  channel1 configuration */
    tmr_output_channel_mode_select(TMR1, TMR_SELECT_CHANNEL_1, TMR_OUTPUT_CONTROL_PWM_MODE_A);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_1, FALSE);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_1C, TRUE);

    /*  channel2 configuration */
    tmr_output_channel_mode_select(TMR1, TMR_SELECT_CHANNEL_2, TMR_OUTPUT_CONTROL_PWM_MODE_A);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_2, TRUE);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_2C, FALSE);
    step++;
  }
  else if(step == 5)
  {
    /* next step: step 6 configuration ---------------------------- */
    /*  channel3 configuration */
    tmr_output_channel_mode_select(TMR1, TMR_SELECT_CHANNEL_3, TMR_OUTPUT_CONTROL_PWM_MODE_A);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_3, FALSE);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_3C, TRUE);

    /*  channel1 configuration */
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_1, FALSE);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_1C, FALSE);

    /*  channel2 configuration */
    tmr_output_channel_mode_select(TMR1, TMR_SELECT_CHANNEL_2, TMR_OUTPUT_CONTROL_PWM_MODE_A);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_2, TRUE);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_2C, FALSE);
    step++;
  }
  else
  {
    /* next step: step 1 configuration ---------------------------- */
    /*  channel1 configuration */
    tmr_output_channel_mode_select(TMR1, TMR_SELECT_CHANNEL_1, TMR_OUTPUT_CONTROL_PWM_MODE_A);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_1, TRUE);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_1C, FALSE);

    /*  channel3 configuration */
    tmr_output_channel_mode_select(TMR1, TMR_SELECT_CHANNEL_3, TMR_OUTPUT_CONTROL_PWM_MODE_A);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_3, FALSE);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_3C, TRUE);

    /*  channel2 configuration */
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_2, FALSE);
    tmr_channel_enable(TMR1, TMR_SELECT_CHANNEL_2C, FALSE);
    step = 1;
  }
}

/**
  * @}
  */ 

/**
  * @}
  */ 
