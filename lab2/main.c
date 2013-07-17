#include "stm32l1xx.h"
#include "stm32l_discovery_lcd.h"

static uint32_t TimingDelay;
//Прототип функции инициализации портов микроконтроллера
void gpio(void);
//Прототип функции инициализации контроллера ЖКИ
void controller(void);
//Прототип функции временной задержки в миллисекундах
void delay(uint32_t);

void main(void)
{
  gpio();
  controller();
  //Вызов функции конфигурации системного таймера SysTick, 
  //находящегося в core_cm3.h
  SysTick_Config(2097);
  
  while(1)
  {
    while(LCD->SR & LCD_SR_UDR);
    //Вывод информации на ЖКИ через регистры
    LCD->RAM[0] = 0x2E369985;
    LCD->RAM[2] = 0x2F2EF880;
    LCD->RAM[4] = 0x10000000;
    LCD->RAM[6] = 0x1;
    //Конец вывода
    LCD->SR |= LCD_SR_UDR;
    delay(3000);//Задержка 3 секунды
    LCD_GLASS_Clear();//Очистили ЖКИ
    
    //Бегущая строка, 1-количество повторений, 200 - задержка в 
    //миллисекундах перед обновлением информации на ЖКИ
    LCD_GLASS_ScrollSentence("      MAI",1,200);
    
    //Вывод на ЖКИ слова FRELA
    LCD_GLASS_DisplayString("FRELA");
    delay(2000);
  }
  
}

// Функция инициализации портов микроконтроллера
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

//Функция инициализации контроллера ЖКИ
void controller(void)
{
  RCC->APB1ENR |= 0x10000200;
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
  LCD->FCR |= 0x1040000;
  LCD->FCR &= ~0x1C00;
  LCD->FCR |= 0x800;
  while(!(LCD->SR&LCD_SR_FCRSR));
  LCD->CR &= ~0x2;
  LCD->CR |= 0x1;
  while(!(LCD->SR&LCD_SR_RDY));
  while(!(LCD->SR&LCD_SR_ENS));
}

//Функция временной задержки в миллисекундах
void delay(uint32_t nTime)
{
  TimingDelay = nTime;
  while(TimingDelay != 0);
}
//Обработчик прерывания системного таймера
void SysTick_Handler(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}