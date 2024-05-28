#define BLYNK_TEMPLATE_ID "TMPL2lkOhPqkG"
#define BLYNK_TEMPLATE_NAME "Home Automation System"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

#define BLYNK_PRINT Serial

char auth[] = "PcpmEXdr2fL2sak7cp4zy6ZZ9wg8Fgar";
char ssid[] = "ELMELIGY"; // Enter your WIFI name
char pass[] = "you2552005";  // Enter your WIFI password

DHT dht(D4, DHT11); // DHT sensor on D4, DHT11 type

BlynkTimer timer;
bool pirbutton = 0;

#define Buzzer D2
#define MQ2 A0
#define flame D1
#define PIR D3
#define relay1 D7
#define relay2 D8

BLYNK_WRITE(V0) {
  pirbutton = param.asInt();
}

void updateBuzzer(bool state) {
  Serial.print("Buzzer State: ");
  Serial.println(state);
  digitalWrite(Buzzer, state ? HIGH : LOW);
}

void control_heater() {
  int raw_value = analogRead(MQ2);
  int value = map(raw_value, 0, 1024, 0, 100); // Map to 0-100 range
  Serial.print("Raw MQ2 value: ");
  Serial.println(raw_value);
  Serial.print("Mapped MQ2 value (0-100): ");
  Serial.println(value);
  Blynk.virtualWrite(V7, value);
  if (value >= 15) { // Trigger buzzer if gas value is greater than or equal to 54
    updateBuzzer(true);
  } else {
    updateBuzzer(false);
  }
}

void DHT11sensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  int h_mapped = map(h, 0, 100, 0, 100); // Map to 0-100 range
  int t_mapped = map(t, -40, 80, 0, 100); // Assuming temperature range -40 to 80°C

  Blynk.virtualWrite(V2, t_mapped);
  Blynk.virtualWrite(V3, h_mapped);
}

void flamesensor() {
  bool raw_value = digitalRead(flame);
  int value = raw_value ? 100 : 0; // Map to 0 or 100
  Serial.print("Flame sensor value (0-100): ");
  Serial.println(value);
  Blynk.virtualWrite(V1, value);
  if (value == 100) {
    updateBuzzer(true);
  } else {
    updateBuzzer(false);
  }
}

void testBuzzer() {
  Serial.println("Testing buzzer...");
  updateBuzzer(true);  // Turn buzzer ON
  delay(1000);         // Wait for 1 second
  updateBuzzer(false); // Turn buzzer OFF
  delay(1000);         // Wait for 1 second
}

void setup() {
  Serial.begin(9600);
  pinMode(Buzzer, OUTPUT);
  pinMode(flame, INPUT);
  pinMode(PIR, INPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  Blynk.begin(auth, ssid, pass);
  dht.begin();
  testBuzzer(); // Test the buzzer during setup
  timer.setInterval(1000L, DHT11sensor);
  timer.setInterval(1000L, flamesensor);
  timer.setInterval(1000L, pirsensor);
  timer.setInterval(1000L, control_heater);
}

void loop() {
  Blynk.run();
  timer.run();
}