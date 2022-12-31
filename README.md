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


