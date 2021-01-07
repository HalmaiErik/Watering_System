#include <SoftwareSerial.h>

SoftwareSerial s(5,6);  // RX, TX
const int pumpPin = 7;

char buff[10];
int pos = 0;

int irrHr = -1;
int irrMin = -1;
int irrSec = -1;

volatile int secs;
volatile int mins;
volatile int hrs;

void setup() {
  Serial.begin(9600);
  s.begin(9600);
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW);

  // Timer
  secs = 0;
  mins = 0;
  hrs = 0;
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = 15624;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS10);
  TCCR1B |= (1 << CS12);
  TIMSK1 |= (1 << OCIE1A);
  sei();
}

void loop() {
  if(s.available() > 0) {
    char inChar = (char) s.read();
    if (inChar != '_') {
       buff[pos++] = inChar;
       //Serial.println(inChar);
    }
    else {
      sscanf(buff, "%d:%d:%d", &irrHr, &irrMin, &irrSec);
      Serial.print(irrHr);
      Serial.print(":");
      Serial.print(irrMin);
      Serial.print(":");
      Serial.print(irrSec);
      Serial.println();
      pos = 0;
      secs = 0;
      mins = 0;
      hrs = 0;   
    }
  }

  if (irrHr != -1 && irrMin != -1 && irrSec != -1) {
    if (hrs == irrHr && mins == irrMin && secs == irrSec) {
      Serial.print(hrs);
      Serial.print(":");
      Serial.print(mins);
      Serial.print(":");
      Serial.print(secs);
      Serial.println();
      digitalWrite(pumpPin, LOW);
      delay(5000);

      secs = 0;
      mins = 0;
      hrs = 0;
    }  
  }
  digitalWrite(pumpPin, HIGH);
}

ISR(TIMER1_COMPA_vect)
{
  secs = secs + 1;
  
  if (secs == 60) {
    mins++;
    secs = 0;
  }
  if (mins == 60) {
    hrs++;
    mins = 0;
  }
}
