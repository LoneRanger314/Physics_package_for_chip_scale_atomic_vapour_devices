// This code is used to get the voltage reading to measure the VCSEL temperature using the source meter setup

float filteredVoltage = 0;
const float alpha = 0.02; // Smoothing factor (0.01 to 0.1)

void setup() {
  Serial.begin(115200);
  
  // This is the magic line for the R4 Minima
  analogReadResolution(14); 
}

void loop() {
  int rawADC = analogRead(A5);
  float currentVoltage = rawADC * (5.15 / 16383.0);
  
  // EMA Filter: 90% old data, 10% new data
  filteredVoltage = (alpha * currentVoltage) + ((1.0 - alpha) * filteredVoltage);
  
  Serial.println(filteredVoltage, 4);
  // Serial.println(currentVoltage, 4);
  delay(100); // Faster sampling helps the filter
} 