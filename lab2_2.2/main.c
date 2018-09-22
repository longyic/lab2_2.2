/*
 * lab2_2.2.c
 *
 * Created: 2018/9/13 15:59:26
 * Author : Lenovo
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "uart.h"
volatile unsigned int edge1, edge2, pulse;
volatile int overflows, rising;

int main(void)
{
	uart_init();
	DDRB |= 0x02;                      //set PB1 as output
	TCNT1 = 0;                         //initial time counter
	
	PORTB |= 0x02;                                   //set PB1 as output
	TCCR1B |= (1 << CS10) | (1 << ICES1);            //no prescaling
	
	edge1 = 0;
	edge2 = 0;
	rising = 0;
	overflows = 0;                                  //initial overflow
	TIFR1 |= (1 << ICR1);                           //clear capture flag
	TIMSK1 |= (1 << ICIE1) | (1 << TOIE1);          //input capture interrupt enable
	sei();
	while (1)
	{
		if (TCNT1 >= 79)
		{
			PORTB ^= (1 << PORTB1);     //change from output to input
			TCNT1 = 0;                  //clear counter
		}

	}
}

ISR(TIMER1_CAPT_vect)
{
	if(!rising){
		TCCR1B &= ~(1 << ICES1);    //input capture negative edge
		TIFR1 |= 0x20;             //clear capture flag
		edge1 = ICR1;
		rising = 1;
		printf("edge1 is %u\n", edge1);
	}
	else{
		TIFR1 |= 0x20;             //clear capture flag
		TCCR1B |= (1 << ICES1);    //input capture negative edge
		edge2 = ICR1;
		rising = 0;
		if(edge2 < edge1)
		{
			overflows--;
		}
		pulse = overflows * 65536 + (edge2 - edge1);
		printf("pulse width is %u \n", pulse);
	}
}
		
ISR(TIMER1_OVF_vect)
{
	overflows++;
}
