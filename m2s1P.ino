#include <Arduino.h>

const byte LED_PIN = 13;
const byte METER_PIN = A4;
volatile bool blinkState = LOW;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(METER_PIN, INPUT);
  Serial.begin(9600);
  initializeTimer(2.0); // Initialize the timer with a default frequency of 2 seconds
}

void loop() {
  double potValue = analogRead(METER_PIN); // Read potentiometer value
  double timerFrequency = mapToFrequency(potValue); // Map potentiometer value to frequency
  configureTimer(timerFrequency); // Configure timer with updated frequency
}

void initializeTimer(double defaultFrequency) {
  noInterrupts(); // Disable interrupts
  
  // Timer1 setup
  TCCR1A = 0; // Reset Timer1 control registers
  TCCR1B = 0;
  TCNT1 = 0; // Reset Timer1 counter
  
  // Set default compare match value (OCR1A) for default frequency
  unsigned long compareMatchValue = calculateCompareMatchValue(defaultFrequency);
  OCR1A = compareMatchValue;
  
  // Set Timer1 control register B (prescaler 1024)
  TCCR1B |= (1 << WGM12); // CTC mode
  TCCR1B |= (1 << CS12) | (1 << CS10); // 1024 prescaler
  
  // Enable Timer1 compare match A interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  interrupts(); // Enable interrupts
}

double mapToFrequency(double potValue) {
  return map(potValue, 0, 1023, 0.1, 10); // Map potentiometer value to frequency range
}

void configureTimer(double timerFrequency) {
  noInterrupts(); // Disable interrupts
  
  unsigned long compareMatchValue = calculateCompareMatchValue(timerFrequency); // Calculate OCR1A value
  OCR1A = compareMatchValue; // Set compare match value
  
  interrupts(); // Enable interrupts
}

unsigned long calculateCompareMatchValue(double timerFrequency) {
  return F_CPU / (2 * 1024 * timerFrequency) - 1; // Calculate OCR1A value based on frequency
}

ISR(TIMER1_COMPA_vect) {
  blinkState = !blinkState; // Toggle blink state
  digitalWrite(LED_PIN, blinkState); // Update LED state
}
