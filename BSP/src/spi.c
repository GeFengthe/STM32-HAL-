#include "spi.h"

SPI_HandleTypeDef	SPI_HandleStruct;


void Init_spi(void)
{
	
	SPI_HandleStruct.Instance = SPI3;
	SPI_HandleStruct.Init.Mode = SPI_MODE_MASTER;					//主机模式
	SPI_HandleStruct.Init.Direction = SPI_DIRECTION_2LINES;			//双线模式
	SPI_HandleStruct.Init.DataSize = SPI_DATASIZE_8BIT;
	SPI_HandleStruct.Init.CLKPolarity = SPI_POLARITY_HIGH;			//串行同步时钟的空闲状态为高电平
	SPI_HandleStruct.Init.CLKPhase = SPI_PHASE_2EDGE;				//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_HandleStruct.Init.NSS = SPI_NSS_SOFT;
	SPI_HandleStruct.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;	//定义波特率预分频的值:波特率预分频值为256
	SPI_HandleStruct.Init.FirstBit =SPI_FIRSTBIT_MSB;					//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_HandleStruct.Init.TIMode = SPI_TIMODE_DISABLE;					//关闭TI模式
	SPI_HandleStruct.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;	//关闭硬件CRC校验
	SPI_HandleStruct.Init.CRCPolynomial = 7;							//crc多项式的值
	HAL_SPI_Init(&SPI_HandleStruct);
    __HAL_SPI_ENABLE(&SPI_HandleStruct);
	
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOB_CLK_ENABLE();       //使能GPIOB时钟
    __HAL_RCC_SPI3_CLK_ENABLE();

    //PB3.5
    GPIO_Initure.Pin=GPIO_PIN_3|GPIO_PIN_5;
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;              //复用推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;                  //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;   //80Mhz           
    GPIO_Initure.Alternate=GPIO_AF6_SPI3;           //PB3.5复用为SPI3
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
}

//读写一个字节
uint8_t SPI3_ReadWriteByte(uint8_t TxData)
{
	uint8_t Rxdata;
	Rxdata =HAL_SPI_TransmitReceive(&SPI_HandleStruct,&TxData , &Rxdata,1,1000);
	return Rxdata;
}

//写一个字节
uint8_t SPI3_WriteByte(uint8_t *Txdata,uint16_t size)
{
	return HAL_SPI_Transmit(&SPI_HandleStruct,Txdata, size, 1000);
	
}
