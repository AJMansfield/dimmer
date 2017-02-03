#include <Time.h>
#include <TimeAlarms.h>

#define MAINS_PERIOD 16667 //number of half-microseconds in a half-period of the mains waveform
#define MAINS_FREQUENCY 60 

const unsigned char channel_a = 9; // Output to Opto Triac pin, channel a
const unsigned char channel_b = 10; // Output to Opto Triac pin, channel b
const unsigned char channel_z = 3; // Output to Opto Triac pin, channel b


void set_ch_a(float x){
	OCR1A = (unsigned int)((1-x) * MAINS_PERIOD);
}
void set_ch_b(float x){
	OCR1B = (unsigned int)((1-x) * MAINS_PERIOD);
}
float get_ch_a(){
	return constrain(1 - (1.0 * OCR1A / MAINS_PERIOD), 0, 1);
}
float get_ch_b(){
	return constrain(1 - (1.0 * OCR1B / MAINS_PERIOD), 0, 1);
}

volatile unsigned long zcount = 0;
unsigned long zoff = 0;
void set_ztime(time_t time){
	cli();
	unsigned long zc = zcount;
	sei();

	zoff = time * MAINS_FREQUENCY * 2 - zc;
}
time_t get_ztime(){
	cli();
	unsigned long zc = zcount;
	sei();

	return (zoff + zc) / (MAINS_FREQUENCY * 2);
}
void zero_cross() {
	TCNT1 = 0;
	zcount++;
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

	set_ch_a(0);
	set_ch_b(0);

	Serial.begin(9600);
	//setSyncProvider(get_ztime);
	//setTime(22,25,0,1,1,17);

	Alarm.alarmRepeat(22,25,5, MorningAlarm);  // 8:30am every day
	Alarm.alarmRepeat(22,25,10,EveningAlarm);  // 5:45pm every day 
	Alarm.alarmRepeat(dowThursday,22,25,7,WeeklyAlarm);  // 8:30:30 every Saturday 
}


void loop() {
	if (Serial.available()) {
		time_t t = processSyncMessage();
		if (t != 0) {
			set_ztime(t);   // set the RTC and the system time to the received value
			setTime(t);
			set_ch_a(0);
			set_ch_b(0);
		}
	}
	Alarm.delay(1000);
	digitalClockDisplay();
}

// functions to be called when an alarm triggers:
void MorningAlarm(){
  Serial.println("Alarm: - turn lights off");    
}

void EveningAlarm(){
  Serial.println("Alarm: - turn lights on");           
}

void WeeklyAlarm(){
  Serial.println("Alarm: - its Monday Morning");      
}

void ExplicitAlarm(){
  Serial.println("Alarm: - this triggers only at the given date and time");       
}

void Repeats(){
  Serial.println("15 second timer");         
}

void OnceOnly(){
  Serial.println("This timer only triggers once");  
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
#define TIME_HEADER  "T"   // Header tag for serial time sync message

unsigned long processSyncMessage() {
	unsigned long pctime = 0L;
	const unsigned long DEFAULT_TIME = 1486072960; // Feb 2 2017 

	if(Serial.find(TIME_HEADER)) {
		pctime = Serial.parseInt();
		return pctime;
		if( pctime < DEFAULT_TIME) { // check the value is a valid time (greater than Jan 1 2013)
			pctime = 0L; // return 0 to indicate that the time is not valid
		}
	}
	return pctime;
}