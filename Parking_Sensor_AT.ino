#include <QuickStats.h>
#include  <FastLED.h>
QuickStats stats;
#define PIR_PIN 2 //PWM D2 for PIR motion sensor input
#define trigPin 12 //PWM D12
#define echoPin 11 //PWM D11
#define LED_PIN 10 //PWM D10
#define NUM_LEDS 15 //Total number of LEDS
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define STOP_DISTANCE 80 //In CENTIMETERS
#define START_DISTANCE 400 //400 CENTIMETERS is the max range of HC-SR04 ultrasonic sensor

/* Define Global variables */
CRGB leds[NUM_LEDS];
float duration;
float durationarray[15];
int distance;
int increment = ((START_DISTANCE - STOP_DISTANCE) / NUM_LEDS); // Same number of increments as the number of LEDS i.e.  For every 5 CM a LED should light up
bool PIR_motion_state = false;

void ledTest() {                                    // running red stripe.
  for (int i = 0; i < NUM_LEDS; i++ ) { 
    FastLED.clear();
    FastLED.show();
    FastLED.delay(10);
    leds[i] = CRGB::Blue;
    FastLED.show();
    FastLED.delay(100);
  }
  for (int i = NUM_LEDS; i >= 0 ; i--) {
    FastLED.clear();
    FastLED.show();
    FastLED.delay(10);
    leds[i] = CRGB(0, 128, 128); // RGB for Teal
    FastLED.show();
    FastLED.delay(100);
  }
} // animationA()
void setup() {
  delay(3000); //safe startup
  // Ultrasonic sensor HC-SR04 pin definition
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(PIR_PIN, INPUT);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 1000);
  FastLED.clear();
  FastLED.show();
  ledTest();
  FastLED.show();
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  Serial.begin(9600);
}

void loop() {

  if (digitalRead(PIR_PIN) == LOW) {
    if (PIR_motion_state == true) {
      Serial.println("Motion ended!");
      PIR_motion_state = false;
      FastLED.clear();
      FastLED.show();
    }
  }
  else {
    Serial.println("Motion started!");
    PIR_motion_state = true;
    for (int i = 0; i < 14; i++) {
      //Distance measurement with HR-S04
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);

      durationarray[i] = pulseIn(echoPin, HIGH);
      distance =  durationarray[i] * 0.034 / 2; //Distance in CENTIMETERS
    }
    duration = stats.median(durationarray, 15);
    distance = duration * 0.034 / 2;
    serialPrint(distance); //print distance measurement to serial port for debug
    displayController(distance);
    delay(60);
  }
}

void displayController(int dist) {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (dist <= STOP_DISTANCE) {
      for (int j = 0; j < 5; j++) {
        FastLED.clear();
        FastLED.show();
        FastLED.delay(100);
        fill_solid(leds, NUM_LEDS, CRGB::Red);
        FastLED.show();
      }
      break;
    }
    if (dist < (STOP_DISTANCE + (increment * i))) {
      FastLED.clear();
      fill_solid(leds,  (NUM_LEDS - (NUM_LEDS - i)), CRGB::Green);
      FastLED.show();
      FastLED.delay(60);
      break;
    }
  }
}

void serialPrint(int dist) {
  Serial.print("Distance:");
  Serial.print(distance);
  Serial.print("cm");
  Serial.print("[");
  Serial.print((duration / 148.0));
  Serial.println("inches]");
}
