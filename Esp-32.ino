#include "esp_camera.h"
#include "img_converters.h"
#include "Arduino.h"
#include <ESP32Servo.h>
#include "fb_gfx.h"
#include "fd_forward.h"
#include "fr_forward.h"



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

unsigned char color_counter = 0;

char renk = 'a';

typedef enum {
    RED,
    GREEN,
    BLUE,
    ORANGE,
    YELLOW,
    BROWN,
    BLACK,
    UNDEFINED = 255
} Color;

char* color_names[7] = {
    "RED", "GREEN", "BLUE", "ORANGE", "YELLOW", "BROWN","BLACK"
};

Color find_color(uint8_t color[3]) {
    static uint8_t color_table[7][3] = {
        {40, 71, 73},    // RED
        {84, 62, 84},    // GREEN
        {98, 76, 41},    // BLUE
        {51, 73, 83},   // ORANGE
        {72, 73, 91},   // YELLOW
        {53, 62, 57},   // BROWN
        {49, 55, 49},   // BLACK
    };

    uint32_t closest_distance = 0xFFFFFFFF;
    uint8_t closest_color = UNDEFINED;

    for (uint8_t i = 0; i < 7; ++i) {
        int32_t distance = 0;
        for (uint8_t dim = 0; dim < 3; ++dim) {
            distance += (color_table[i][dim] - color[dim]) * (color_table[i][dim] - color[dim]);
        }

        if (distance < closest_distance) {
            closest_distance = distance;
            closest_color = i;
        }
    }

    return static_cast<Color>(closest_color);
}

void setup() {
    Serial.begin(38400);
    Serial.setDebugOutput(true);
    Serial.println();


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
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    if (psramFound()) {
        config.frame_size = FRAMESIZE_UXGA;
        config.jpeg_quality = 10;
        config.fb_count = 2;
    } else {
        config.frame_size = FRAMESIZE_SVGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x", err);
        return;
    }

    sensor_t* s = esp_camera_sensor_get();
    s->set_framesize(s, FRAMESIZE_QQVGA);
    s->set_saturation(s, 2);
}

void loop() {
      camera_fb_t* fb = NULL;

      fb = esp_camera_fb_get();
      if (!fb) {
          Serial.println("Camera capture failed");
          return;
      }

      size_t out_len, out_width, out_height;
      uint8_t* out_buf;
      bool s;

      dl_matrix3du_t* image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);
      if (!image_matrix) {
          esp_camera_fb_return(fb);
          Serial.println("dl_matrix3du_alloc failed");
          return;
      }

      out_buf = image_matrix->item;
      out_len = fb->width * fb->height * 3;
      out_width = fb->width;
      out_height = fb->height;

      s = fmt2rgb888(fb->buf, fb->len, fb->format, out_buf);
      esp_camera_fb_return(fb);
      if (!s) {
          dl_matrix3du_free(image_matrix);
          Serial.println("to rgb888 failed");
          return;
      } else {
          double r_average = 0;
          double g_average = 0;
          double b_average = 0;
          double r_row = 0;
          double g_row = 0;
          double b_row = 0;
          for (int i = 0; i < out_len; i += 3) {
              r_row += out_buf[i + 2];
              g_row += out_buf[i + 1];
              b_row += out_buf[i];
          }
          r_average = r_row / (out_len / 3);
          g_average = g_row / (out_len / 3);
          b_average = b_row / (out_len / 3);
          //Serial.printf("r=%d g=%d b=%d len=%d w=%d h=%d\n", (int)r_average, (int)g_average, (int)b_average, out_len, out_width, out_height);

          uint8_t color[3] = {(uint8_t)r_average, (uint8_t)g_average, (uint8_t)b_average};
          Color closest_color = find_color(color);
          switch (closest_color) {
              case 0:
                if (renk != 'R') {
                  renk = 'R';
                  color_counter = 0; 
                }
                break;
              case 1:
                if (renk != 'G') {
                  renk = 'G';
                  color_counter = 0; 
                }
                break;
              case 2:
                if (renk != 'B') {
                  renk = 'B';
                  color_counter = 0; 
                }
                break;
              case 3:
                if (renk != 'O') {
                  renk = 'O';
                  color_counter = 0; 
                }
                break;
              case 4:
                if (renk != 'Y') {
                  renk = 'Y';
                  color_counter = 0; 
                }
                break;
              case 5:
                if (renk != 'W') {
                  renk = 'W';
                  color_counter = 0; 
                }
                break;
              case 6:
                if (renk != 'P') {
                  renk = 'P';
                }
                break;
              default:
                break;
            }
          //Serial.printf("Color: %s\n", color_names[closest_color]);
      }
      dl_matrix3du_free(image_matrix);
      
      if (renk == 'R') {
        if (color_counter < 14) {
          color_counter++; 
          if (color_counter == 14) {
            Serial.print("R");
          }
        }
      }
      else if (renk == 'G') {
        if (color_counter < 14) {
          color_counter++; 
          if (color_counter == 14) {
            Serial.print("G");
          }
        }
      }
      else if (renk == 'B') {
        if (color_counter < 14) {
          color_counter++; 
          if (color_counter == 14) {
            Serial.print("B");
          }
        }
      }
      else if (renk == 'O') {
        if (color_counter < 14) {
          color_counter++; 
          if (color_counter == 14) {
            Serial.print("O");
          }
        }
      }
      else if (renk == 'Y') {
        if (color_counter < 14) {
          color_counter++; 
          if (color_counter == 14) {
            Serial.print("Y");
          }
        }
      }
      else if (renk == 'W') {
        if (color_counter < 14) {
          color_counter++; 
          if (color_counter == 14) {
            Serial.print("W");
          }
        }
      }
}