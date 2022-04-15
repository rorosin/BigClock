//////////////////Settings/////////////////////

#define USE_RTC 0 // use RTC clock or no

//////////////////Includes/////////////////////

// #include <RusDuino7Seg.h>
#include <EncButton.h>

#if (USE_RTC == 1)
#include <microDS3231.h>
#include <buildTime.h>
#endif

//////////////////RTC handle///////////////////

#if (USE_RTC == 1)
MicroDS3231 rtc;
#endif

//////////////////Time count///////////////////

uint8_t time_array[4] = {
	0, // tens of hours
	0, // hours
	0, // tens of minutes
	0  // minutes
};

uint8_t sekund = 0;
uint8_t minut = 0;
uint8_t chasov = 0;

///////////7-segments indictor/////////////////

int last_indicator; // number of last used indicator

// SevSeg seg(10, 11, 12, false);

byte symbols[10] = {
	B01111110,    // 0
	B00110000,    // 1
	B01101101,    // 2
	B01111001,    // 3
	B00110011,    // 4
	B01011011,    // 5
	B01011111,    // 6
	B01110000,    // 7
	B01111111,    // 8
	B01111011     // 9
};

int anode_pins[4] = {
	6, // 1
	7, // 2
	8, // 3
	9  // 4
};

///////////////Buttons/////////////////////////

#define RST_BTN_PIN 3
EncButton<EB_TICK, RST_BTN_PIN> rstBtn;

///////////////Other///////////////////////////

uint32_t dyn_tmr;        // dynamic indication timer
uint32_t timecount_tmr;  // time count timer

#if (USE_RTC == 1)
uint32_t rsttime_tmr;    // time reset timer
#endif

///////////////Setup///////////////////////////

void setup()
{
	Serial.begin(9600);

	Serial.println("BigClock is starting!");
#if (USE_RTC == 1)
	if(!rtc.begin()){
		Serial.println("RTC error!");
		for(;;);
	} else {
		Serial.println("RTC working!");
	}
	// rtc.setTime(BUILD_SEC, BUILD_MIN, BUILD_HOUR, BUILD_DAY, BUILD_MONTH, BUILD_YEAR);
	rtc.setTime(COMPILE_TIME);
#endif

	Serial.println("BigClock started!");

	for (int i = 0; i < 4; ++i) {
		pinMode(anode_pins[i], OUTPUT);
	}
}

//////////////Loop/////////////////////////////

void loop()
{
	rstBtn.tick();
	if (millis() - dyn_tmr >= 5) {
		dyn_tmr = millis(); // reset tmr

		if (last_indicator == 4) last_indicator = 0;
		else last_indicator++;

		byte symbol_byte = symbols[time_array[last_indicator]];
		setDigit(last_indicator, symbol_byte);
	}

	if (millis() - timecount_tmr >= 1000) {
		timecount_tmr = millis(); // reset tmr

		sekund++;
		if (sekund > 59) {
			sekund = 0;
			minut++;
		}
		if (minut > 59) {
			minut = 0;
			chasov++;
		}
		if (chasov > 23) {
			chasov = 0;
			minut = 0;
			sekund = 0;
		}
		time_array[4] = minut % 10;
		time_array[3] = minut / 10;
		time_array[2] = chasov % 10;
		time_array[1] = chasov / 10;
	}

#if (USE_RTC == 1)
	if (millis() - rsttime_tmr >= 3600000 or rstBtn.click()) {
		rsttime_tmr = millis(); // reset tmr
		resetTime();
	}
#endif
}

/////////////Set Digit//////////////////////////

void setDigit(int indicator, byte digit)
{
	for (int i = 0; i < 4; ++i) {
		digitalWrite(anode_pins[i], LOW);
	}
	// seg.sendByte(digit);

	digitalWrite(11, LOW);
	shiftOut(10, 12, LSBFIRST, digit);
	digitalWrite(11, HIGH);

	digitalWrite(anode_pins[indicator], HIGH);
}

#if (USE_RTC == 1)
/////////////Reset Time////////////////////////

void resetTime() {
	// Serial.println("time reseted");
	seconds = rtc.getSeconds();
	minutes = rtc.getMinutes();
	hours = rtc.getHours();
}
#endif