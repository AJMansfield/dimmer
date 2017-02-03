

#define MAINS_PERIOD 16667 //number of half-microseconds in a half-period of the mains waveform

const unsigned char channel_a = 9; // Output to Opto Triac pin, channel a
const unsigned char channel_b = 10; // Output to Opto Triac pin, channel b
const unsigned char channel_z = 3; // Output to Opto Triac pin, channel b


void set_ch_a(float x){
	OCR1A = (unsigned int)((1-x) * MAINS_PERIOD);
}
void set_ch_b(float x){
	OCR1B = (unsigned int)((1-x) * MAINS_PERIOD);
}

void zero_cross() {
	TCNT1 = 0;
}
void setup() {

	pinMode(channel_z, INPUT);
	pinMode(channel_a, OUTPUT);
	pinMode(channel_b, OUTPUT);

	TCCR1A = _BV(COM1A0) | _BV(COM1A1) | _BV(COM1B0) | _BV(COM1B1) | _BV(WGM11);
	//set inverted PWM output for pins 9 and 10
	TCCR1B = _BV(CS11) | _BV(WGM13) | _BV(WGM12);
	//set prescaler to x8 - OCR1A and OCR1B are in half-microseconds; set fast PWM with ICR1 as reset value
	ICR1 = MAINS_PERIOD - 30; //reset just before next phase


	attachInterrupt(1, zero_cross, RISING);
}

void loop() {
	for(float x = 0; x <= 1; x += 0.01){
		set_ch_a(x);
		set_ch_b(1-x);
		delay(100);
	}
	for(float x = 0; x <= 1; x += 0.01){
		set_ch_a(1-x);
		set_ch_b(x);
		delay(100);
	}
}
