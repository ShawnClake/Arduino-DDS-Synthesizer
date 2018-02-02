/* Simple Sinewave DDS Routine
 * Speedy version -- using bit-shifts in the accumulator.
 * See https://github.com/hexagon5un/barking_dogs_dds_demo 
 * Public Domain
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "dds_waveforms.h"
#include "scale16.h"

#define clear_bit(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define set_bit(sfr, bit)   (_SFR_BYTE(sfr) |= _BV(bit))

#define SAMPLE_RATE       24000UL 
#define NUM_VOICES        1
#define SPREAD            1
#define BPM               200


/* Init functions, defined at the bottom of the file */
static inline void setup_pwm_audio_timer(void);
static inline void setup_sample_timer(void);

struct DDS {
	uint16_t increment;
	uint16_t accumulator;
	const int8_t* sample;   /* pointer to beginning of sample in memory */
};
volatile struct DDS voices[NUM_VOICES];

ISR(TIMER1_COMPA_vect) {
	PORTB |= (1 << PB1); // debug, toggle pin PB1, Arduino D9
	int16_t total = 0;

	for (uint8_t i = 0; i < NUM_VOICES; i++) {
		total += (int8_t) pgm_read_byte_near(voices[i].sample + (voices[i].accumulator >> 8));
		voices[i].accumulator += voices[i].increment;
	}
	total = total / NUM_VOICES;
	OCR2A = total + 128; // add in offset to make it 0-255 rather than -128 to 127
	PORTB &= ~(1 << PB1); // debug, toggle pin
}

uint16_t scale[] = {NOTE_C0, NOTE_D0, NOTE_E0, NOTE_F0, NOTE_G0, NOTE_A0, NOTE_B0, NOTE_C1, NOTE_D1, NOTE_E1, NOTE_F1, NOTE_G1, NOTE_A1, NOTE_B1, NOTE_C2, NOTE_D2, NOTE_E2, NOTE_F2, NOTE_G2, NOTE_A2, NOTE_B2};

static uint8_t i=0;
static uint8_t waveform=3;

int useBPM = 120;

void my_delay_ms(int ms)
{
  while (0 < ms)
  {  
    _delay_ms(1);
    --ms;
  }
}

int getBPMQuaterDelay()
{
  int potRead = analogRead(2); 
  useBPM = scaleVal(potRead, 0, 1023, 60, 300);
  useBPM = BPM;
  return 1000 / (useBPM / 60);
}

double scaleVal(double valueIn, double baseMin, double baseMax, double limitMin, double limitMax) {
  return ((limitMax - limitMin) * (valueIn - baseMin) / (baseMax - baseMin)) + limitMin;
}

void setup() 
{
	setup_sample_timer();
	setup_pwm_audio_timer();
	set_bit(DDRB, PB1); // debugging 
	
	//voices[0].sample = Sine;
	//voices[1].sample = Tri3;
	//voices[2].sample = Saw3;
	//voices[3].sample = Square3;
   switch (waveform){
    case(0):
      voices[0].sample = Sine;
      //voices[1].sample = Sine;
      //voices[2].sample = Sine;
      //voices[3].sample = Sine;
      //waveform=1;
      break;
    case(1):
      voices[0].sample = Tri15;
      //voices[1].sample = Tri15;
      //voices[2].sample = Tri15;
      //voices[3].sample = Tri15;
      //waveform=2;
      break;
    case(2):
      voices[0].sample = Saw15;
      //voices[1].sample = Saw15;
      //voices[2].sample = Saw15;
      //voices[3].sample = Saw15;
      //waveform=3;
      break;
    case(3):
      voices[0].sample = Square15;
      voices[1].sample = Square15;
      voices[2].sample = Square15;
      voices[3].sample = Square15;
      //waveform=0;
      break;
    default:
      waveform = 0;
  }
}


void incrementer(uint16_t note)
{
voices[0].increment = note - (SPREAD / 3);
voices[1].increment = note + SPREAD;
voices[2].increment = note - SPREAD;
voices[3].increment = note + (SPREAD / 3);
  
}

void shh()
{
voices[0].increment = 0;
voices[1].increment = 0;
voices[2].increment = 0;
voices[3].increment = 0;
  
}

int quater(uint16_t note)
{
  voices[0].increment = note;
  int delayAmount = getBPMQuaterDelay();
  my_delay_ms(delayAmount);
}

int eigth(uint16_t note)
{
  voices[0].increment = note;
  int delayAmount = getBPMQuaterDelay() / 2;
  my_delay_ms(delayAmount);
}

int half(uint16_t note)
{
  voices[0].increment = note;
  int delayAmount = getBPMQuaterDelay() * 2;
  my_delay_ms(delayAmount);
}

int whole(uint16_t note)
{
  voices[0].increment = note;
  int delayAmount = getBPMQuaterDelay() * 4;
  my_delay_ms(delayAmount);
}


int stac = 10;
int quaterStac(uint16_t note)
{
  incrementer(note);
  int delayAmount = getBPMQuaterDelay() - stac;
  my_delay_ms(delayAmount);
  shh();
  my_delay_ms(stac);
}

int eigthStac(uint16_t note)
{
  incrementer(note);
  int delayAmount = getBPMQuaterDelay() / 2 - stac;
  my_delay_ms(delayAmount);
  shh();
  my_delay_ms(stac);
}

int halfStac(uint16_t note)
{
  incrementer(note);
  int delayAmount = getBPMQuaterDelay() * 2 - stac;
  my_delay_ms(delayAmount);
  shh();
  my_delay_ms(stac);
}

int wholeStac(uint16_t note)
{
  incrementer(note);
  int delayAmount = getBPMQuaterDelay() * 4 - stac;
  my_delay_ms(delayAmount);
  shh();
  my_delay_ms(stac);
}
 
void loop() 
{  


  
  /*eigth(scale[0]);
  eigth(scale[1]);
  eigth(scale[0]);
  eigth(scale[1]);
  quater(scale[0]);
  quater(scale[1]);
  half(scale[0]);
  eigth(scale[1]);
  eigth(scale[0]);
  eigth(scale[1]);
  eigth(scale[0]);
  whole(scale[1]);*/

  quaterStac(scale[2]);
  quaterStac(scale[1]);
  quaterStac(scale[0]);
  quaterStac(scale[1]);
  quaterStac(scale[2]);
  quaterStac(scale[2]);
  halfStac(scale[2]);

  quaterStac(scale[1]);
  quaterStac(scale[1]);
  halfStac(scale[1]);

  quaterStac(scale[2]);
  quaterStac(scale[4]);
  halfStac(scale[4]);

  quaterStac(scale[2]);
  quaterStac(scale[1]);
  quaterStac(scale[0]);
  quaterStac(scale[1]);
  quaterStac(scale[2]);
  quaterStac(scale[2]);
  quaterStac(scale[2]);
  quaterStac(scale[2]);

  quaterStac(scale[1]);
  quaterStac(scale[1]);
  quaterStac(scale[2]);
  quaterStac(scale[1]);
  wholeStac(scale[0]);
  
  whole(0);

  quater(256);
  quater(200);
  quater(150);
  quater(100);
  quater(75);
  quater(50);
  quater(25);
  quater(20);

  whole(0);

	////voices[0].increment = scale[i];
	//voices[1].increment = scale[i];
	//voices[2].increment = scale[i];
	//voices[3].increment = scale[i];
 
	////_delay_ms(100);
	////i++;
  ////if(i==8)
  ////  i = 0;
	/*if (i == 8){
		_delay_ms(100);
		i = 0;
		switch (waveform){
		case(0):
			voices[0].sample = Sine;
			voices[1].sample = Sine;
			voices[2].sample = Sine;
			voices[3].sample = Sine;
			waveform=1;
			break;
		case(1):
			voices[0].sample = Tri15;
			voices[1].sample = Tri15;
			voices[2].sample = Tri15;
			voices[3].sample = Tri15;
			waveform=2;
			break;
		case(2):
			voices[0].sample = Saw15;
			voices[1].sample = Saw15;
			voices[2].sample = Saw15;
			voices[3].sample = Saw15;
			waveform=3;
			break;
		case(3):
			voices[0].sample = Square15;
			voices[1].sample = Square15;
			voices[2].sample = Square15;
			voices[3].sample = Square15;
			waveform=0;
			break;
		default:
			waveform = 0;
		}

	}*/
}




static inline void setup_sample_timer(){
	// Undo Arduino defaults:
	TCCR1A = 0;
	TCCR1B = 0; 
	TIMSK1 = 0;
	// Set up in count-till-clear mode 
	set_bit(TCCR1B, WGM12); 
	// Sync to CPU clock: fast
	set_bit(TCCR1B, CS10);
	// Enable output-compare interrupt
	set_bit(TIMSK1, OCIE1A);   
	OCR1A = F_CPU / SAMPLE_RATE - 1; 
	sei();
}

static inline void setup_pwm_audio_timer(){
	// Undo Arduino defaults:
	TCCR2A = 0;
	TCCR2B = 0; 
	// Set fast PWM mode
	TCCR2A |= _BV(WGM21) | _BV(WGM20); 
	// With output on OC2A / Arduino pin 11
	set_bit(TCCR2A, COM2A1);
	// Set fastest clock speed: ~62kHz @ 16MHz
	set_bit(TCCR2B, CS20);
	// Signal is symmetric around 127
	OCR2A = 127;
	// output on pin 11 -- OC2A 
	set_bit(DDRB, PB3);  /* or pinMode(11, OUTPUT) in Arduinese */
}

