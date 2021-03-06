// ***************************************************************************
// TV mode
// ***************************************************************************

uint8_t  dipInterval = 10;
uint16_t darkTime = 250;
unsigned long currentDipTime;
unsigned long dipStartTime;
unsigned long currentMillis;
uint8_t  ledState = LOW;
long     previousMillis = 0; 
uint16_t led = 5;
uint16_t interv = 2000;
uint8_t  twitch = 50;
uint8_t  dipCount = 0;
uint8_t  analogLevel = 100;
boolean  timeToDip = false;


void hsb2rgbAN1(uint16_t index, uint8_t sat, uint8_t bright, uint8_t myled) {
  // Source: https://blog.adafruit.com/2012/03/14/constant-brightness-hsb-to-rgb-algorithm/
  uint8_t temp[5], n = (index >> 8) % 3;
  temp[0] = temp[3] = (uint8_t)((                                         (sat ^ 255)  * bright) / 255);
  temp[1] = temp[4] = (uint8_t)((((( (index & 255)        * sat) / 255) + (sat ^ 255)) * bright) / 255);
  temp[2] =           (uint8_t)(((((((index & 255) ^ 255) * sat) / 255) + (sat ^ 255)) * bright) / 255);
  strip->setPixelColor(myled, temp[n + 2], temp[n + 1], temp[n], 0);
}


void updateLed (uint16_t led, uint8_t brightness) {
  ledstates[led] = brightness;
  for (uint16_t i=0; i<WS2812FXStripSettings.stripSize; i++) {
    uint16_t index = (i%3 == 0) ? 400 : random(0,767);
    hsb2rgbAN1(index, 200, ledstates[i], i);
  }
  strip->show();
}


// See: http://forum.mysensors.org/topic/85/phoneytv-for-vera-is-here/13
void handleTV() {
  if (timeToDip == false) {
    currentMillis = millis();
    if(currentMillis-previousMillis > interv) {
      previousMillis = currentMillis;
      //interv = random(750,4001);//Adjusts the interval for more/less frequent random light changes
      interv = random(1000-(ws2812fx_speed*2),5001-(ws2812fx_speed*8));
      twitch = random(40,100);// Twitch provides motion effect but can be a bit much if too high
      dipCount = dipCount++;
    }
    if(currentMillis-previousMillis<twitch) {
      led=random(0, WS2812FXStripSettings.stripSize - 1);
      analogLevel=random(50,255);// set the range of the 3 pwm leds
      ledState = ledState == LOW ? HIGH: LOW; // if the LED is off turn it on and vice-versa: 
      updateLed(led, (ledState) ? 255 : 0);   
      if (dipCount > dipInterval) { 
        DBG_OUTPUT_PORT.println("dip");
        timeToDip = true;
        dipCount = 0;
        dipStartTime = millis();
        darkTime = random(50,150);
        dipInterval = random(5,250);// cycles of flicker
      }
    } 
  } else {
    DBG_OUTPUT_PORT.println("Dip Time");
    currentDipTime = millis();
    if (currentDipTime - dipStartTime < darkTime) {
      for (uint16_t i=3; i<WS2812FXStripSettings.stripSize; i++) {
        updateLed(i, 0);
      }
    } else {
      timeToDip = false;
    }
    //strip->show();
  }
}
