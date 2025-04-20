/*******************************************************
This program was created by the CodeWizardAVR V4.03 
Automatic Program Generator
� Copyright 1998-2024 Pavel Haiduc, HP InfoTech S.R.L.
http://www.hpinfotech.ro

Project : Task 4. Project work
Version : 0.1
Date    : 17.04.2025
Author  : Rene Iliste 
Company : TTK University of Applied Sciences
Comments: 
This is a traffic light project made on an ATMEGA32A controller by Rene. There is connected 8 LEDs - traffic light, 1 speaker - for panic mode audio, 2 buttons - for input, 1 potentiometer - for volume, 1 contrast control - to change the LCD contrast.
If you press button 1, you can cross the road with the next cycle and the counter will count down to zero so that people can cross the road smoothly.
If you press the 2nd button, panic mode will be activated and all traffic lights will turn red and the buzzer will be activated for 10 sek.

Chip type               : ATmega32A
Program type            : Application
AVR Core Clock frequency: 8,000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 512
*******************************************************/

// I/O Registers definitions
#include <mega32a.h>
#include <stdbool.h>
#include <stdlib.h>

// Delay functions
#include <delay.h>

// Alphanumeric LCD functions
#include <alcd.h>

// Declare your global variables here
volatile unsigned long millis;
int i,j;
char ch[8];
bool buttonPressed = false;

#define LEFT_GREEN PORTD.0
#define LEFT_YELLOW PORTD.1
#define LEFT_RED PORTD.2
#define RIGHT_GREEN PORTD.3
#define RIGHT_YELLOW PORTD.4
#define RIGHT_RED PORTD.5
#define MID_GREEN PORTD.6
#define MID_RED PORTD.7

#define BUZZER PORTB.0
#define BUTTON_LEFT PORTB.1
#define BUTTON_RIGHT PORTB.2

//Delay function for consisten delays
void delay_ms_custom(unsigned long ms) {
    unsigned long start = millis;
    while ((millis - start) < ms);
}

//for 1sec 250hz buzzer
void buzzer(){
 for (j = 250; j >= 0; j--){  
 BUZZER = 1;
 delay_ms(2);
 BUZZER = 0;
 delay_ms(2);
 }
}

void panicMode (){
  RIGHT_GREEN = 0;
  LEFT_GREEN = 0;
  LEFT_YELLOW = 0;
  RIGHT_YELLOW = 0;
  RIGHT_RED = 1;
  LEFT_RED = 1;
  MID_GREEN = 0;
  MID_RED = 1;    
      for (i = 10; i >= 0; i--) {
            lcd_clear();
            lcd_puts("BANIC MODE: ");
            itoa(i, ch);
            lcd_puts(ch);
            buzzer();
            //delay_ms(1000);
      }
  lcd_clear();
  lcd_puts("No more Panic");         
      
}


// Timer 0 overflow interrupt service routine, not the best solutions but it works
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
// Place your code here
millis ++;
 if (PINB.1 == 0){
        delay_ms(50);//Debounce delay
        lcd_clear();
        lcd_puts("button pressed");
        buttonPressed = true;    
        }
  if (PINB.2 == 0){
        delay_ms(50);
        lcd_clear();
        lcd_puts("BANIC!");
        panicMode ();    
        }       
 
// Reinitialize Timer 0 value
TCNT0=0xF8;
}

// Timer 0 output compare interrupt service routine
interrupt [TIM0_COMP] void timer0_comp_isr(void)
{
// Place your code here
}






//blinking block
void blinking(int pin) {
    int i;
    for (i = 0; i < 3; i++) {
        switch(pin) {
            case 0: PORTD.0 = 0; delay_ms_custom(500); PORTD.0 = 1; break;
            case 1: PORTD.1 = 0; delay_ms_custom(500); PORTD.1 = 1; break;
            case 2: PORTD.2 = 0; delay_ms_custom(500); PORTD.2 = 1; break;
            case 3: PORTD.3 = 0; delay_ms_custom(500); PORTD.3 = 1; break;
            case 4: PORTD.4 = 0; delay_ms_custom(500); PORTD.4 = 1; break;
            case 5: PORTD.5 = 0; delay_ms_custom(500); PORTD.5 = 1; break;
            case 6: PORTD.6 = 0; delay_ms_custom(500); PORTD.6 = 1; break;
            case 7: PORTD.7 = 0; delay_ms_custom(500); PORTD.7 = 1; break;
        }
        delay_ms_custom(250);
    }
}
//this is here just for DRY princible
void orangeCycle(){
      RIGHT_GREEN = 0;
      RIGHT_RED = 0;
      LEFT_RED = 0;
      LEFT_GREEN = 0;
      LEFT_YELLOW = 1;
      RIGHT_YELLOW = 1;
      delay_ms_custom(2000);
      LEFT_YELLOW = 0;
      RIGHT_YELLOW = 0;
}


//When pedestrian button is pushed, then with next traffic light change button value is read 
void buttonCycle (){
  lcd_clear();
  if(RIGHT_GREEN == 1) {RIGHT_YELLOW = 1; RIGHT_GREEN = 0;};
  if(LEFT_GREEN == 1) {LEFT_YELLOW = 1; LEFT_GREEN = 0;};
  delay_ms_custom(2000);
  RIGHT_GREEN = 0;
  LEFT_GREEN = 0;
  LEFT_YELLOW = 0;
  RIGHT_YELLOW = 0;
  RIGHT_RED = 1;
  LEFT_RED = 1;
  MID_GREEN = 1;
  MID_RED = 0;
  for (i = 5; i >= 0; i--) {
            lcd_clear();
            lcd_puts("Time to walk: ");
            itoa(i, ch);
            lcd_puts(ch);
            delay_ms_custom(1000);
        }
  blinking(6);
  MID_RED = 1;
  MID_GREEN = 0;
  lcd_clear();
  lcd_puts("Press button");
  delay_ms_custom(1000); 
  buttonPressed = false;
}

//Traffic light in normal mode: left and right side
void trafficLightCycle (){
      
      if(buttonPressed){
        buttonCycle ();
      }
      orangeCycle(); 
      lcd_clear();
      lcd_puts("Press button"); 
      MID_GREEN = 0;
      MID_RED = 1;
      RIGHT_GREEN = 0;
      LEFT_RED = 0;
      LEFT_GREEN = 1;
      RIGHT_RED = 1;
      delay_ms_custom(5000);
      blinking(0);
      
      
      
      if(buttonPressed){
        buttonCycle ();
      }
      orangeCycle();   
      LEFT_GREEN = 0;
      RIGHT_RED = 0;
      RIGHT_GREEN = 1;
      LEFT_RED = 1;
      delay_ms_custom(5000);
      blinking(3);
      
      
}

// External Interrupt 0 service routine, here is multiple interupts conflict, comment out
//interrupt [EXT_INT0] void ext_int0_isr(void)
//{
//// Place your code here
//        if (PINB.1 == 0){
//        delay_ms_custom(50);
//        lcd_clear();
//        lcd_puts("button pressed");
//        buttonPressed = true;    
//        }
//        if (PINB.1 == 1) {
//        MID_GREEN = 0;
//        }
//}

// External Interrupt 1 service routine
//interrupt [EXT_INT1] void ext_int1_isr(void)
//{
//// Place your code here
//
//}

// ADC Voltage Reference: AREF pin
#define ADC_VREF_TYPE ((0<<REFS1) | (0<<REFS0) | (0<<ADLAR))

// Read the AD conversion result
// Read Voltage=read_adc*(Vref/1024.0)
unsigned int read_adc(unsigned char adc_input)
{
ADMUX=adc_input | ADC_VREF_TYPE;
// Delay needed for the stabilization of the ADC input voltage
delay_us(10);
// Start the AD conversion
ADCSRA|=(1<<ADSC);
// Wait for the AD conversion to complete
while ((ADCSRA & (1<<ADIF))==0);
ADCSRA|=(1<<ADIF);
return ADCW;
}

void main(void)
{
// Declare your local variables here
// Input/Output Ports initialization
// Port A initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRA=(0<<DDA7) | (0<<DDA6) | (0<<DDA5) | (0<<DDA4) | (0<<DDA3) | (0<<DDA2) | (0<<DDA1) | (0<<DDA0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTA=(0<<PORTA7) | (0<<PORTA6) | (0<<PORTA5) | (0<<PORTA4) | (0<<PORTA3) | (0<<PORTA2) | (0<<PORTA1) | (0<<PORTA0);

// Port B initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=Out 
DDRB=(0<<DDB7) | (0<<DDB6) | (0<<DDB5) | (0<<DDB4) | (0<<DDB3) | (0<<DDB2) | (0<<DDB1) | (1<<DDB0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=P Bit1=P Bit0=0 
PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (1<<PORTB2) | (1<<PORTB1) | (0<<PORTB0);

// Port C initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRC=(0<<DDC7) | (0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTC=(0<<PORTC7) | (0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

// Port D initialization
// Function: Bit7=Out Bit6=Out Bit5=Out Bit4=Out Bit3=Out Bit2=Out Bit1=Out Bit0=Out 
DDRD=(1<<DDD7) | (1<<DDD6) | (1<<DDD5) | (1<<DDD4) | (1<<DDD3) | (1<<DDD2) | (1<<DDD1) | (1<<DDD0);
// State: Bit7=0 Bit6=0 Bit5=0 Bit4=0 Bit3=0 Bit2=0 Bit1=0 Bit0=0 
PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 7,813 kHz
// Mode: Normal top=0xFF
// OC0 output: Disconnected
// Timer Period: 1,024 ms
TCCR0=(0<<WGM00) | (0<<COM01) | (0<<COM00) | (0<<WGM01) | (1<<CS02) | (0<<CS01) | (1<<CS00);
TCNT0=0xF8;
OCR0=0x00;


// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=(0<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (1<<OCIE0) | (1<<TOIE0);

// External Interrupt(s) initialization
// INT0: On
// INT0 Mode: Low level
// INT1: On
// INT1 Mode: Low level
// INT2: Off
//GICR|=(1<<INT1) | (1<<INT0) | (0<<INT2);
//MCUCR=(0<<ISC11) | (0<<ISC10) | (0<<ISC01) | (0<<ISC00); //changed falling edge, before (0<<ISC01)
//MCUCSR=(0<<ISC2);
//GIFR=(1<<INTF1) | (1<<INTF0) | (0<<INTF2);


// Alphanumeric LCD initialization
// Connections are specified in the
// Project|Configure|C Compiler|Libraries|Alphanumeric LCD menu:
// RS: PORTC Bit 0
// RD: PORTC Bit 1
// EN: PORTC Bit 2
// D4: PORTC Bit 4
// D5: PORTC Bit 5
// D6: PORTC Bit 6
// D7: PORTC Bit 7
// Characters/line: 16
lcd_init(16);
lcd_clear();

// Globally enable interrupts
#asm("sei");

while (1)
  { 
  // Place your code here   
  trafficLightCycle();
  }
}
