

#define MAINS_PERIOD 16667 //number of half-microseconds in a half-period of the mains waveform

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


float multiMap(float val, const float* _in, const float* _out, uint8_t size){
	// take care the value is within range
	// val = constrain(val, _in[0], _in[size-1]);
	if (val <= _in[0]) return _out[0];
	if (val >= _in[size-1]) return _out[size-1];

	// search right interval
	uint8_t pos = 1;  // _in[0] allready tested
	while(val > _in[pos]) pos++;

	// this will handle all exact "points" in the _in array
	if (val == _in[pos]) return _out[pos];

	// interpolate in the right segment for the rest
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
	for(float x = 0; x < 1; x += 0.01){
		set_ch_a(x);
		set_ch_b(1-x);
		delay(100);
	}
	for(float x = 0; x < 1; x += 0.01){
		set_ch_a(1-x);
		set_ch_b(x);
		delay(100);
	}
}
