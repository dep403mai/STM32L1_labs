#include "stm32l1xx_rcc.h"
#include "stm32l1xx_gpio.h"
//API для работы с TSL
#include "stm32_tsl_api.h"
//Функции для работы с сенсорной панелью 
#include "discover_functions.h"
//Функции для работы с ЖКИ
#include "stm32l_discovery_lcd.h"

/*################ Прототипы функций #############*/
void gpio(void);//Инициализация портов
void controller(void);//Инициализация контроллера ЖКИ
void delay(uint32_t); //Временная задержка в миллисекундах
void led3(void);
void led4(void);
void handler(void);//Обработчик прерываний

//Значение переменной flag меняется в обработчике 
//прерываний EXTI0_IRQHandler() при нажатия кнопки
int flag=0;
static uint32_t TimingDelay;


void main(void)
{
    /*############ Настройка светодиодов #############*/ 
     GPIO_InitTypeDef GPIO_InitStructure;
    //Включение тактирования
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_400KHz;//GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    //Инициализация структуры
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /*################ Настройка прерываний #############*/ 
    EXTI->IMR|=EXTI_IMR_MR0;                          
    EXTI->RTSR|=EXTI_RTSR_TR0;                          
    NVIC_EnableIRQ (EXTI0_IRQn); 
    //NVIC_SetPriority(EXTI0_IRQn, 0);
    
    /*###### Настройка GPIO и контроллера ЖКИ ######*/   
    gpio();
    controller();
    
    /*################ Настройка кнопки USER #############*/ 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//ВАЖНО!
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /*############# Настройка сенсорной панели #############*/ 
    TSL_Init();
    sMCKeyInfo[0].Setting.b.IMPLEMENTED = 1;
    sMCKeyInfo[0].Setting.b.ENABLED = 1;
    sMCKeyInfo[0].DxSGroup = 0x00; 
    
    /*######### Главная программа в бесконечном цикле ######*/  
      while(1)
      {
          if (flag==0)
          {
                TSL_Action();
                Button_value();//Сенсорная панель как 4 раздельные кнопки
          } 
          else if (flag==1)
          {
                TSL_Action();
                Slider_value();//Сенсорная панель как слайдер
          } 
          else if (flag==2)
          {
            
               LCD_GLASS_Clear();//Очистили ЖКИ
               LCD_GLASS_DisplayString("GREEN");//Вывели на ЖКИ GREEN
               led3();//Мигаем зеленым светодиодом
          }
          else if (flag==3)
          {
                LCD_GLASS_Clear();//Очистили ЖКИ
                LCD_GLASS_DisplayString("BLUE");//Вывели на ЖКИ BLUE
                led4();//Мигаем синим светодиодом
          } 
      }
}
/*##### Тактирование контроллера ЖКИ и сенсорной панели #####*/ 
void controller(void)
{
/*################ Тактирование HSI #############*/ 
    RCC->CR |=0x1;
    while(!(RCC->CR&RCC_CR_HSIRDY));
    RCC->CFGR |=0x1; 

/*################ Тактирование контроллера ЖКИ #############*/   
    RCC->APB1ENR |= 0x90000200; //плюс тактирование компаратора в 31 разряде
    PWR->CR |= 0x100;
    RCC->CSR |= 0x800000;
    RCC->CSR &= ~0x800000;
    RCC->CSR |= 0x100;
    while(!(RCC->CSR&RCC_CSR_LSERDY));
    RCC->CSR |= 0x10000;
    LCD->CR &= ~0x60;
    LCD->CR |= 0x40;
    LCD->CR &= ~0x1C;
    LCD->CR |= 0xC;
    LCD->CR |= 0x80;
    LCD->FCR &= ~0x3C00000;
    LCD->FCR &= ~0x3C0000;
    LCD->FCR |= 0x40000; //ps=1 div=17
    LCD->FCR &= ~0x1C00;
    LCD->FCR |= 0x800;
    while(!(LCD->SR&LCD_SR_FCRSR));
    LCD->CR &= ~0x2;
    LCD->CR |= 0x1;
    while(!(LCD->SR&LCD_SR_RDY));
    while(!(LCD->SR&LCD_SR_ENS));
}
/*########## Настройка портов GPIO для работы с ЖКИ ##########*/ 
void gpio(void)
{
    RCC->AHBENR |= 0x7;
    GPIOA->MODER |= 0x802A00A8;
    GPIOB->MODER |= 0xAAAA0A80;
    GPIOC->MODER |= 0xAAA0AA;
    GPIOA->OTYPER &= ~0x870E;
    GPIOB->OTYPER &= ~0xFF38;
    GPIOC->OTYPER &= ~0xFCF;
    GPIOA->PUPDR &= ~0xC03F00FC;
    GPIOB->PUPDR &= ~0xFFFF0FC0;
    GPIOC->PUPDR &= ~0xFFF0FF;
    GPIOA->OSPEEDR &= ~0xC03F00FC;
    GPIOB->OSPEEDR &= ~0xFFFF0FC0;
    GPIOC->OSPEEDR &= ~0xFFFFF0FF;
    GPIOA->AFR[0] |= 0xBBB0;
    GPIOA->AFR[1] |= 0xB0000BBB;
    GPIOB->AFR[0] |= 0xBBB000;
    GPIOB->AFR[1] |= 0xBBBBBBBB;
    GPIOC->AFR[0] |= 0xBB00BBBB;
    GPIOC->AFR[1] |= 0xBBBB;
}
/*############# Функция задержки в миллисекундах ##########*/
void delay(uint32_t nTime)//Определение функции delay()
{
  TimingDelay = nTime;
  while(TimingDelay != 0);
}

/*################ Мигание зеленым светодиодом #############*/
void led3(void) //Определение функции led3()
{
    GPIO_SetBits(GPIOB, GPIO_Pin_7);
    delay(250);//250 мс
    GPIO_ResetBits(GPIOB, GPIO_Pin_7);
    delay(250);
}
/*################ Мигание синим светодиодом #############*/
void led4(void) //Определение функции led4()
{
    GPIO_SetBits(GPIOB, GPIO_Pin_6);
    delay(250);
    GPIO_ResetBits(GPIOB, GPIO_Pin_6);
    delay(250);
}
/*################ Обработчик прерываний #############*/ 
//Обработчик прерывания нажатия на кнопку
void EXTI0_IRQHandler(void)
{
      EXTI->PR|=0x01; //Сброс бита события, вызвавшего прерывание
      flag++;
      if (flag>3)
        flag=0;
}

//Обработчик прерывания системного таймера
//Вызов функции SysTick_Config() определенной в core_cm3.h 
//происходит в файле stm32_tsl_timebase.c
void SysTick_Handler(void)
{    
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}