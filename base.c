#define F_CPU 1000000UL
#define RESOLUTION 0xff

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <math.h>
/*
PA7 input
PA0-3 rgbw
*/
volatile uint8_t r=0,g=0,b=0,w=0;

void pwm()
{
	static uint8_t cnt = 0;
	cnt=(cnt+1)&RESOLUTION;
	PORTA=(r>cnt)|((g>cnt)<<1)|((b>cnt)<<2)|((w>cnt)<<3);
}

void rgb(uint32_t color, uint8_t bright)
{
	static float rr=0,gg=0,bb=0;
	while(color>3*RESOLUTION)
		color-=3*RESOLUTION;
	if(color<=RESOLUTION)
	{
		rr=RESOLUTION-color;
		gg=color;
	}
	else if(color>RESOLUTION && color<=2*RESOLUTION)
	{
		gg=2*RESOLUTION-color;
		bb=color-RESOLUTION;
	}
	else
	{
		bb=3*RESOLUTION-color;
		rr=color-2*RESOLUTION;
	}
	rr=rr*bright/RESOLUTION;
	gg=gg*bright/RESOLUTION;
	bb=bb*bright/RESOLUTION;

	r=(uint8_t) rr;
	g=(uint8_t) gg;
	b=(uint8_t) bb;
}

/*SIGNAL(USI_OVF_vect)
{
	rgb(0,RESOLUTION);
	USIDR=64;
	//reset counter & clear flag
	USISR=(1<<USIOIF);
}*/

SIGNAL(TIM0_COMPA_vect)
{
	static uint32_t color=0;
	rgb(color,RESOLUTION);
	color++;
}

int main(void)
{
	//setup spi
/*	DDRA = (1<<5)|(1<<4);	//data out and sck as output
	PORTA = (1<<5)|(1<<4)|(1<<6);
	USICR = (1<<USIWM0)|(1<<USICS1)|(1<<USIOIE);
	USISR = (1<<USIOIF);
*/
	//setup timer0
	TCCR0B = (1<<CS02);
	TIMSK0 = (1<<OCIE0A);
	OCR0A = 0xff;
	
	//setup rgb
	DDRA |= 0xf;	//first 4 pins out
	PORTA &= 0xf0;
	DDRB = 0xff;
	PORTB=0;

	sei();	//enable interrupts

	rgb(6*RESOLUTION/5,RESOLUTION);
	while(1)
	{
		pwm();
	}
}
