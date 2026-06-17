#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// ================= OLED =================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display1(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

Adafruit_SSD1306 display2(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

// ================= DHT =================

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// ================= PINS =================

#define VIB_PIN 34
#define BUZZER_PIN 18

// ================= VARIABLES =================

volatile float temperature = 0;
volatile int vibration = 0;

volatile int healthScore = 100;
volatile int failureRisk = 0;

String status = "GOOD";
String fault = "NORMAL";
String serviceTime = "No Service";

bool buzzerState = false;

// =================================================
// SENSOR TASK
// =================================================

void sensorTask(void *pvParameters)
{
  while (true)
  {
    float t = dht.readTemperature();

    if (!isnan(t))
    {
      temperature = t;
    }

    int sensorValue = analogRead(VIB_PIN);

    vibration = map(
                  sensorValue,
                  0,
                  4095,
                  0,
                  100);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

// =================================================
// ANALYSIS TASK
// =================================================

void predictionTask(void *pvParameters)
{
  while (true)
  {
    int score = 100;

    // Temperature penalties
    if (temperature > 35)
      score -= 20;

    if (temperature > 45)
      score -= 30;

    // Vibration penalties
    if (vibration > 50)
      score -= 15;

    if (vibration > 80)
      score -= 25;

    healthScore = constrain(score, 0, 100);

    failureRisk = 100 - healthScore;

    // Status

    if (healthScore >= 80)
      status = "GOOD";

    else if (healthScore >= 50)
      status = "WARNING";

    else
      status = "CRITICAL";

    // Fault Detection

    if (temperature > 45 &&
        vibration > 80)
      fault = "BEARING";

    else if (temperature > 45)
      fault = "OVERHEAT";

    else if (vibration > 80)
      fault = "MISALIGN";

    else
      fault = "NORMAL";

    // Service Time

    if (healthScore > 80)
      serviceTime = "No Service";

    else if (healthScore > 60)
      serviceTime = "30 Days";

    else if (healthScore > 40)
      serviceTime = "15 Days";

    else
      serviceTime = "Immediate";

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

// =================================================
// OLED 1 TASK
// =================================================

void displayTask1(void *pvParameters)
{
  while (true)
  {
    display1.clearDisplay();

    display1.setTextColor(WHITE);
    display1.setTextSize(1);

    display1.setCursor(0, 0);
    display1.println("Machine Health");

    display1.setCursor(0, 15);
    display1.print("Temp: ");
    display1.print(temperature, 1);
    display1.println("C");

    display1.setCursor(0, 30);
    display1.print("Vib : ");
    display1.print(vibration);
    display1.println("%");

    display1.setCursor(0, 45);
    display1.print("Health:");
    display1.print(healthScore);
    display1.println("%");

    display1.setCursor(0, 56);
    display1.print(status);

    display1.display();

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

// =================================================
// OLED 2 TASK
// =================================================

void displayTask2(void *pvParameters)
{
  while (true)
  {
    display2.clearDisplay();

    display2.setTextColor(WHITE);
    display2.setTextSize(1);

    display2.setCursor(0, 0);
    display2.println("Prediction");

    display2.setCursor(0, 15);
    display2.print("Risk: ");
    display2.print(failureRisk);
    display2.println("%");

    display2.setCursor(0, 30);
    display2.print("Svc:");

    if(serviceTime == "Immediate")
      display2.print("NOW");
    else
      display2.print(serviceTime);

    display2.setCursor(0, 45);
    display2.print("Fault:");
    display2.print(fault);

    display2.setCursor(0, 56);

    if(buzzerState)
      display2.print("ALERT ON");
    else
      display2.print("ALERT OFF");

    display2.display();

    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

// =================================================
// ALERT TASK
// =================================================

void alertTask(void *pvParameters)
{
  while (true)
  {
    if (failureRisk > 70)
    {
      digitalWrite(BUZZER_PIN, HIGH);
      buzzerState = true;
    }
    else
    {
      digitalWrite(BUZZER_PIN, LOW);
      buzzerState = false;
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// =================================================
// SETUP
// =================================================

void setup()
{
  Serial.begin(115200);

  dht.begin();

  pinMode(BUZZER_PIN, OUTPUT);

  Wire.begin(21, 22);

  // OLED 1
  if (!display1.begin(
        SSD1306_SWITCHCAPVCC,
        0x3C))
  {
    Serial.println("OLED1 Failed");
    while (true);
  }

  // OLED 2
  if (!display2.begin(
        SSD1306_SWITCHCAPVCC,
        0x3D))
  {
    Serial.println("OLED2 Failed");
    while (true);
  }

  display1.clearDisplay();
  display2.clearDisplay();

  display1.display();
  display2.display();

  // FreeRTOS Tasks

  xTaskCreate(
    sensorTask,
    "Sensor",
    4096,
    NULL,
    2,
    NULL);

  xTaskCreate(
    predictionTask,
    "Prediction",
    4096,
    NULL,
    3,
    NULL);

  xTaskCreate(
    displayTask1,
    "OLED1",
    4096,
    NULL,
    1,
    NULL);

  xTaskCreate(
    displayTask2,
    "OLED2",
    4096,
    NULL,
    1,
    NULL);

  xTaskCreate(
    alertTask,
    "Alert",
    2048,
    NULL,
    4,
    NULL);
}

void loop()
{
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}