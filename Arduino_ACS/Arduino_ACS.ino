int zero;

void setup() {
  Serial.begin(9600);
  calibrate();
}
void loop() {
  uint32_t period = 1000000 / 50;
  uint32_t t_start = micros();

  uint32_t Isum = 0;
  uint32_t measurements_count = 0;
  int32_t Inow;

  while (micros() - t_start < period) {
    Inow = analogRead(A0) - zero;
    Isum += Inow*Inow;
    measurements_count++;
  }

  float Irms = sqrt(Isum / measurements_count) / 1023 * 5.0 / 0.066;
  Serial.println(Irms);
  Serial.println();
  delay(1000);
}

void calibrate() {
  uint16_t acc = 0;
  for (int i = 0; i < 10; i++) {
    acc += analogRead(A0);
  }
  zero = acc / 10;
}
