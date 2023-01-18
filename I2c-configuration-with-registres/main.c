

#include "RccConfig.h"
#include "Delay.h"
#include "I2C.h"
#include "lcd.h"

#define TRIG_PIN GPIO_PIN_15
#define TRIG_PORT GPIOA



void HCSR04_Init(void)
{
    // Enable clock for TRIG_PORT and TIM6
    RCC->AHBENR |= RCC_AHBENR_TRIG_PORT_EN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

    // Configure TRIG_PIN as output
    TRIG_PORT->MODER |= (1 << (TRIG_PIN * 2));

    // Configure TIM6 in input capture mode
    TIM6->CCMR1 |= TIM_CCMR1_CC1S_0; // Select input on TI1
    TIM6->CCER |= TIM_CCER_CC1E;  // Enable capture
    TIM6->ARR = 0xffff;  // Set the autoreload value
    TIM6->PSC = (SystemCoreClock / 1000000) - 1;  // Set the prescaler
    TIM6->EGR |= TIM_EGR_UG;  // Update registers

    // Enable interrupt for TIM2
    NVIC_EnableIRQ(TIM6_IRQn);
    NVIC_SetPriority(TIM6_IRQn, 0);
}








void HCSR04_Read (void)
{
    TRIG_PORT->BSRR = TRIG_PIN;  // pull the TRIG pin HIGH
    delay(10);  // wait for 10 us
    TRIG_PORT->BRR = TRIG_PIN;  // pull the TRIG pin low

    TIM2->DIER |= TIM_DIER_CC1IE;
}  




void TIM6_IRQHandler(void)
{
    if (TIM6->SR & TIM_SR_CC1IF) // if the interrupt source is channel1
    {
        if (Is_First_Captured==0) // if the first value is not captured
        {
            IC_Val1 = TIM6->CCR1; // read the first value
            Is_First_Captured = 1;  // set the first captured as true
            // Now change the polarity to falling edge
            TIM6->CCER &= ~TIM_CCER_CC1P;
        }

        else if (Is_First_Captured==1)   // if the first is already captured
        {
            IC_Val2 = TIM6->CCR1;  // read second value
            TIM6->CNT = 0;  // reset the counter

            if (IC_Val2 > IC_Val1)
            {
                Difference = IC_Val2-IC_Val1;
            }

            else if (IC_Val1 > IC_Val2)
            {
                Difference = (0xffff - IC_Val1) + IC_Val2;
            }

            Distance = Difference * .034/2;
            Is_First_Captured = 0; // set it back to false

            // set polarity to rising edge
            TIM6->CCER |= TIM_CCER_CC1P;
            TIM6->DIER &= ~TIM_DIER_CC1IE;
        }
    }
}






int main ()
{
	SysClockConfig ();
	TIM6Config ();
	I2C_Config ();	
	lcd_init();
	
	
	//MPU6050_Init ();
	while (1)
	{
		//MPU6050_Read_Accel ();
		HCSR04_Read();
	  lcd_put_cur(0, 0);

	  lcd_send_string ("Dist= ");


	  //lcd_send_data((Distance/10) + 48);   // 100th pos
	  //lcd_send_data(((Distance/10)%10) +48);  // 10th pos

	  lcd_send_data((Distance %10)+48);  // 1st pos
	  lcd_send_string(" cm");
		Delay_ms (1000);
	}
}

