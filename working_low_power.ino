#include<avr/io.h>

#define GREEN_LED_PIN PB0
#define RED_LED_PIN PB1
#define LED_ENABLED_TIME 10000
#define BREAK_TIME 2500

volatile bool enabled = false;

void setup_leds() {
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
}

void setup_external_interrupt(){
  sei();                 // Enabling global interrupt
  ADCSRA &= ~(1<<ADEN);  // Disable ADC ~200uA
  GIMSK |= (1<<PCIE);    // Pin change interrupt enable
  PCMSK |= (1<<PCINT2);  // Pin change interrupt to 2nd pin PB2
}


// Set unused pins to pullup
void setup_unused_pins() {
  pinMode(PB3, INPUT_PULLUP);
  pinMode(PB4, INPUT_PULLUP);
  pinMode(PB5, INPUT_PULLUP);
}

// Enter Sleep Mode
void enters_sleep()
{  
  MCUCR |= (1<<SM1);      // Enabling sleep mode and powerdown sleep mode
  MCUCR |= (1<<SE);       // Enabling sleep enable bit
  __asm__ __volatile__ ( "sleep" "\n\t" :: ); // Sleep instruction to put controller to sleep
}

// Interrupt Service Routine
ISR (PCINT0_vect)       
{
  MCUCR &= ~(1<<SE);      // Disabling sleep mode inside interrupt routine
}

void setup() {
  setup_leds();
  setup_external_interrupt();      // configuration for external interrupt
  setup_unused_pins(); 
}

void switch_led(bool enabled) {
  if (enabled) {
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(RED_LED_PIN, LOW);
  } else {
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);
  }
}

void switch_leds_off() {
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
}

void loop() {
  enabled = !enabled; 

  // Switch led
  switch_led(enabled);
  delay(LED_ENABLED_TIME);

  // Switch leds off
  switch_leds_off();   
  delay(BREAK_TIME);   // Break to turn leds off

  // Enter sleep mode 
  enters_sleep();                  
}
