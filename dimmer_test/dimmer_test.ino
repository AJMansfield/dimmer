unsigned char channel_1 = 6;    // Output to Opto Triac pin, channel 1
unsigned char channel_2 = 5;    // Output to Opto Triac pin, channel 2



void setup() {
  // put your setup code here, to run once:
  pinMode(channel_1, OUTPUT);// Set AC Load pin as output
  pinMode(channel_2, OUTPUT);// Set AC Load pin as output
  digitalWrite(channel_1, HIGH);
  digitalWrite(channel_2, HIGH);
}

void loop(){};
