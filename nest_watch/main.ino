#include "esp_camera.h"
#include <WiFi.h>

// ESP32-CAM (AI-Thinker) Pin Definition
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27

#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

WiFiServer server(80);

void startCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_SXGA;  // size format
  config.jpeg_quality = 4;             // 0-63 lower = better quality
  config.fb_count = 1;

  // Initialize camera
  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera init failed");
    while (true)
      ;  // halt
  }
}


// Replace with your network credentials
const char* ssid = "SH-WIFI";
const char* password = "btUA4hTrBPZv3tIL";

// Static IP configuration
IPAddress local_IP(192, 168, 0, 100);  // Your desired IP
IPAddress gateway(192, 168, 0, 1);     // Usually your router's IP
IPAddress subnet(255, 255, 255, 0);    // Typical subnet
IPAddress primaryDNS(1, 1, 1, 1);      // Optional
IPAddress secondaryDNS(8, 8, 8, 8);    // Optional

void setup() {
  Serial.begin(115200);

  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);  // Ensure it's off at boot

  delay(1000);

  // Set static IP
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

  WiFi.begin(ssid, password);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected with static IP: ");
  Serial.println(WiFi.localIP());

  // Init camera
  startCamera();

  // Start web server
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client connected");

    String req = client.readStringUntil('\r');
    Serial.println("Request: " + req);
    client.readStringUntil('\n');  // discard newline

    if (req.indexOf("GET /capture") != -1) {
      digitalWrite(4, HIGH);  // Turn on LED
      delay(300);             // Allow LED to illuminate

      // Flush old frame
      camera_fb_t* fb = esp_camera_fb_get();
      if (fb) esp_camera_fb_return(fb);

      delay(100);  // Short delay between flush and real capture

      // Now capture fresh frame
      fb = esp_camera_fb_get();
      delay(300);
      digitalWrite(4, LOW);  // Turn off LED

      if (!fb) {
        Serial.println("Camera capture failed");
        client.println("HTTP/1.1 500 Internal Server Error");
        client.println("Content-Type: text/plain");
        client.println();
        client.println("Camera capture failed");
        client.stop();
        return;
      }

      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: image/jpeg");
      client.println("Content-Length: " + String(fb->len));
      client.println("Connection: close");
      client.println();

      client.write(fb->buf, fb->len);
      esp_camera_fb_return(fb);
    }


    delay(1);
    client.stop();
    Serial.println("Client disconnected");
  }
}
