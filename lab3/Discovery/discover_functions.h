 /**
 ******************************************************************************
 * @file    discover_functions.c
 * @author  Microcontroller Division
 * @editor  Teleshov A.
 * @version V0.0.1
 * @date    MAY-2012
 * @brief   Discover demo functions
 ******************************************************************************
 * @copy
 *
 * This is copy of original file ST Microelectronics that was 
 * optimized for lab in department 403 MAI
 * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
 */

#include "stm32l1xx.h"  





#define SLIDER_DETECTED (sMCKeyInfo[0].Setting.b.DETECTED)
#define SLIDER_POSITION (sMCKeyInfo[0].UnScaledPosition)

void convert_into_char(uint32_t number, uint16_t *p_tab);
void SysTick_Handler(void);
void Button_value(void);
void Slider_value(void);




/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
