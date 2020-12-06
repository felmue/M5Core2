// M5Core2 - light sleep / deep sleep, timer and touch wake
// Note: experimental - use at your own risk
#include <M5Core2.h>

#define CST_POWER_MODE_ACTIVE (0x00)
#define CST_POWER_MODE_MONITOR (0x01)
#define CST_POWER_MODE_HIBERNATE (0x03) // deep sleep

#define I2C_ADDR_AXP 0x34
#define I2C_ADDR_TOUCH 0x38

void WriteNByte(uint8_t addr, uint8_t reg, uint8_t num, uint8_t *data)
{
  Wire1.beginTransmission(addr);
  Wire1.write(reg);
  Wire1.write(data, num);
  Wire1.endTransmission();
}

uint8_t ReadNByte(uint8_t addr, uint8_t reg, uint8_t num, uint8_t *data)
{
  uint8_t i = 0;

  Wire1.beginTransmission(addr);
  Wire1.write(reg);
  Wire1.endTransmission(false);
  Wire1.requestFrom((int) addr, (int) num);
  while(Wire1.available())
  {
    data[i++] = Wire1.read();
  }
  return i;
}

// Select source for BUS_5V
// 0 : use internal boost
// 1 : powered externally
void AxpSetBusPowerMode(uint8_t state)
{
  uint8_t data;
  if (state == 0)
  {
    // Set GPIO to 3.3V (LDO OUTPUT mode)
    ReadNByte(I2C_ADDR_AXP, 0x91, 1, &data);
    data = (data & 0x0F) | 0xF0;
    WriteNByte(I2C_ADDR_AXP, 0x91, 1, &data);
    // Set GPIO0 to LDO OUTPUT, pullup N_VBUSEN to disable VBUS supply from BUS_5V
    ReadNByte(I2C_ADDR_AXP, 0x90, 1, &data);
    data = (data & 0xF8) | 0x02;
    WriteNByte(I2C_ADDR_AXP, 0x90, 1, &data);
    // Set EXTEN to enable 5v boost
    ReadNByte(I2C_ADDR_AXP, 0x10, 1, &data);
    data = data | 0x04;
    WriteNByte(I2C_ADDR_AXP, 0x10, 1, &data);
  }
  else
  {
    // Set EXTEN to disable 5v boost
    ReadNByte(I2C_ADDR_AXP, 0x10, 1, &data);
    data = data & ~0x04;
    WriteNByte(I2C_ADDR_AXP, 0x10, 1, &data);
    // Set GPIO0 to float, using enternal pulldown resistor to enable VBUS supply from BUS_5V
    ReadNByte(I2C_ADDR_AXP, 0x90, 1, &data);
    data = (data & 0xF8) | 0x07;
    WriteNByte(I2C_ADDR_AXP, 0x90, 1, &data);
  }
}

void TouchSetPowerMode(uint8_t mode)
{
  uint8_t data;

  delay(200); // w/o delay hypernation doesn't work
  data = mode;
  WriteNByte(I2C_ADDR_TOUCH, 0xA5, 1, &data);
}

void setup()
{
  M5.begin();

  M5.Lcd.setTextFont(2);

  Serial.println("Light / Deep Sleep Test.");
  M5.Lcd.println("Light / Deep Sleep Test.");

  Serial.println("Going to light sleep for 50 seconds.");
  M5.Lcd.println("Going to light sleep for 50 seconds.");
  delay(2500);

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_39, 0);

  // Enable external 5V / disable internal 5V boost
  AxpSetBusPowerMode(1);
  // LCD - sleep mode
  M5.Lcd.sleep();
  // Periph power voltage preset (LCD_logic, SD card)
  M5.Axp.SetLDOVoltage(2, 0);
  // Touch - monitor mode
  TouchSetPowerMode(CST_POWER_MODE_MONITOR);
  // ESP32 - lower voltage
  M5.Axp.SetDCVoltage(0, 2700);

  M5.Axp.LightSleep(SLEEP_SEC(50));

  // ESP32 - restore voltage
  M5.Axp.SetDCVoltage(0, 3350);
  // Touch - active mode
  TouchSetPowerMode(CST_POWER_MODE_ACTIVE);
  // Periph power voltage preset (LCD_logic, SD card)
  M5.Axp.SetLDOVoltage(2, 3300);
  // LCD - active mode
  M5.Lcd.wakeup();
  // Disable external 5V / enable internal 5V boost
  AxpSetBusPowerMode(0);

  Serial.println("Wakeup from light sleep.");
  M5.Lcd.println("Wakeup from light sleep.");
  delay(1000);

  Serial.println("Going to deep sleep for 50 seconds.");
  M5.Lcd.println("Going to deep sleep for 50 seconds.");
  delay(2500);

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_39, 0);

  // Enable external 5V / disable internal 5V boost
  AxpSetBusPowerMode(1);
  // LCD - sleep mode
  M5.Lcd.sleep();
  // Periph power voltage preset (LCD_logic, SD card)
  M5.Axp.SetLDOVoltage(2, 0);
  // Touch - monitor mode
  TouchSetPowerMode(CST_POWER_MODE_MONITOR);
  // ESP32 - lower voltage
  M5.Axp.SetDCVoltage(0, 2700);

  gpio_deep_sleep_hold_en();

  M5.Axp.DeepSleep(SLEEP_SEC(50));
  // Never reached
}

void loop()
{

}
