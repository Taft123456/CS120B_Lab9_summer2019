/*
 * Lab9_PWM.c
 *
 * Created: 7/9/2019 2:50:30 PM
 * Author : Taft
 */ 

#include <avr/io.h>

void set_PWM(double frequency) {
	static double current_frequency;
	if (frequency != current_frequency) {

		if (!frequency) {TCCR3B &= 0x08;} 
		else {TCCR3B |= 0x03;}
		if (frequency < 0.954) {OCR3A = 0xFFFF;}
		else if (frequency > 31250) {OCR3A = 0x0000;}
		else {OCR3A = (short)(8000000 / (128 * frequency)) - 1;}
		TCNT3 = 0; 
		current_frequency = frequency;
	}
}
void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}
void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

enum States{INIT, START, TURN_ON, TURN_OFF} state;	
	
void Tick()
{
	switch(state) 
	{
		case INIT:
			state = START;
			break;
		case START:
			if((~PINA & 0x07) != 0x00)
			{
				state = TURN_ON;
				break;
			}
			else
			{
				state = START;
				break;
			}
		case TURN_ON:
			if((~PINA & 0x07) != 0x00)
			{
				state = TURN_ON; break;
			}
			else if((~PINA & 0x07) == 0x03)
			{
				state = TURN_OFF; break;
			}
			else if((~PINA & 0x07) == 0x05)
			{
				state = TURN_OFF; break;
			}
			else if((~PINA & 0x07) == 0x06)
			{
				state = TURN_OFF; break;
			}
			else if((~PINA & 0x07) == 0x07)
			{
				state = TURN_OFF; break;
			}
			else
			{
				state = TURN_OFF; break;
			}
		case TURN_OFF:
			state = START;
			break;
		default:
			break;
	}
	switch(state)
	{
		case INIT:
			break;
		case START:
			set_PWM(0);
			break;
		case TURN_ON:
			if((~PINA & 0x07) == 0x01)
			{
				set_PWM(261.63);
				break;
			}
			else if((~PINA & 0x07) == 0x02)
			{
				set_PWM(293.66);
				break;
			}
			else if((~PINA & 0x07) == 0x04)
			{
				set_PWM(329.63);
				break;
			}
		case TURN_OFF:
			set_PWM(0);
			break;
		default:
			break;
	}
}

int main(void)
{
	DDRA = 0x00; 
	DDRB = 0xFF; 
	PORTA = 0xFF;
	PORTB = 0x00;
	state = INIT;
	PWM_on();
    while (1) 
    {
		Tick();
    }
}

