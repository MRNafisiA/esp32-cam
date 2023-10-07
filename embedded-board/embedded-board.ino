#include <WiFi.h>
#include <esp_camera.h>
#include <HTTPClient.h>

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// 4 for flash led or 33 for normal led
#define LED_GPIO_NUM       4

// Replace with your network credentials
const char* ssid = "HEPCO-AP";
const char* password = "****";

// Replace with your server's endpoint URL
const char* serverEndpoint = "http://172.20.21.76:9000/receive-image";

const camera_config_t config = {
    .pin_pwdn = PWDN_GPIO_NUM,
    .pin_reset = RESET_GPIO_NUM,
    .pin_xclk = XCLK_GPIO_NUM,
    .pin_sscb_sda = SIOD_GPIO_NUM,
    .pin_sscb_scl = SIOC_GPIO_NUM,
    .pin_d7 = Y9_GPIO_NUM,
    .pin_d6 = Y8_GPIO_NUM,
    .pin_d5 = Y7_GPIO_NUM,
    .pin_d4 = Y6_GPIO_NUM,
    .pin_d3 = Y5_GPIO_NUM,
    .pin_d2 = Y4_GPIO_NUM,
    .pin_d1 = Y3_GPIO_NUM,
    .pin_d0 = Y2_GPIO_NUM,
    .pin_vsync = VSYNC_GPIO_NUM,
    .pin_href = HREF_GPIO_NUM,
    .pin_pclk = PCLK_GPIO_NUM,
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,
    .pixel_format = PIXFORMAT_JPEG,
    .frame_size = FRAMESIZE_UXGA,
    .jpeg_quality = 10,
    .fb_count = 1
};

void setup() {
  Serial.begin(115200);
  delay(1000); // Allow time for the serial monitor to open

  Serial.println("Connecting to Wi-Fi...");
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");

  // Initialize the camera
  Serial.println("Initializing camera...");
  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera initialization failed");
    while (1);
  }
  Serial.println("Camera initialized");
}

void loop() {
  // Capture an image
  camera_fb_t * fb = esp_camera_fb_get();
  if(!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  // Create an HTTP client
  HTTPClient http;

  // Set the server's endpoint URL
  http.begin(serverEndpoint);

  // Set the content type as "image/jpeg"
  http.addHeader("Content-Type", "image/jpeg");

  // Send the image data in the request body
  int httpCode = http.POST(fb->buf, fb->len);
  if (httpCode == HTTP_CODE_OK) {
    Serial.println("Image sent successfully");
  } else {
    Serial.println("Failed to send image");
  }

  // Release the camera buffer
  esp_camera_fb_return(fb);

  // End the HTTP client
  http.end();

  // Delay between capturing and sending images (adjust as needed)
  delay(200);
}
