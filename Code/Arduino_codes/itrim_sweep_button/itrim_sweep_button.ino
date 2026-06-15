/*
  Asymmetrical Laser Sweep Trigger
  - 8s Ramp Up (Thermal Equilibrium)
  - 1s Hold (Measurement Window)
  - 4s Ramp Down (Controlled Reset)
*/

const int dacPin = A0;
const int buttonPin = 2;
const int maxDacSteps = 2560; // 1.25V ceiling (assumes 2V hardware divider)

// Timing constants in microseconds
const unsigned long delayUp = 3125;   // 8 seconds total
const unsigned long delayDown = 1562; // 4 seconds total
const int holdTimeMs = 1000;          // 1 second hold

void setup() {
  analogWriteResolution(12);
  pinMode(buttonPin, INPUT_PULLUP);
  
  analogWrite(dacPin, 0); // Start at 0V
  
  Serial.begin(115200);
  Serial.println("System Ready. Waiting for button press...");
}

void loop() {
  if (digitalRead(buttonPin) == LOW) {
    Serial.println("Sweep Initiated: 8s Up | 1s Hold | 4s Down");
    
    executeAsymmetricalSweep();
    
    Serial.println("Sweep Sequence Complete.");
    
    // Wait for button release to avoid double-triggering
    while (digitalRead(buttonPin) == LOW) delay(10);
    delay(50); 
  }
}

void executeAsymmetricalSweep() {
  // 1. Ramp Up (8 Seconds)
  for (int i = 0; i <= maxDacSteps; i++) {
    analogWrite(dacPin, i);
    delayMicroseconds(delayUp);
  }

  // 2. Hold (1 Second)
  delay(holdTimeMs);

  // 3. Ramp Down (4 Seconds)
  for (int i = maxDacSteps; i >= 0; i--) {
    analogWrite(dacPin, i);
    delayMicroseconds(delayDown);
  }
}