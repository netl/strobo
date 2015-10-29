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

void rgb(uint32_t color)
{
	while(color>3*RESOLUTION)
		color-=3*RESOLUTION;
	if(color<=RESOLUTION)
	{
		r=RESOLUTION-color;
		g=color;
	}
	else if(color>RESOLUTION && color<=2*RESOLUTION)
	{
		g=2*RESOLUTION-color;
		b=color-RESOLUTION;
	}
	else
	{
		b=3*RESOLUTION-color;
		r=color-2*RESOLUTION;
	}
}

int main(void)
{
	TCCR0B = (1<<CS00);	//no prescale
	TIMSK0 = (1<<OCIE0A);	//interrupt on compare match
	OCR0A = 0xf;	//interrupt every 0xf cycles
	//TCCR0 |= (1<<CS00);	//timer/counter0 with no prescale
	//TIMSK |= (1<<TOIE0);	//enable interrupt for timer/counter0
	sei();	//enable interrupts
	
	DDRA = 0xf;	//first 4 pins out
	PORTA= 0;
	DDRB = 0xff;
	PORTB=0;
	pwm=0;
	srand(1);
	uint32_t goal = 0, current=0;
	uint32_t i;
	while(1)
	{	
		/*for(i = 0 ; i<3*RESOLUTION; i++)
		{
			rgb(i);
			_delay_ms(5);
		}*/
		_delay_ms(5);
		rgb(current);
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
