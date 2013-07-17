#include <stm32l1xx.h>
#include <stm32l1xx_gpio.h>
#include <stm32l1xx_rcc.h> 

void delay(int B); /*�������� ������� delay()*/
void led3(void);   /*�������� ������� led3()*/
void led4(void);   /*�������� ������� led4()*/

int main(void)
{
/*���������� ���������� ���� uint32_t*/   
uint32_t i,j; 

/*���������� ��������� GPIO_InitStructure*/ 
GPIO_InitTypeDef GPIO_InitStructure; 
/*��������� ������������*/
RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);   
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6 ;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
/*������������� ���������*/        
GPIO_Init(GPIOB, &GPIO_InitStructure);  
  while (1)
  {
    j=0;
      while (j<3)
      {
        for (i=0;i<2;i++)
        led3();
        led4();
        j++;       
      }
    j=0;
      while (j<3)
     {
        for (i=0;i<2;i++)
        led4();
        led3();
        j++;       
      }         
  }
}
void delay(int B) /*����������� ������� delay()*/
{
  int i;  
  for (i=0;i<=B;i++)
  ;
}
void led3(void)   /*����������� ������� led3()*/
{
  GPIO_SetBits(GPIOB, GPIO_Pin_7);
  delay(100000);
  GPIO_ResetBits(GPIOB, GPIO_Pin_7);
  delay(100000);
}
void led4(void)   /*����������� ������� led4()*/
{
  GPIO_SetBits(GPIOB, GPIO_Pin_6);
  delay(100000);
  GPIO_ResetBits(GPIOB, GPIO_Pin_6);
  delay(100000);
}
