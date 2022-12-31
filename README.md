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
