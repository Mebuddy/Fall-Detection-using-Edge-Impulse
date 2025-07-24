
#include <Wire.h>
#include <MPU6050.h>
MPU6050 mpu;
float ax, ay, az, gx, gy, gz;
float ax_offset = 0, ay_offset = 0, az_offset = 0;
float gx_offset = 0, gy_offset = 0, gz_offset = 0;
float alpha = 0.9; // Smoothing factor
float smoothed_ax = 0, smoothed_ay = 0, smoothed_az = 0;
float smoothed_gx = 0, smoothed_gy = 0, smoothed_gz = 0;
void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed!");
    while (1);
  }
  Serial.println("Calibrating... Keep device still...");
  delay(1000);
  calibrateSensor();
  Serial.println("Calibration done.");
}
void loop() {
  int16_t raw_ax, raw_ay, raw_az;
  int16_t raw_gx, raw_gy, raw_gz;
  mpu.getMotion6(&raw_ax, &raw_ay, &raw_az, &raw_gx, &raw_gy, &raw_gz);
  // Convert to 'g' and '°/s' units
  ax = (raw_ax - ax_offset) / 16384.0;
  ay = (raw_ay - ay_offset) / 16384.0;
  az = (raw_az - az_offset) / 16384.0;
  gx = (raw_gx - gx_offset) / 131.0;
  gy = (raw_gy - gy_offset) / 131.0;
  gz = (raw_gz - gz_offset) / 131.0;
  // Apply smoothing
  smoothed_ax = alpha * smoothed_ax + (1 - alpha) * ax;
  smoothed_ay = alpha * smoothed_ay + (1 - alpha) * ay;
  smoothed_az = alpha * smoothed_az + (1 - alpha) * az;
  smoothed_gx = alpha * smoothed_gx + (1 - alpha) * gx;
  smoothed_gy = alpha * smoothed_gy + (1 - alpha) * gy;
  smoothed_gz = alpha * smoothed_gz + (1 - alpha) * gz;
  // Print in Edge Impulse format
  Serial.print(smoothed_ax, 3); Serial.print(",");
  Serial.print(smoothed_ay, 3); Serial.print(",");
  Serial.print(smoothed_az, 3); Serial.print(",");
  Serial.print(smoothed_gx, 3); Serial.print(",");
  Serial.print(smoothed_gy, 3); Serial.print(",");
  Serial.println(smoothed_gz, 3);
  delay(30); // 20 Hz sampling
}
void calibrateSensor() {
  long ax_sum = 0, ay_sum = 0, az_sum = 0;
  long gx_sum = 0, gy_sum = 0, gz_sum = 0;
  const int samples = 200;
  for (int i = 0; i < samples; i++) {
    int16_t raw_ax, raw_ay, raw_az, raw_gx, raw_gy, raw_gz;
    mpu.getMotion6(&raw_ax, &raw_ay, &raw_az, &raw_gx, &raw_gy, &raw_gz);
    ax_sum += raw_ax;
    ay_sum += raw_ay;
    az_sum += raw_az - 16384; // compensate for gravity
    gx_sum += raw_gx;
    gy_sum += raw_gy;
    gz_sum += raw_gz;
    delay(5);
  }
  ax_offset = ax_sum / samples;
  ay_offset = ay_sum / samples;
  az_offset = az_sum / samples;
  gx_offset = gx_sum / samples;
  gy_offset = gy_sum / samples;
  gz_offset = gz_sum / samples;
}
