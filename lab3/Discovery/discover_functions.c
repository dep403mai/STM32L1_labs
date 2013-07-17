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


#include "stm32_tsl_api.h"
#include "stm32l15x_tsl_ct_acquisition.h"

#include "discover_functions.h"
#include "stm32l_discovery_lcd.h"
 
/*########## Вывод точки касания в процентах #############*/  
void Slider_value(void)
{
  uint16_t Message[6];  
  uint32_t percent_value;
/*################ Отключили светодиоды #############*/  
  GPIO_ResetBits(GPIOB, GPIO_Pin_6);
  GPIO_ResetBits(GPIOB, GPIO_Pin_7);
 
/*В массив Massage[] заносятся символы для отображения на ЖКИ */ 
  Message[0] = ' ';
  Message[1] = ' ';
  Message[2] = ' ';

  if ((TSL_GlobalSetting.b.CHANGED) && (TSLState == TSL_IDLE_STATE)) 
  {
    TSL_GlobalSetting.b.CHANGED = 0;
  }//Если касание обнаружено
  if ((SLIDER_DETECTED)&&(TSLState == TSL_IDLE_STATE))  {
/*###### Вычисление положения касания сенсорной панели #####*/ 
    percent_value = SLIDER_POSITION ;
    percent_value *= 10000;
    percent_value /= 255 ;
/*##### Функция перевода значения в char #####*/      
    convert_into_char(percent_value,Message);
    Message[3] = '°' ;
    Message[4] = '/' ;
    Message[5] = '%' ;   
/* Вывод на ЖКИ в виде "X %", где Х - положение точки касания */
    LCD_GLASS_DisplayStrDeci(Message);
  } 
  else if((!TSL_GlobalSetting.b.CHANGED) && (TSLState == TSL_IDLE_STATE))
  {     //Если касание не обнаружено
        Message[3] = '°' ;
        Message[4] = '/' ;
        Message[5] = '%' ;   
        LCD_GLASS_DisplayStrDeci(Message);
  } 
}/*###########################################################*/  

/*####### Сенсорная панель как 4 раздельные кнопки ########*/  
void Button_value(void)
{
  uint8_t Message[6];  
/*В массив Massage[] заносятся символы для отображения на ЖКИ */   
    Message[0] = ' ';
    Message[1] = '0';
    Message[2] = '0';
    Message[3] = '0';
    Message[4] = '0';
    Message[5] = ' ';
    
  if ((TSL_GlobalSetting.b.CHANGED) && (TSLState == TSL_IDLE_STATE)) 
  {
    TSL_GlobalSetting.b.CHANGED = 0;

  }//Если касание обнаружено
  if ((SLIDER_DETECTED)&&(TSLState == TSL_IDLE_STATE))  {
/*Вся область сенсорной панели делится на 4 части определением точки касания*/  
    if( SLIDER_POSITION <= 25 ){
      Message[1] = 255;       
      GPIO_SetBits(GPIOB, GPIO_Pin_7); //Включили зеленый светодиод
      GPIO_ResetBits(GPIOB, GPIO_Pin_6);} //Отключили синий светодиод

    if( (SLIDER_POSITION > 25 ) && (SLIDER_POSITION <= 110 )) {       
      Message[2] = 255; 
      GPIO_ResetBits(GPIOB, GPIO_Pin_7); //Отключили зеленый светодиод
      GPIO_SetBits(GPIOB, GPIO_Pin_6);  } //Включили синий светодиод
    if( (SLIDER_POSITION > 110 ) && (SLIDER_POSITION <= 200 ))  {      
      Message[3] = 255;  
      GPIO_SetBits(GPIOB, GPIO_Pin_6);  //Включили синий светодиод
      GPIO_SetBits(GPIOB, GPIO_Pin_7); }//Включили зеленый светодиод
      
    if( SLIDER_POSITION > 200 )    {    
      Message[4] = 255;
      GPIO_ResetBits(GPIOB, GPIO_Pin_6); //Отключили синий светодиод
      GPIO_ResetBits(GPIOB, GPIO_Pin_7);}//Отключ. зеленый светодиод
      
/*Вывод на ЖКИ в виде "0Х00", где Х - положение точки касания */   
   LCD_GLASS_DisplayString(Message);
  } 
  else if((!TSL_GlobalSetting.b.CHANGED) && (TSLState == TSL_IDLE_STATE))
  {     //Если касание не обнаружено
        LCD_GLASS_DisplayString(Message);
  }
   
}


void convert_into_char(uint32_t number, uint16_t *p_tab)
{
  uint16_t units=0, tens=0, hundreds=0, thousands=0, misc=0;
  
  units = (((number%10000)%1000)%100)%10;
  tens = ((((number-units)/10)%1000)%100)%10;
  hundreds = (((number-tens-units)/100))%100%10;
  thousands = ((number-hundreds-tens-units)/1000)%10;
  misc = ((number-thousands-hundreds-tens-units)/10000);
  
  *(p_tab+4) = units + 0x30;
  *(p_tab+3) = tens + 0x30;
  *(p_tab+2) = hundreds + 0x30;
  *(p_tab+1) = thousands + 0x30;
  *(p_tab) = misc + 0x30;

}
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
