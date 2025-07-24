
#include <Wire.h>
#include <MPU6050.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include YOUR CUSTOM ML MODEL
// === CONFIGURATION ZONE ===
// WiFi networks
const char* ssid1 = "";
const char* password1 = "";
const char* ssid2 = "";
const char* password2 = "";
// Confidence threshold for fall detection
const float FALL_CONFIDENCE_THRESHOLD = 0.90;
// Recipient list
struct Recipient {
  const char* phone;
  const char* apiKey;
};
Recipient recipients[] = {
  { "PHONE NUMBER AS SHOWN IN THE MESSAGE", "API_KEY AS SHOWN IN THE MESSAGE" },
  { "PHONE NUMBER AS SHOWN IN THE MESSAGE", "API_KEY AS SHOWN IN THE MESSAGE" },
  { "PHONE NUMBER AS SHOWN IN THE MESSAGE", "API_KEY AS SHOWN IN THE MESSAGE" }
};
const int totalRecipients = sizeof(recipients) / sizeof(recipients[0]);
// === END CONFIGURATION ===
MPU6050 mpu;
float features[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];
ei_impulse_result_t result;
void setup() {
  Serial.begin(115200);
  connectToWiFi(); // Tries both networks
  Wire.begin(21, 22); // SCL AND SDA PINS
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 not connected!");
    while (1);
  }
  Serial.println("MPU6050 ready!");
}
void loop() {
  for (int i = 0; i < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE; i += EI_CLASSIFIER_RAW_SAMPLE_COUNT) {
    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    features[i + 0] = ax / 16384.0f;
    features[i + 1] = ay / 16384.0f;
    features[i + 2] = az / 16384.0f;
    features[i + 3] = gx / 131.0f;
    features[i + 4] = gy / 131.0f;
    features[i + 5] = gz / 131.0f;
    delay(1000 / EI_CLASSIFIER_FREQUENCY);
  }
  signal_t signal;
  numpy::signal_from_buffer(features, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
  EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);
  if (res != EI_IMPULSE_OK) {
    Serial.println("Classifier error");
    return;
  }
  Serial.println("Prediction results:");
  for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
    Serial.print("  ");
    Serial.print(result.classification[ix].label);
    Serial.print(": ");
    Serial.println(result.classification[ix].value, 4);
    // Check for fall detection
    if (strcmp(result.classification[ix].label, "Falling") == 0 &&
        result.classification[ix].value >= FALL_CONFIDENCE_THRESHOLD) {
      Serial.println("Fall Detected! Sending WhatsApp alerts...");
      String message = " Alert: A fall has been detected by the ESP32 device!";
      for (int i = 0; i < totalRecipients; i++) {
        sendWhatsAppMessage(recipients[i].phone, recipients[i].apiKey, message);
        delay(10000); // avoid rate limits
      }
    }
  }
  Serial.println("------------------------");
  delay(1000);
}
// === Function to connect to WiFi ===
void connectToWiFi() {
  Serial.print("Connecting to WiFi: ");
  WiFi.begin(ssid1, password1);
  for (int i = 0; i < 20 && WiFi.status() != WL_CONNECTED; i++) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\n Failed to connect to WiFi 1. Trying WiFi 2...");
    WiFi.begin(ssid2, password2);
    for (int i = 0; i < 20 && WiFi.status() != WL_CONNECTED; i++) {
      delay(500);
      Serial.print(".");
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n Connected to WiFi: " + WiFi.SSID());
  } else {
    Serial.println("\n Failed to connect to any WiFi.");
  }
}
// === Function to send WhatsApp messages ===
void sendWhatsAppMessage(const char* phone, const char* apiKey, String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://api.whatabot.net/whatsapp/sendMessage?text=" +
                 urlencode(message) + "&apikey=" + apiKey + "&phone=" + phone;
    Serial.println("Sending to " + String(phone));
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.println("Sent. Response: " + http.getString());
    } else {
      Serial.println("Failed to send. HTTP Code: " + String(httpCode));
    }
    http.end();
  } else {
    Serial.println("WiFi not connected.");
  }
}
// URL encoding
String urlencode(String str) {
  String encoded = "";
  char c, code0, code1;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (isalnum(c)) {
      encoded += c;
    } else {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9) code1 = (c & 0xf) - 10 + 'A';
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9) code0 = c - 10 + 'A';
      encoded += '%';
      encoded += code0;
      encoded += code1;
    }
  }
  return encoded;
}
