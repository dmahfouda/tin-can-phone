int speakerPin = 5;
 
int numTones = 10;
int tones[] = {261, 277, 294, 311, 330, 349, 370, 392, 415, 440};
//            mid C  C#   D    D#   E    F    F#   G    G#   A
 
void setup()
{
  for (int i = 0; i < numTones; i++)
  {
    tone2(speakerPin, tones[i], 500);
    // delay(500);
  }
  noTone2(speakerPin);
  Serial.begin(9600);
  pinMode(speakerPin, OUTPUT);
}
 
void loop()
{
  // Serial.printf("%d\n",LED_BUILTIN);
  // digitalWrite(speakerPin, millis()%100<50);
	for (int i = 0; i < numTones; i++){
	    tone2(speakerPin, tones[i], 500);
	    delay(500);
	  }
}