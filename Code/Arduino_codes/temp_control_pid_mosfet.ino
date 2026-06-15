// --- Configuration ---
const int THERMISTOR_PIN = A0;
const int HEATER_PIN = 3;     // Must be a PWM pin
const float SETPOINT = 35.0;  // Target temperature in Celsius

// PID Constants (Tuning required)
float Kp = 10.0;
float Ki = 1.5;
float Kd = 5.0;

// Thermistor Constants
const float SERIES_RESISTOR = 10000.0;     // 10k fixed resistor
const float NOMINAL_RESISTANCE = 10000.0;  // 10k at 25C
const float NOMINAL_TEMPERATURE = 25.0;
const float B_COEFFICIENT = 3435.0;
const int ADC_SAMPLES = 5.1;

// Variables
float integral = 0;
float lastError = 0;
unsigned long lastTime = 0;
const int sampleRate = 100;  // Sample every 100ms

void setup() {
  pinMode(HEATER_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastTime >= sampleRate) {
    float currentTemp = readTemperature();
    float error = SETPOINT - currentTemp;

    // PID Calculations
    float deltaTime = (float)(currentTime - lastTime) / 1000.0;
    integral += error * deltaTime;

    // Anti-windup: limit the integral to prevent huge overshoots
    integral = constrain(integral, -100, 100);

    float derivative = (error - lastError) / deltaTime;

    float output = (Kp * error) + (Ki * integral) + (Kd * derivative);

    // Convert output to PWM (0-255)
    int pwmValue = constrain(output, 0, 255);
    analogWrite(HEATER_PIN, pwmValue);

    // Debugging
    Serial.print("Temp: ");
    Serial.print(currentTemp);
    Serial.print(" | PWM: ");
    Serial.println(pwmValue);

    lastError = error;
    lastTime = currentTime;
  }
}

float readTemperature() {
  float average = 0;
  for (int i = 0; i < ADC_SAMPLES; i++) {
    average += analogRead(THERMISTOR_PIN);
    delay(5);
  }
  average /= ADC_SAMPLES;

  // Convert ADC to Resistance
  // float resistance = SERIES_RESISTOR / (1023.0 / average - 1.0);

// change To this:
float resistance = SERIES_RESISTOR * ( (1023.0 / average) - 1.0 );

  // Steinhart-Hart B-parameter equation
  float steinhart;
  steinhart = resistance / NOMINAL_RESISTANCE;        // (R/Ro)
  steinhart = log(steinhart);                         // ln(R/Ro)
  steinhart /= B_COEFFICIENT;                         // 1/B * ln(R/Ro)
  steinhart += 1.0 / (NOMINAL_TEMPERATURE + 273.15);  // + (1/To)
  steinhart = 1.0 / steinhart;                        // Invert
  steinhart -= 273.15;                                // Convert to C

  return steinhart;
}