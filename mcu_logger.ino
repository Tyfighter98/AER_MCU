int inputPinArray[] = {13,12,11,10,9,8};
int inputPinValues[] = {0,0,0,0,0,0};
unsigned long t;
unsigned long dt;
unsigned long elapsed;
unsigned long total = 0; 
int cnt = 0;
volatile bool isrFlag = false;
void setup() {

  // TIMER 2 for interrupt frequency 1000 Hz:
  cli(); // stop interrupts
  TCCR2A = 0; // set entire TCCR2A register to 0
  TCCR2B = 0; // same for TCCR2B
  TCNT2  = 0; // initialize counter value to 0
  // set compare match register for 1000 Hz increments
  OCR2A = 249; // = 16000000 / (64 * 1000) - 1 (must be <256)
  // turn on CTC mode
  TCCR2B |= (1 << WGM21);
  // Set CS22, CS21 and CS20 bits for 64 prescaler
  TCCR2B |= (1 << CS22) | (0 << CS21) | (0 << CS20);
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);
  sei(); // allow interrupts

  t = 0;
  dt = 0;
  cnt = 0;

  Serial.begin(115200);

  for(int j=13;j > 7;j--){
    pinMode(inputPinArray[j],INPUT);
  }
}

// Interrupt function to sample signals
ISR(TIMER2_COMPA_vect) {
  cnt++;
  if (cnt > 40) {
    for(int j=0;j < 6;j++){
      inputPinValues[j] = digitalRead(inputPinArray[j]);
    }
    isrFlag=true; 
  }
}



void loop() {
  // Check if the ISR has run and flagged that the inputs have been run
  if(isrFlag==true){
    cnt = 0;
    // Calculate the elapsed time in miliseconds
    dt = micros();
    elapsed += (dt - t) / 1000;
    total += elapsed;

    // Begins writing log data over serial interface
    Serial.print(elapsed);
    Serial.print(",");
    // Loop through the first 5 input and comma seperate values for csv
    for(int j=0;j < 5;j++){
      Serial.print(inputPinValues[j]);
      Serial.print(",");
    }
    // print final input to serial
    Serial.print(inputPinValues[5]);
    Serial.println();
    // reset flag and timer
    isrFlag = false;
    t = micros();
  }
}
