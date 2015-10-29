#define F_CPU 1000000UL
#define RESOLUTION 0x7f

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <math.h>
/*
PA7 input
PA0-3 rgbw
*/
volatile char cnt=0;
volatile uint16_t pwm;
volatile uint8_t r=0,g=0,b=0,w=0;
int i=0,j=0;

SIGNAL(TIM0_COMPA_vect)
{
	TCNT0 = 0;
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

int main(void)
{
	TCCR0B = (1<<CS00);	//no prescale
	TIMSK0 = (1<<OCIE0A);	//interrupt on compare match
	OCR0A = 0xf;	//interrupt every 0xf cycles
	sei();	//enable interrupts
	
	DDRA = 0xf;	//first 4 pins out
	PORTA= 0;
	DDRB = 0xff;
	PORTB=0;
	pwm=0;
	srand(1);
	uint32_t goal = 0, current=0;
	while(1)
	{
		rgb(current,RESOLUTION);
		if(current<goal)
			current++;
		else if(current>goal)
			current--;
		else
		{
			if(goal<3*RESOLUTION)
			{
				current+=3*RESOLUTION;
				goal+=3*RESOLUTION;
			}
			goal+=(rand() %(4*RESOLUTION))-2*RESOLUTION;
		}
	}
}
