#include "stm32l1xx_rcc.h"
#include "stm32l1xx_gpio.h"
//API ��� ������ � TSL
#include "stm32_tsl_api.h"
//������� ��� ������ � ��������� ������� 
#include "discover_functions.h"
//������� ��� ������ � ���
#include "stm32l_discovery_lcd.h"

/*################ ��������� ������� #############*/
void gpio(void);//������������� ������
void controller(void);//������������� ����������� ���
void delay(uint32_t); //��������� �������� � �������������
void led3(void);
void led4(void);
void handler(void);//���������� ����������

//�������� ���������� flag �������� � ����������� 
//���������� EXTI0_IRQHandler() ��� ������� ������
int flag=0;
static uint32_t TimingDelay;


void main(void)
{
    /*############ ��������� ����������� #############*/ 
     GPIO_InitTypeDef GPIO_InitStructure;
    //��������� ������������
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_400KHz;//GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    //������������� ���������
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /*################ ��������� ���������� #############*/ 
    EXTI->IMR|=EXTI_IMR_MR0;                          
    EXTI->RTSR|=EXTI_RTSR_TR0;                          
    NVIC_EnableIRQ (EXTI0_IRQn); 
    //NVIC_SetPriority(EXTI0_IRQn, 0);
    
    /*###### ��������� GPIO � ����������� ��� ######*/   
    gpio();
    controller();
    
    /*################ ��������� ������ USER #############*/ 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//�����!
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /*############# ��������� ��������� ������ #############*/ 
    TSL_Init();
    sMCKeyInfo[0].Setting.b.IMPLEMENTED = 1;
    sMCKeyInfo[0].Setting.b.ENABLED = 1;
    sMCKeyInfo[0].DxSGroup = 0x00; 
    
    /*######### ������� ��������� � ����������� ����� ######*/  
      while(1)
      {
          if (flag==0)
          {
                TSL_Action();
                Button_value();//��������� ������ ��� 4 ���������� ������
          } 
          else if (flag==1)
          {
                TSL_Action();
                Slider_value();//��������� ������ ��� �������
          } 
          else if (flag==2)
          {
            
               LCD_GLASS_Clear();//�������� ���
               LCD_GLASS_DisplayString("GREEN");//������ �� ��� GREEN
               led3();//������ ������� �����������
          }
          else if (flag==3)
          {
                LCD_GLASS_Clear();//�������� ���
                LCD_GLASS_DisplayString("BLUE");//������ �� ��� BLUE
                led4();//������ ����� �����������
          } 
      }
}
/*##### ������������ ����������� ��� � ��������� ������ #####*/ 
void controller(void)
{
/*################ ������������ HSI #############*/ 
    RCC->CR |=0x1;
    while(!(RCC->CR&RCC_CR_HSIRDY));
    RCC->CFGR |=0x1; 

/*################ ������������ ����������� ��� #############*/   
    RCC->APB1ENR |= 0x90000200; //���� ������������ ����������� � 31 �������
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
/*########## ��������� ������ GPIO ��� ������ � ��� ##########*/ 
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
/*############# ������� �������� � ������������� ##########*/
void delay(uint32_t nTime)//����������� ������� delay()
{
  TimingDelay = nTime;
  while(TimingDelay != 0);
}

/*################ ������� ������� ����������� #############*/
void led3(void) //����������� ������� led3()
{
    GPIO_SetBits(GPIOB, GPIO_Pin_7);
    delay(250);//250 ��
    GPIO_ResetBits(GPIOB, GPIO_Pin_7);
    delay(250);
}
/*################ ������� ����� ����������� #############*/
void led4(void) //����������� ������� led4()
{
    GPIO_SetBits(GPIOB, GPIO_Pin_6);
    delay(250);
    GPIO_ResetBits(GPIOB, GPIO_Pin_6);
    delay(250);
}
/*################ ���������� ���������� #############*/ 
//���������� ���������� ������� �� ������
void EXTI0_IRQHandler(void)
{
      EXTI->PR|=0x01; //����� ���� �������, ���������� ����������
      flag++;
      if (flag>3)
        flag=0;
}

//���������� ���������� ���������� �������
//����� ������� SysTick_Config() ������������ � core_cm3.h 
//���������� � ����� stm32_tsl_timebase.c
void SysTick_Handler(void)
{    
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}