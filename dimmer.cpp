#include "dimmer.h"


void TriacDimmer::begin(){
	TCCR1A = 0;
	TCCR1B = _BV(ICNC1) | _BV(ICES1) | _BV(CS11);
	TIMSK1 = _BV(ICIE1);
}

void TriacDimmer::setBrightness(uint8_t pin, float x){
	if(pin == 9){
		float y = 1 - TriacDimmer::detail::interpolate(x,
			TriacDimmer::detail::brightness_lut,
			TriacDimmer::detail::phase_lut,
			TriacDimmer::detail::lut_length);

		TriacDimmer::detail::ch_A_up = (1-y) * ICR1;
		TriacDimmer::detail::ch_A_dn = TriacDimmer::detail::ch_A_up + TriacDimmer::detail::pulse_length;
	} else if(pin == 10){
		float y = 1 - TriacDimmer::detail::interpolate(x,
			TriacDimmer::detail::brightness_lut,
			TriacDimmer::detail::phase_lut,
			TriacDimmer::detail::lut_length);

		TriacDimmer::detail::ch_B_up = (1-y) * ICR1;
		TriacDimmer::detail::ch_B_dn = TriacDimmer::detail::ch_B_up + TriacDimmer::detail::pulse_length;
	}
	return 0;
}

float TriacDimmer::getCurrentBrightness(uint8_t pin){
	if(pin == 9){
		float y = 1 - (float) TriacDimmer::detail::ch_A_up / ICR1;
		return TriacDimmer::detail::interpolate(y,
			TriacDimmer::detail::phase_lut,
			TriacDimmer::detail::brightness_lut,
			TriacDimmer::detail::lut_length);
	} else if(pin == 10){
		float y = 1 - (float) TriacDimmer::detail::ch_B_up / ICR1;
		return TriacDimmer::detail::interpolate(y,
			TriacDimmer::detail::phase_lut,
			TriacDimmer::detail::brightness_lut,
			TriacDimmer::detail::lut_length);
	}
	return 0;
}

void TriacDimmer::end(){
	TCCR1A = 0;
	TCCR1B = 0;
	TIMSK1 = 0;
}

constexpr float TriacDimmer::detail::interpolate(const float x, const float[] x_table, const float[] y_table, uint8_t table_length){
	if (x <= _in[0]) return _out[0];
	if (x >= _in[size-1]) return _out[size-1];
	uint8_t pos = 1;
	while(x > _in[pos]) pos++;
	if (x == _in[pos]) return _out[pos];
	return (x - _in[pos-1]) * (_out[pos] - _out[pos-1]) / (_in[pos] - _in[pos-1]) + _out[pos-1];
}

TriacDimmer::detail::ISR(TIMER1_CAPT_vect){
	OCR1A = ch_A_up;
	OCR1B = ch_B_up;
	TCCR1A = _BV(COM1A0) | _BV(COM1A1) | _BV(COM1B0) | _BV(COM1B1);
	TIMSK1 = _BV(ICIE1) | _BV(OCIE1A) | _BV(OCIE1B);
	TCNT1 = TCNT1 - ICR1 + 1;
}
TriacDimmer::detail::ISR(TIMER1_COMPA_vect){
	TCCR1A &=~ _BV(COM1A1);
	TIMSK1 &=~ _BV(OCIE1A);
	OCR1A = ch_A_dn;
	if(TCNT1 - OCR1A >= 0){
		TCCR1C = _BV(FOC1A);
	}
}
TriacDimmer::detail::ISR(TIMER1_COMPB_vect){
	TCCR1B &=~ _BV(COM1B1);
	TIMSK1 &=~ _BV(OCIE1B);
	OCR1B = ch_B_dn;
	if(TCNT1 - OCR1B >= 0){
		TCCR1C = _BV(FOC1B);
	}
}

