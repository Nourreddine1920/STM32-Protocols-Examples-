### STM32 Protocols Examples 🚀 


#### ✔ HC-04 Interface 


#### ✔  LCD-Ultrasonic-Interface 


#### ✔ Joystick Application with ADC Protocol 


#### ✔ KY-016-RGB


#### ✔ Infrared Remote


#### ✔ SPI Interface From Scratch 👇


##### Introduction 💡

- I will cover both sending and receiving data in the master mode.
 Also I am going to use the ADXL345 for the demonstration

> SPI communication uses 4 Pins 

1. MOSI Master Out Slave In is used to send the data to the Slave
2. MISO Master in Slave out is used to receive the data from the slave
3. SCK Serial Clock is used to keep the master and slave in sync
4. SS Slave select is used to select and unselect the slave


##### Slave select (NSS) pin management 🤔

In Master mode, Slave Select management can be done using two different ways

- Software NSS management : This means that we want to manage the slave using the software. So we don’t have to rely on the physical NSS pin, and instead we can use any pin from the mcu as the Slave Select pin.
- Hardware NSS management : Here we have to use the actual NSS pin (fixed pin) to control the slave device. The NSS signal is driven low as soon as the SPI is enabled in master mode, and is kept low until the SPI is disabled.


##### SPI Configuration ❓

The configuration for the SPI is relatively simpler than what we have seen in other peripherals. Here we only have to modify 1 register. Let’s see the steps to configure the SPI

```
/************** STEPS TO FOLLOW *****************
1. Enable SPI clock
2. Configure the Control Register 1
3. Configure the CR2
************************************************/
```

> SPI Configuration 

```
void SPIConfig (void)
{
  RCC->APB2ENR |= (1<<12);  // Enable SPI1 CLock
	
  SPI1->CR1 |= (1<<0)|(1<<1);   // CPOL=1, CPHA=1
	
  SPI1->CR1 |= (1<<2);  // Master Mode
	
  SPI1->CR1 |= (3<<3);  // BR[2:0] = 011: fPCLK/16, PCLK2 = 80MHz, SPI clk = 5MHz
	
  SPI1->CR1 &= ~(1<<7);  // LSBFIRST = 0, MSB first
	
  SPI1->CR1 |= (1<<8) | (1<<9);  // SSM=1, SSi=1 -> Software Slave Management
	
  SPI1->CR1 &= ~(1<<10);  // RXONLY = 0, full-duplex
	
  SPI1->CR1 &= ~(1<<11);  // DFF=0, 8 bit data
	
  SPI1->CR2 = 0;
}
```

1. First of all enable the SPI 1 clock in the RCC_APB2ENR Register
2. Now will modify the CPOL and CPHA bits according to the slave requirement ( watch video if you don’t know what this means)
3. Enable the master mode
4. Next is the prescalar. The slave device can support upto 5 MBits/s, so I am keeping the presclalar of 16
This will divide the APB2 clock (80 MHz) by 16, and bring it down to 5MHz
5. The data format is selected as MSB first
6. Then we will configure the software slave management. Like I said in the beginning, we will use the software to control the slave, so we need to set these SSM and SSI bits
7. Next we will configure the full duplex mode by resetting the RXONLY (10th) bit
8. Next is the data length bit, and we will keep the 8 bit data.
9. Then reset the entire CR2 register, since we will not be setting up any DMA or Interrupt in this tutorial


##### SPI Transmit  ❓

- Before Transmitting the data, we will wait for the TXE (Transmit Register Empty) bit in the Status Register to set. This indicates that the Transmit Register is empty and we can load the data
- After transmitting the data, we will wait for the BSY (Busy) bit in the Status Register to reset. This will indicate that the SPI is not busy in communication anymore and we can proceed with other things


```
void SPI_Transmit (uint8_t *data, int size)
{
	
	/************** STEPS TO FOLLOW *****************
	1. Wait for the TXE bit to set in the Status Register
	2. Write the data to the Data Register
	3. After the data has been transmitted, wait for the BSY bit to reset in Status Register
	4. Clear the Overrun flag by reading DR and SR
	************************************************/		
	
	int i=0;
	while (i<size)
	{
	   while (!((SPI1->SR)&(1<<1))) {};  // wait for TXE bit to set -> This will indicate that the buffer is empty
	   SPI1->DR = data[i];  // load the data into the Data Register
	   i++;
	}	
	
	
/*During discontinuous communications, there is a 2 APB clock period delay between the
write operation to the SPI_DR register and BSY bit setting. As a consequence it is
mandatory to wait first until TXE is set and then until BSY is cleared after writing the last
data.
*/
	while (!((SPI1->SR)&(1<<1))) {};  // wait for TXE bit to set -> This will indicate that the buffer is empty
	while (((SPI1->SR)&(1<<7))) {};  // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication	
	
	//  Clear the Overrun flag by reading DR and SR
	uint8_t temp = SPI1->DR;
					temp = SPI1->SR;
	
}
```

##### SPI Recieve  ❓

>  The receiving process is as follows:


- Before receiving the data, we must send some dummy byte to the device. Since the slave device is in the transmission mode, this dummy byte does not change the registers or the data of the registers
- On receiving the dummy byte, the device transmits one byte of data.
- This will set the RXNE (Receive Buffer Not Empty) bit. This will indicate that there is some data in the Data Register, which is ready to be read.
And later we can copy the data from the Data Register into our buffer. This clears the RXNE bit


```
void SPI_Receive (uint8_t *data, int size)
{
	/************** STEPS TO FOLLOW *****************
	1. Wait for the BSY bit to reset in Status Register
	2. Send some Dummy data before reading the DATA
	3. Wait for the RXNE bit to Set in the status Register
	4. Read data from Data Register
	************************************************/		

	while (size)
	{
		while (((SPI1->SR)&(1<<7))) {};  // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication
		SPI1->DR = 0;  // send dummy data
		while (!((SPI1->SR) &(1<<0))){};  // Wait for RXNE to set -> This will indicate that the Rx buffer is not empty
	  *data++ = (SPI1->DR);
		size--;
	}	
}
```

- As you can see above, we wait for the busy flag to reset.
- Then we send some dummy byte to the device. I am transmitting 0
- next we wait for the RXNE bit to set
- And finally copy the data from the DR into our buffer

##### GPIO Configuration   ❓

> I am using the following Pins for the SPI 1:

- PA5 -> CLK
- PA6 -> MISO
- PA7 -> MOSI
- PA9 -> Slave Select

The first three pins need to be set in the alternate function mode, and the PA9 will be set as the general output pin.

The Alternate function mode depends on the microcontroller. For example, let’s see the Alternate function description for F446RE

> You can see in the picture above that the SPI 1 is the AF5. Also since I am using Pins PA5, 6 and 7, I am going to use the AFRL Register. In case you are using the Pins 8 to 15, you must use the AFRH register

```
void GPIOConfig (void)
{
	RCC->AHB1ENR |= (1<<0);  // Enable GPIO Clock
	
	GPIOA->MODER |= (2<<10)|(2<<12)|(2<<14)|(1<<18);  // Alternate functions for PA5, PA6, PA7 and Output for PA9
	
	GPIOA->OSPEEDR |= (3<<10)|(3<<12)|(3<<14)|(3<<18);  // HIGH Speed for PA5, PA6, PA7, PA9
	
	GPIOA->AFR[0] |= (5<<20)|(5<<24)|(5<<28);   // AF5(SPI1) for PA5, PA6, PA7
}
```

- First we will enable the GPIOA clock
- Then select the Alternate Function mode for PA5, PA6 and PA7 and the output mode for PA9
- Next we will set the speed for all four pins. The speed is set to HIGH Speed
- And finally configure the Alternate Function in the AFR[0] (AFRL)


#### F103 GPIO CONFIG   ❓


> The Configuration in F103 is slightly different. F103 do not have the Alternate Function Registers and therefore we have to use the Control Register to configure the SPI pins

Below is the Picture from the F103 Reference manual, suggesting the configuration for the SPI

```
void GPIOConfig (void)
{
	RCC->APB2ENR |=  (1<<2);  // Enable GPIOA clock
	
	GPIOA->CRL = 0;
	GPIOA->CRL |= (11U<<20);   // PA5 (SCK) AF output Push Pull
	GPIOA->CRL |= (11U<<28);   // PA7 (MOSI) AF output Push Pull
	GPIOA->CRL |= (1<<26);    // PA6 (MISO) Input mode (floating)
	GPIOA->CRL |= (3<<16);    // PA4 used for CS, GPIO Output 
	
}
```

- Here we will enable the GPIOA clock
- Then reset the entire Control Register
- Now set the AF output Push Pull mode for PA5 -> Clock
- Do the Same for the PA7 -> MOSI
- We need to set the Alternate Function input mode for the PA6 -> MISO
- And set the general Output mode for the PA4 -> SS



