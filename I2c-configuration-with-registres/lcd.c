/** Put this in the src folder **/

#include "lcd.h"
#include "Delay.h"
#include "I2C.h"

//extern I2C_HandleTypeDef hi2c1;  // change your handler here accordingly

#define LCD_ADDRESS 0x3E // I2C address of the LCD
#define LCD_COMMAND_MODE 0x00 // Command register address
#define LCD_DATA_MODE 0x40 // Data register address
#define LCD_CLEARDISPLAY 0x01 // Clear display command
#define LCD_RETURNHOME 0x02 // Return cursor to home position command
#define LCD_ENTRYMODESET 0x04 // Set entry mode command
#define LCD_DISPLAYCONTROL 0x08 // Set display control command
#define LCD_CURSORSHIFT 0x10 // Set cursor shift command
#define LCD_FUNCTIONSET 0x20 // Set function command
#define LCD_SETCGRAMADDR 0x40 // Set character generator RAM address command
#define LCD_SETDDRAMADDR 0x80 // Set display data RAM address command



void LCD_Write (uint8_t Address, uint8_t Reg, uint8_t Data[4])
{
	I2C_Start ();
	I2C_Address (Address);
	I2C_Write (Reg);
	I2C_Write (Data[4]);
	I2C_Stop ();
}

void LCD_Read (uint8_t Address, uint8_t Reg, uint8_t *buffer, uint8_t size)
{
	I2C_Start ();
	I2C_Address (Address);
	I2C_Write (Reg);
	I2C_Start ();  // repeated start
	I2C_Read (Address+0x01, buffer, size);
	I2C_Stop ();
}



void lcd_send_cmd (char cmd)
{
  char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd&0xf0);
	data_l = ((cmd<<4)&0xf0);
	data_t[0] = data_u|0x0C;  //en=1, rs=0
	data_t[1] = data_u|0x08;  //en=0, rs=0
	data_t[2] = data_l|0x0C;  //en=1, rs=0
	data_t[3] = data_l|0x08;  //en=0, rs=0
	LCD_Write(LCD_ADDRESS , LCD_COMMAND_MODE  ,data_t);
	//HAL_I2C_Master_Transmit (&hi2c1, SLAVE_ADDRESS_LCD,(uint8_t *) data_t, 4, 100);
}

void lcd_send_data (char data)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_u|0x0D;  //en=1, rs=0
	data_t[1] = data_u|0x09;  //en=0, rs=0
	data_t[2] = data_l|0x0D;  //en=1, rs=0
	data_t[3] = data_l|0x09;  //en=0, rs=0
	LCD_Write(LCD_ADDRESS , LCD_DATA_MODE  ,data_t);
	//HAL_I2C_Master_Transmit (&hi2c1, SLAVE_ADDRESS_LCD,(uint8_t *) data_t, 4, 100);
}

void lcd_clear (void)
{
	lcd_send_cmd (0x80);
	for (int i=0; i<70; i++)
	{
		lcd_send_data (' ');
	}
}

void lcd_put_cur(int row, int col)
{
    switch (row)
    {
        case 0:
            col |= 0x80;
            break;
        case 1:
            col |= 0xC0;
            break;
    }

    lcd_send_cmd (col);
}


void lcd_init (void)
{
	// 4 bit initialisation
	Delay_ms(50);  // wait for >40ms
	lcd_send_cmd (0x30);
	Delay_ms(5);  // wait for >4.1ms
	lcd_send_cmd (0x30);
	Delay_ms(1);  // wait for >100us
	lcd_send_cmd (0x30);
	Delay_ms(10);
	lcd_send_cmd (0x20);  // 4bit mode
	Delay_ms(10);

  // dislay initialisation
	lcd_send_cmd (0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	Delay_ms(1);
	lcd_send_cmd (0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
	Delay_ms(1);
	lcd_send_cmd (0x01);  // clear display
	Delay_ms(1);
	Delay_ms(1);
	lcd_send_cmd (0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	Delay_ms(1);
	lcd_send_cmd (0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
}

void lcd_send_string (char *str)
{
	while (*str) lcd_send_data (*str++);
}
