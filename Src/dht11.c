//
// Created by LMF on 2023/12/31.
//
#include "dht11.h"
#include "tim.h"
/*
 *TIM3定时器实现us级延时
 */


//输出
void DHT11_OUT(void ){
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /*Configure GPIO pin : PB12 */
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}
//输入
void DHT11_IN(void ){
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /*Configure GPIO pin : PB12 */
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}
//复位DHT11
void DHT11_Reset(void ){
    DHT11_OUT();                                          //设置为输出
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_11, GPIO_PIN_RESET);     //拉低引脚
    HAL_Delay(20);                                             //延迟20ms
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_11, GPIO_PIN_SET);       //拉高引脚
    Delay_us(30);
}
//等待DHT11回应
uint8_t DHT11_Check(void)
{
    uint8_t retry=0;
    DHT11_IN();                                           //设置为输入
    while(GPIO_PIN_SET==HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_11) && retry<100)
    {
        retry++;
        Delay_us(1);
    }
    if(retry>=100)
        return 1;
    else
        retry=0;

    while(GPIO_PIN_RESET==HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_11) && retry<100)
    {
        retry++;
        Delay_us(1);
    }
    if(retry>=100)
        return 1;
    return 0;
}
//从DHT11读取一个位
uint8_t DHT11_Read_Bit(void)
{
    uint8_t retry=0;
    while(GPIO_PIN_SET==HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_11) && retry<100)
    {
        retry++;
        Delay_us(1);
    }
    retry=0;

    while(GPIO_PIN_RESET==HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_11) && retry<100)
    {
        retry++;
        Delay_us(1);
    }
    Delay_us(40);

    if(GPIO_PIN_SET==HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_11))
        return 1;
    else
        return 0;
}

//从DHT11读取一个字节
uint8_t DHT11_Read_Byte(void)
{
    uint8_t dat=0;
    for(uint8_t i=0;i<8;i++)
    {
        dat <<= 1;
        dat |= DHT11_Read_Bit();
    }
    return dat;
}
//从DHT11读取一次数据
uint8_t DHT11_Read_Data(uint8_t* humi,uint8_t* temp)
{
    uint8_t buf[5];
    DHT11_Reset();
    if(DHT11_Check() == 0)
    {
        for(uint8_t i=0;i<5;i++)
            buf[i]=DHT11_Read_Byte();
        if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
        {
            *humi=buf[0];       //这里省略小数部分
            *temp=buf[2];
        }
    }
    else
        return 1;
    return 0;
}