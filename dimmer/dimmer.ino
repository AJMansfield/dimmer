#include <Time.h>
#include <TimeAlarms.h>

#define MAINS_PERIOD 16667 //number of half-microseconds in a half-period of the mains waveform
#define MAINS_FREQUENCY 60 

const unsigned char channel_a = 9; // Output to Opto Triac pin, channel a
const unsigned char channel_b = 10; // Output to Opto Triac pin, channel b
const unsigned char channel_z = 3; // Output to Opto Triac pin, channel b



const float in[] = 
{ -0.            ,   9.25295293e-08,   2.52559560e-05,   9.66419104e-04,
   6.17766236e-03,   1.89552895e-02,   4.10991260e-02,   7.40639247e-02,
   1.15521833e-01,   1.64535545e-01,   2.20903015e-01,   2.80866796e-01,
   3.43594021e-01,   4.10268098e-01,   4.77649597e-01,   5.43711948e-01,
   6.07640486e-01,   6.68898386e-01,   7.26232887e-01,   7.78168688e-01,
   8.24150458e-01,   8.64133390e-01,   8.98217770e-01,   9.26632476e-01,
   9.49644260e-01,   9.67583008e-01,   9.80871218e-01,   9.90041286e-01,
   9.95740338e-01,   9.98723761e-01,   9.99838777e-01,   1.00000000e+00};
const float out[] =
{ 0.        ,  0.03225806,  0.06451613,  0.09677419,  0.12903226,
  0.16129032,  0.19354839,  0.22580645,  0.25806452,  0.29032258,
  0.32258065,  0.35483871,  0.38709677,  0.41935484,  0.4516129 ,
  0.48387097,  0.51612903,  0.5483871 ,  0.58064516,  0.61290323,
  0.64516129,  0.67741935,  0.70967742,  0.74193548,  0.77419355,
  0.80645161,  0.83870968,  0.87096774,  0.90322581,  0.93548387,
  0.96774194,  1.        };


//linear interpolation function
float multiMap(float val, const float* _in, const float* _out, uint8_t size){
	if (val <= _in[0]) return _out[0];
	if (val >= _in[size-1]) return _out[size-1];
	uint8_t pos = 1;
	while(val > _in[pos]) pos++;
	if (val == _in[pos]) return _out[pos];
	return (val - _in[pos-1]) * (_out[pos] - _out[pos-1]) / (_in[pos] - _in[pos-1]) + _out[pos-1];
}




void set_ch_a(float x){
	float y = multiMap(x, in, out, sizeof(in) / sizeof(float));
	OCR1A = (unsigned int)((1-y) * MAINS_PERIOD);
}
void set_ch_b(float x){
	float y = multiMap(x, in, out, sizeof(in) / sizeof(float));
	OCR1B = (unsigned int)((1-y) * MAINS_PERIOD);
}
float get_ch_a(){
	float y = constrain(1 - (1.0 * OCR1A / MAINS_PERIOD), 0, 1);
	float x = multiMap(y, out, in, sizeof(in) / sizeof(float));
	return x;
}
float get_ch_b(){
	float y = constrain(1 - (1.0 * OCR1B / MAINS_PERIOD), 0, 1);
	float x = multiMap(y, out, in, sizeof(in) / sizeof(float));
	return x;
}

volatile uint64_t zcount = 0;
uint64_t zoff = 0;
void set_ztime(time_t time){
	cli();
	uint64_t zc = zcount;
	sei();

	zoff = time * MAINS_FREQUENCY * 2 - zc;
}
time_t get_ztime(){
	cli();
	uint64_t zc = zcount;
	sei();

	return (zoff + zc) / (MAINS_FREQUENCY * 2);
}
void zero_cross() {
	TCNT1 = 0;
	zcount++;
}


#define FADE_TIME 1800 //seconds to fade in, 600 = 10 minutes
unsigned int fade_a_counter = 0;
unsigned int fade_b_counter = 0;
bool fade_a_cancel = false;
bool fade_b_cancel = false;

void fade_a_in(){
	Serial.println("fading in A");
	fade_a_counter = 0;
	fade_a_cancel = false;
	Alarm.timerOnce(1, fade_a_in_helper);
}
void fade_a_in_helper(){
	fade_a_counter++;
	if(fade_a_cancel){
		return;
	}
	if(fade_a_counter >= FADE_TIME){
		set_ch_a(1);
		return;
	}
	set_ch_a(1.0 * fade_a_counter / FADE_TIME);
	Alarm.timerOnce(1, fade_a_in_helper);
}
void fade_b_in(){
	Serial.println("fading in B");
	fade_b_counter = 0;
	fade_b_cancel = false;
	Alarm.timerOnce(1, fade_b_in_helper);
}
void fade_b_in_helper(){
	fade_b_counter++;
	if(fade_b_cancel){
		return;
	}
	if(fade_b_counter >= FADE_TIME){
		set_ch_b(1);
		return;
	}
	set_ch_b(1.0 * fade_b_counter / FADE_TIME);
	Alarm.timerOnce(1, fade_b_in_helper);
}
void turn_off(){
	Serial.println("turning off");
	fade_a_cancel = true;
	fade_b_cancel = true;
	set_ch_a(0);
	set_ch_b(0);
}


void setup() {
	pinMode(channel_z, INPUT);
	pinMode(channel_a, OUTPUT);
	pinMode(channel_b, OUTPUT);

	TCCR1A = _BV(COM1A0) | _BV(COM1A1) | _BV(COM1B0) | _BV(COM1B1) | _BV(WGM11);
	//set inverted PWM output for pins 9 and 10
	TCCR1B = _BV(CS11) | _BV(WGM13) | _BV(WGM12);
	//set prescaler to x8 - OCR1A and OCR1B are in half-microseconds; set fast PWM with ICR1 as reset value
	ICR1 = MAINS_PERIOD - 100; //reset just before next phase
	attachInterrupt(1, zero_cross, RISING);

	set_ch_a(0);
	set_ch_b(0);

	Serial.begin(9600);
	//setSyncProvider(get_ztime);
	setTime(12,0,0,1,1,17);
	set_ztime(now());

	Alarm.timerRepeat(1, everySecond); //index 0

	Alarm.alarmRepeat(12,0,10, fade_a_in); //index 1
	Alarm.alarmRepeat(12,0,30, fade_b_in); //index 2
	Alarm.alarmRepeat(12,0,50, turn_off); //index 3
	Alarm.disable(1);
	Alarm.disable(2);
	Alarm.disable(3);
}


void loop() {
	Alarm.delay(1000);

	
}

void everySecond(){
	if (Serial.available()) {
		processSyncMessage();
	}
	//digitalClockDisplay();
}

void digitalClockDisplay(){
	// digital clock display of the time
	Serial.print(hour());
	printDigits(minute());
	printDigits(second());
	Serial.print(" ");
	Serial.print(day());
	Serial.print(" ");
	Serial.print(month());
	Serial.print(" ");
	Serial.print(year()); 
	Serial.print(" A:");
	Serial.print(get_ch_a()); 
	Serial.print(" B:");
	Serial.print(get_ch_b()); 
	Serial.print(" T:");
	Serial.print(now()); 
	Serial.println(); 
}

void printDigits(int digits){
	// utility function for digital clock display: prints preceding colon and leading 0
	Serial.print(":");
	if(digits < 10)
		Serial.print('0');
	Serial.print(digits);
}

/*  code to process time sync messages from the serial port   */
#define TIME_HEADER  'T'  // Header tag for serial time sync message
#define A_SET_HEADER  'A' // Header tag for alarm set message
#define B_SET_HEADER  'B' // Header tag for alarm set message
#define C_SET_HEADER  'C'
#define A_DISABLE_HEADER 'X'
#define B_DISABLE_HEADER 'Y'
#define C_DISABLE_HEADER 'Z'
#define INQUIRY_HEADER '?'

unsigned long processSyncMessage() {
	unsigned long t = 0L;
	const unsigned long DEFAULT_TIME = 1486072960; // Feb 2 2017 

	switch(Serial.read()){
	case TIME_HEADER:
		t = Serial.parseInt();
		set_ztime(t);
		setTime(t);
		turn_off();
		Serial.println("set time");
		turn_off();
		break;
	case A_SET_HEADER:
		t = Serial.parseInt();
		Alarm.write(1, t);
		Serial.println("set channel A");
		turn_off();
		break;
	case B_SET_HEADER:
		t = Serial.parseInt();
		Alarm.write(2, t);
		Serial.println("set channel B");
		turn_off();
		break;
	case C_SET_HEADER:
		t = Serial.parseInt();
		Alarm.write(3, t);
		Serial.println("set channel C");
		turn_off();
		break;
	case A_DISABLE_HEADER:
		Alarm.disable(1);
		Serial.println("unset channel A");
		turn_off();
		break;
	case B_DISABLE_HEADER:
		Alarm.disable(2);
		Serial.println("unset channel B");
		turn_off();
		break;
	case C_DISABLE_HEADER:
		Alarm.disable(3);
		Serial.println("unset channel C");
		turn_off();
		break;
	case INQUIRY_HEADER:
		Serial.println(now());
		Serial.println(Alarm.read(1));
		Serial.println(Alarm.read(2));
		Serial.println(Alarm.read(3));
		Serial.println(get_ch_a()); 
		Serial.println(get_ch_b()); 

	default:
		break;
	}


}









