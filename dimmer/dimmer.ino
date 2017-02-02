#include <Time.h>
#include <TimeLib.h>
#include <TimeAlarms.h>


const unsigned char channel_1 = 6; // Output to Opto Triac pin, channel 1
const unsigned char channel_2 = 5; // Output to Opto Triac pin, channel 2
volatile unsigned char duty_1 = 128;
volatile unsigned char duty_2 = 128;
volatile unsigned long t_1 = 0;
volatile unsigned long t_2 = 0;


volatile unsigned long cps = 120;
volatile unsigned long zero_count = 0;
volatile unsigned long period;
void zero_cross() {
  zero_count++;

  
  static unsigned long last_time = 0;
  unsigned long this_time = micros();
  if(this_time - last_time < 100) return;
  period = this_time - last_time;
  last_time = this_time;


}


void setup() {
  attachInterrupt(1, zero_cross, RISING);
  pinMode(channel_1, OUTPUT);
  digitalWrite(channel_1, LOW);
  pinMode(channel_2, OUTPUT);
  digitalWrite(channel_2, LOW);
  
  Serial.begin(9600);
  delay(200);
  
  uint8_t SaveSREG = SREG;   // save interrupt flag
  cli();   // disable interrupts
  unsigned long p = period;
  SREG = SaveSREG;   // restore the interrupt flag
  
  if(p < 9090 ){
    cps = 120;
  } else {
    cps = 100;
  }
  
  Serial.print("doing ");
  Serial.print(cps);
  Serial.println(" counts per second");
  
  
}




void loop() {
  uint8_t SaveSREG = SREG;   // save interrupt flag
  cli();   // disable interrupts
  unsigned long p = period;
  SREG = SaveSREG;   // restore the interrupt flag

  
  Serial.print("Last period: ");
  Serial.print(p);
  Serial.println(" microseconds");

  delay(1000);

}
