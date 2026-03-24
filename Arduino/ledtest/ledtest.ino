const int pinR = 25; // Red channel pin
const int pinG = 26; // Green channel pin
const int pinB = 27; // Blue channel pin

// Note for ESP32 users: `analogWrite` is supported in ESP32 Core 3.0.0 and above.
// If you are on an older core, you might need to use `ledcAttachPin` and `ledcWrite`.

void setup() {
  pinMode(pinR, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinB, OUTPUT);
}

void setColor(int r, int g, int b) {
  // If your strip colors are inverted (0 is max brightness, 255 is off), 
  // you can change these to: analogWrite(pinR, 255 - r); etc.
  analogWrite(pinR, r);
  analogWrite(pinG, g);
  analogWrite(pinB, b);
}

void loop() {
  int delayTime = 10; // Speed of the transition

  // Red to Yellow
  for(int i = 0; i <= 255; i++) { 
    setColor(255, i, 0); 
    delay(delayTime); 
  }
  
  // Yellow to Green
  for(int i = 255; i >= 0; i--) { 
    setColor(i, 255, 0); 
    delay(delayTime); 
  }
  
  // Green to Cyan
  for(int i = 0; i <= 255; i++) { 
    setColor(0, 255, i); 
    delay(delayTime); 
  }
  
  // Cyan to Blue
  for(int i = 255; i >= 0; i--) { 
    setColor(0, i, 255); 
    delay(delayTime); 
  }
  
  // Blue to Magenta
  for(int i = 0; i <= 255; i++) { 
    setColor(i, 0, 255); 
    delay(delayTime); 
  }
  
  // Magenta to Red
  for(int i = 255; i >= 0; i--) { 
    setColor(255, 0, i); 
    delay(delayTime); 
  }
}
