/*
 * DRV300 Laser Driver Interface - Version 7.0
 * Features: 20s Ramp Up -> 5s Ramp Down (No Hold)
 * Manual Mode: Direct 12-bit mapping
 */

const int dacPin = A0;      
const int btnPin = 2;       
const int togglePin = 3;    
const int potPin = A1;      

// Constants
const int RESOLUTION = 12;           
const int MAX_DAC_VALUE = 1145;      // 1.25V limit
const unsigned long RAMP_UP_MS = 20000;
const unsigned long RAMP_DOWN_MS = 5000;

bool lastToggleState = HIGH;

void setup() {
  analogWriteResolution(RESOLUTION); 
  
  pinMode(btnPin, INPUT_PULLUP);
  pinMode(togglePin, INPUT_PULLUP);
  
  Serial.begin(115200);
  Serial.println("--- DRV300 System Ready (Continuous Sweep) ---");
}

void loop() {
  bool currentToggle = digitalRead(togglePin);

  // Mode Change Printing
  if (currentToggle != lastToggleState) {
    Serial.println(currentToggle == LOW ? "[MODE] MANUAL" : "[MODE] AUTO");
    lastToggleState = currentToggle;
  }

  if (currentToggle == LOW) {
    runManualControl();
  } else if (digitalRead(btnPin) == LOW) {
    runSweepSequence();
  } else {
    analogWrite(dacPin, 0); 
  }
}

// Manual Control: Original direct mapping logic
void runManualControl() {
  analogWrite(dacPin, analogRead(potPin)); 
}

void runSweepSequence() {
  Serial.println(">>> SWEEP STARTING");
  
  unsigned long startTime = millis();
  unsigned long elapsed = 0;

  // 1. Ramp Up (20 Seconds)
  while (elapsed < RAMP_UP_MS) {
    elapsed = millis() - startTime;
    if (elapsed > RAMP_UP_MS) elapsed = RAMP_UP_MS; 
    
    int val = (elapsed * (unsigned long)MAX_DAC_VALUE) / RAMP_UP_MS;
    analogWrite(dacPin, val);
  }
  
  // 2. Immediate Ramp Down (5 Seconds)
  // We reset the startTime immediately to transition without a pause
  startTime = millis();
  elapsed = 0;
  
  while (elapsed < RAMP_DOWN_MS) {
    elapsed = millis() - startTime;
    if (elapsed > RAMP_DOWN_MS) elapsed = RAMP_DOWN_MS;

    int val = MAX_DAC_VALUE - ((elapsed * (unsigned long)MAX_DAC_VALUE) / RAMP_DOWN_MS);
    analogWrite(dacPin, val);
  }

  // Ensure fully off at end of sweep
  analogWrite(dacPin, 0);
  Serial.println("<<< SWEEP COMPLETE");
}