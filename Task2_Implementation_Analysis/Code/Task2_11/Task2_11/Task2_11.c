/*
 * Task2_11.c
 *
 * Created: 20-01-2015 10:51:45 PM
 *  Author: Rishabh
 */ 


#define F_CPU 14745600
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <math.h> //included to support power function
#include "lcd.h"

unsigned char ADC_Conversion(unsigned char);
unsigned char ADC_Value;
unsigned char sharp1,sharp2, distance, adc_reading;
unsigned int value0,value1,value2;

//ADC pin configuration
void adc_pin_config (void)
{
	DDRF = 0x00; //set PORTF direction as input
	PORTF = 0x00; //set PORTF pins floating
	DDRK = 0x00; //set PORTK direction as input
	PORTK = 0x00; //set PORTK pins floating
}

//Function to Initialize PORTS
void port_init()
{
	lcd_port_config();
	adc_pin_config();
}


//Function to Initialize ADC
void adc_init()
{
	ADCSRA = 0x00;
	ADCSRB = 0x00;		//MUX5 = 0
	ADMUX = 0x20;		//Vref=5V external --- ADLAR=1 --- MUX4:0 = 0000
	ACSR = 0x80;
	ADCSRA = 0x86;		//ADEN=1 --- ADIE=1 --- ADPS2:0 = 1 1 0
}

//This Function accepts the Channel Number and returns the corresponding Analog Value
unsigned char ADC_Conversion(unsigned char Ch)
{
	unsigned char a;
	if(Ch>7)
	{
		ADCSRB = 0x08;
	}
	Ch = Ch & 0x07;
	ADMUX= 0x20| Ch;
	ADCSRA = ADCSRA | 0x40;		//Set start conversion bit
	while((ADCSRA&0x10)==0);	//Wait for ADC conversion to complete
	a=ADCH;
	ADCSRA = ADCSRA|0x10; //clear ADIF (ADC Interrupt Flag) by writing 1 to it
	ADCSRB = 0x00;
	return a;
}

// This Function prints the Analog Value Of Corresponding Channel No. at required Row
// and Coloumn Location.
void print_sensor(char row, char coloumn,unsigned char channel)
{
	ADC_Value = ADC_Conversion(channel);
	lcd_print(row, coloumn, ADC_Value, 3);
}

// This Function calculates the actual distance in millimeters(mm) from the input
// analog value of Sharp Sensor.
unsigned int Sharp_GP2D12_estimation(unsigned char adc_reading)
{
	float distance;
	unsigned int distanceInt;
	distance = (int)(10.00*(2799.6*(1.00/(pow(adc_reading,1.1546)))));
	distanceInt = (int)distance;
	if(distanceInt>800)
	{
		distanceInt=800;
	}
	return distanceInt;
}

void init_devices (void)
{
	cli(); //Clears the global interrupts
	port_init();
	adc_init();
	sei(); //Enables the global interrupts
}





int main(void)
{
	unsigned int value;
	init_devices();
	
	lcd_set_4bit();
	lcd_init();
	
    while(1)
    {
		
		//Prints value of Analog IR Proximity Sensor 3
		value0 =ADC_Conversion(6);
		/*print_sensor(2,2,6);*/
		lcd_print(2,2,value0,3); 							//Prints Value Of Distanc in MM measured by Sharp Sensor.
		
		sharp1 = ADC_Conversion(10);						//Stores the Analog value of front sharp connected to ADC channel 11 into variable "sharp"
		value1 = Sharp_GP2D12_estimation(sharp1);			//Stores Distance calsulated in a variable "value".
		lcd_print(2,7,value1,3); 							//Prints Value Of Distanc in MM measured by Sharp Sensor.
		
		sharp2 = ADC_Conversion(11);						//Stores the Analog value of front sharp connected to ADC channel 11 into variable "sharp"
		value2 = Sharp_GP2D12_estimation(sharp2);			//Stores Distance calsulated in a variable "value".
		lcd_print(2,13,value2,3); 							//Prints Value Of Distanc in MM measured by Sharp Sensor.
		if(value0<120 && value1<300 && value2>350)
		{
			lcd_cursor(1,3);
			lcd_string("BLOCK C");
		}	
		else if(value0>135 && value1<300 && value2>350)
		{
			lcd_cursor(1,3);
			lcd_string("BLOCK A");
		}
		else if(value0>135 && value1<300 && value2<300 && value1<750)
		{
			lcd_cursor(1,3);
			lcd_string("BLOCK B");
		}
		else if(value0<120 && value1<300 && value2<300 && value1<750)
		{
			lcd_cursor(1,3);
			lcd_string("BLOCK D");
		}	
		else if(value1==800 && value2==800)
		{
			lcd_cursor(1,3);
			lcd_string("        ");
		}
		else
		{
			lcd_cursor(1,3);
			lcd_string("        ");
		}		
			
    }
}