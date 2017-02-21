
#define MAINS_PERIOD 16667 //number of half-microseconds in a half-period of the mains waveform
#define MAINS_FREQUENCY 60 

const unsigned char channel_z = 8; // Input from zero crossing detector
const unsigned char channel_a = 9; // Output to Opto Triac pin, channel a
const unsigned char channel_b = 10; // Output to Opto Triac pin, channel b


ISR(TIMER1_CAPT_vect) {
	GTCCR = _BV(TSM);
	TCNT1 = TCNT1 - ICR1 - MAINS_PERIOD + 1 + 300;
	GTCCR = 0;
}
ISR(TIMER1_COMPA_vect) {
	delayMicroseconds(8);
	//OC1A = 0; // what do I do here?
	TCCR1A &=~ _BV(COM1A0);
	TCCR1C = _BV(FOC1A);
	TCCR1A |= _BV(COM1A0);

	PORTB &=~ _BV(PORTB1);

}
ISR(TIMER1_COMPB_vect) {
	delayMicroseconds(8);
	//OC1B = 0; // what do I do here?
}


void setup() {

	TCCR1A = _BV(COM1A0) | _BV(COM1A1) | _BV(COM1B0) | _BV(COM1B1); 
	// set output on compare match, normal mode, prescaler /8
	TCCR1B = _BV(CS11) | _BV(ICNC1) | _BV(ICES1);
	// input capture noise cancel, positive edge
	TIMSK1 = _BV(ICIE1) | _BV(OCIE1A) | _BV(OCIE1B);
	// generate interrupts for input capture, A/B compare match

	pinMode(channel_z, INPUT);	//set output buffers
	pinMode(channel_a, OUTPUT); 
	pinMode(channel_b, OUTPUT);

	Serial.begin(19200);
}


void loop() {
	if (Serial.available()) {
		processSyncMessage();
	}

	delay(100);
	digitalClockDisplay();
}

void digitalClockDisplay(){
	Serial.print(" ICR1:");
	Serial.print(ICR1); 
	Serial.print(" A:");
	Serial.print(OCR1A); 
	Serial.print(" B:");
	Serial.print(OCR1B);
	Serial.print("\n");
}

/*  code to process time sync messages from the serial port   */
#define A_SET_HEADER  'A' // Header tag for alarm set message
#define B_SET_HEADER  'B' // Header tag for alarm set message

unsigned long processSyncMessage() {
	unsigned long t = 0L;
	const unsigned long DEFAULT_TIME = 1486072960; // Feb 2 2017 

	switch(Serial.read()){

	case A_SET_HEADER:
		t = Serial.parseInt();
		OCR1A = t;
		break;
	case B_SET_HEADER:
		t = Serial.parseInt();
		OCR1B = t;
		break;
	}


}









