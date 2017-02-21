#include <Wire.h>
#include <i2cdetect.h>
#include <extEEPROM.h> 



void hexprintint( int number, byte width ) {
	int currentMax = 0x10;
		for (byte i=1; i<width; i++){
			if (number < currentMax) {
				Serial.print("0");
			}
		currentMax *= 0x10;
		} 
	Serial.print(number, HEX);
}
void hexprintbyte( byte number, byte width ) {
	int currentMax = 0x10;
		for (byte i=1; i<width; i++){
			if (number < currentMax) {
				Serial.print("0");
			}
		currentMax *= 0x10;
		} 
	Serial.print(number, HEX);
}

void setup() {
	Wire.begin();
	Serial.begin(19200);

}



void loop() {
	if (Serial.available()) {
		processSyncMessage();
	}
	delay(100);
}

void scan(int low = 0x08, int high=0x80){
	Serial.print("Scanning address range 0x");
	Serial.print(low, HEX);
	Serial.print("-0x");
	Serial.print(high, HEX);
	Serial.print("\n");
	i2cdetect(low, high);
}
void dump_eeprom(int addr){
	extEEPROM eep(kbits_32, 1, 0x20, addr);

	byte buf[0x20];

	for(unsigned int i = 0; i < 0x8000; i += 0x20){
		eep.read(i, buf, 0x20);

		Serial.print("0x");
		hexprintint(i, 4);
		Serial.print(": ");
		for(unsigned int j = 0; j < 0x20; j++){
			hexprintbyte(buf[j], 2);
			Serial.print(" ");
		}
		Serial.print("\n");
	}
	Serial.print("\nok\n");
}

unsigned long parse_hex(String s){
	if(s.length() >= 2 && s[1] == 'x'){
		s.remove(0,2);
	}

	return strtol(s.c_str(), nullptr, 0x10);
}
/*  code to process time sync messages from the serial port   */
#define SCAN_HEADER 'S' //scan address range
#define READ_HEADER 'R' //read address
#define EEPROM_HEADER 'E' //dump eeprom



unsigned long processSyncMessage() {
	unsigned long t = 0L;
	const unsigned long DEFAULT_TIME = 1486072960; // Feb 2 2017

	switch(Serial.read()){
	case SCAN_HEADER:{
		String str = Serial.readString();
		str.trim();
		if(str == ""){
			scan();
		} else {
			int hy = str.indexOf('-');
			String first = str.substring(0,hy);
			String last = str.substring(hy+1);

			int low = parse_hex(first);
			int high = parse_hex(last);

			scan(low, high);
		}
	} break;
	case READ_HEADER:{
		String str = Serial.readString();
		str.trim();
		int co = str.indexOf(':');
		int hy = str.indexOf('-');
		String s = str.substring(0,co);
		int addr = parse_hex(s);
		s = str.substring(co+1, hy-co);
		int low = parse_hex(s);
		s= str.substring(hy+1);
		int high = parse_hex(s);
	} break;
	case EEPROM_HEADER:{
		String str = Serial.readStringUntil('\n');
		str.trim();
		// int co = str.indexOf(':');
		// int hy = str.indexOf('-');
		// if(co == -1 || hy == -1){
		// 	Serial.print("Malformed Arguments\n");
		// 	break;
		// }

		//String s = str.substring(0,co);
		int addr = parse_hex(str);
		// s = str.substring(co+1, hy);
		// unsigned int low = parse_hex(s);
		// s= str.substring(hy+1);
		// unsigned int high = parse_hex(s);

		dump_eeprom(addr);//, low, high);
		Serial.print(addr,HEX);
		Serial.print("\n");

	} break;
	default:
		break;
	}
}