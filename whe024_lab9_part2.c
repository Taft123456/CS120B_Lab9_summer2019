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

enum States{INIT, START, INCR, DECR, WAIT, SWITCH} state;
double frequency_list[9] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25, 0};
double tp1 = 0;
unsigned char b = 0x00;
unsigned char t = 0x07;
unsigned char pos = 0x00;
unsigned char ck = 0x00;
unsigned char tp2 = 0x00;	

void Tick()
{
	switch(state) 
	{
		case INIT:
			state = START;
			break;
		case START:
			if((~PINA & 0x07) == 0x01)
			{
				state = INCR;
				break;
			}
			else if((~PINA & 0x07) == 0x02)
			{
				state = DECR;
				break;
			}
			else if((~PINA & 0x07) == 0x04)
			{
				state = SWITCH;
				break;
			}
			else
			{
				state = START;
				break;
			}
		case INCR:
			tp1 = frequency_list[pos];
			state = WAIT;
			break;
		case DECR:
			tp1 = frequency_list[pos];
			state = WAIT;
			break;
		case WAIT:
			if((~PINA & 0x07) == 0x00)
			{
				state = START;
				break;
			}
			else
			{
				state = WAIT;
				break;
			}
		case SWITCH:
			state = WAIT;
			break;
		default:
			break;
	}
	switch(state) 
	{
		case INIT:
			break;
		case START:
			break;
		case INCR:
			if((pos + 1) > t)
			{
				pos = t;
				break;
			}
			else
			{
				++pos;
				break;
			}
		case DECR:
			if((pos - 1) < b)
			{
				pos = b;
				break;
			}
			else
			{
				--pos;
				break;
			}
		case WAIT:
			tp1 = frequency_list[pos];
			set_PWM(tp1);
			break;
		case SWITCH:
			if(ck)
			{
				PWM_off();
				tp2 = pos;
				pos = 8;
				ck = 0;
				break;
			}
			else
			{
				PWM_on();
				pos = tp2;
				ck = 1;
				break;
			}
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
	ck = 1;
	while(1)
	{
		Tick();
	}
}