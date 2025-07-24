#include <CapacitiveSensor.h>
#include <Pitches.h>

/*
 * TaterClavier
 * Andrew Smith and Erin Brandt 2025
 * Uses capacitive sensing to use potatoes as inputs to produce tones
 * Potentiometers control sensitivity of sensors and frequency of notes, essentially to allow key to change
 * Update July 12 2025
 */

CapacitiveSensor C4 = CapacitiveSensor(7, 2);  // 390k resistor between pins 3 & 2, pin 2 is sensor pin, add a wire and or foil
CapacitiveSensor CS4 = CapacitiveSensor(7, 3);
CapacitiveSensor D4 = CapacitiveSensor(7, 4);
CapacitiveSensor DS4 = CapacitiveSensor(14, 19);
CapacitiveSensor E4 = CapacitiveSensor(14, 18);
CapacitiveSensor F4 = CapacitiveSensor(14, 17);
CapacitiveSensor FS4 = CapacitiveSensor(14, 16);
CapacitiveSensor G4 = CapacitiveSensor(7, 6);
CapacitiveSensor GS4 = CapacitiveSensor(12, 8);
CapacitiveSensor AN4 = CapacitiveSensor(12, 9);
CapacitiveSensor AS4 = CapacitiveSensor(12, 10);
CapacitiveSensor B4 = CapacitiveSensor(12, 11);


int keys[][12]= {{NOTE_C3, NOTE_CS3, NOTE_D3, NOTE_DS3, NOTE_E3, NOTE_F3, NOTE_FS3, NOTE_G3, NOTE_GS3, NOTE_A3, NOTE_AS3, NOTE_B3},
                {NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4},
                {NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5},
                {NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6}
                };

const int switchPin = 15;

//set whether you want discrete key changes or continuous frequency change with knob turn
String mode;
int moderead;
int i;
long key;
int threshPot;
int capThreshold;
int keyPot;
int buzzerPin = 5;
int flags[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};                                        //used to indicate whether this potato was previously touched
//float freqs[] = { 261.626, 293.66, 329.63, 349.23, 392, 440, 493.88, 523.251 };  //frequency of notes: C4, D4, E4, F4, G4, A4, B4, C5
long reads[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};                                       //used to get reading from potato capacitive sensors
String modes[3] = {"wibble", "tone"};

float freq;      //frequency that will be played
float freqMult;  //multiplier for the frequency, will vary depending on the position of the tuning potentiometer
int keymem = 0;
int max;


void setup() {
  //Serial.begin(19200);
  pinMode(LED_BUILTIN, OUTPUT);
  
  C4.set_CS_AutocaL_Millis(0xFFFFFFFF);
  CS4.set_CS_AutocaL_Millis(0xFFFFFFFF);
  D4.set_CS_AutocaL_Millis(0xFFFFFFFF);
  DS4.set_CS_AutocaL_Millis(0xFFFFFFFF);
  E4.set_CS_AutocaL_Millis(0xFFFFFFFF);
  F4.set_CS_AutocaL_Millis(0xFFFFFFFF);
  FS4.set_CS_AutocaL_Millis(0xFFFFFFFF);
  G4.set_CS_AutocaL_Millis(0xFFFFFFFF);
  GS4.set_CS_AutocaL_Millis(0xFFFFFFFF);
  AN4.set_CS_AutocaL_Millis(0xFFFFFFFF);
  AS4.set_CS_AutocaL_Millis(0xFFFFFFFF);
  B4.set_CS_AutocaL_Millis(0xFFFFFFFF);

  pinMode(buzzerPin, OUTPUT);
  pinMode(switchPin, INPUT);
}

void loop() {
  readtaters();                     //look at the potatoes to see which ones are being touched
  for (i = 0; i < 12; i++) {
    readnontaters();
    mode = modes[moderead];
    //Serial.println(moderead);
    if ((capThreshold < reads[i]) && (flags[i] == 0)) {
      digitalWrite(LED_BUILTIN, HIGH); 
      if (mode == "tone") {
        getkeypot(0,3);
       // Serial.println(key);
        keymem = key;
        playtone();
      } else if (mode == "var") {
        playvar();
      } else if (mode == "wibble") {
        playwibble();
      }
    } else if ((capThreshold > reads[i]) && (flags[i] == 1)) {
      if (mode == "tone"){
    
      }
      for (int j = 0; j < 12; j++) {
        flags[j] = 0;
      }
    }
  }
  if (flags[0] + flags[1] + flags[2] + flags[3] + flags[4]+ flags[5] + flags[6] + flags[7] + flags[8] + flags[9] + flags[10] + flags[11]== 0) {
    if (mode == "tone"){
          getkeypot(0, 3);
          keymem = key;
    }
    noTone(buzzerPin);
    digitalWrite(LED_BUILTIN, LOW); 

  }
}
void readtaters() {
  //reads from all the potato sensors, from lowest to highest
  reads[0] = C4.capacitiveSensor(30);
  reads[1] = CS4.capacitiveSensor(30);
  reads[2] = D4.capacitiveSensor(30);
  reads[3] = DS4.capacitiveSensor(30);
  reads[4] = E4.capacitiveSensor(30);
  reads[5] = F4.capacitiveSensor(30);
  reads[6] = FS4.capacitiveSensor(30);
  reads[7] = G4.capacitiveSensor(30);
  reads[8] = GS4.capacitiveSensor(30);
  reads[9] = AN4.capacitiveSensor(30);
  reads[10] = AS4.capacitiveSensor(30);
  reads[11] = B4.capacitiveSensor(30);
  
}

void readnontaters() {
  threshPot = analogRead(A6);                       //reads current state of the threshold potentiometer            
  capThreshold = map(threshPot, 0, 1023, 500, 20);  //maps threshold potentiometer to a useful range. Tweak the last two numbers if the range needs to be adjusted
  moderead = digitalRead(switchPin);                //reads current state of the mode switch

}
void getkeypot(int min, int max){
  keyPot = analogRead(A7);
  key = map(keyPot, 0, 800, min, max);
}

void playtone() {
  //maps the tuning potentiometer to different octaves, of all 12 notes. Right now we have four.
  tone(buzzerPin, keys[key][i]);
  flags[i] = 1;
}
void playwibble() {
  getkeypot(5, 30);
  tone(buzzerPin, keys[keymem][i] + key, (70-key));
  delay(key);
  tone(buzzerPin, keys[keymem][i], (key));
  delay(key);
}
void playvar() {
  key = map(keyPot, 0, 1023, 0, 20);
  freq = keys[0][1] + (50 * key, 100);
  tone(buzzerPin, freq);
  flags[i] = 1;
}