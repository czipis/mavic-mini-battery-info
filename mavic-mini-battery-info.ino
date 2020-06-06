/*
thanks to
PowerCartel for smart battery routines - https://github.com/PowerCartel/PackProbe
Bodmer for fast TFT library - https://github.com/Bodmer/TFT_ST7735
Alain Aeropic - for BatMan inspiration - https://www.thingiverse.com/thing:4235767 
*/

#define VERSION   "v1.0"

#include <TFT_ST7735.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>

#include <Wire.h>
byte deviceAddress = 11;

// Standard and common non-standard Smart Battery commands
#define BATTERY_MODE             0x03
#define TEMPERATURE              0x08
#define VOLTAGE                  0x09
#define CURRENT                  0x0A
#define RELATIVE_SOC             0x0D
#define ABSOLUTE_SOC             0x0E
#define REMAINING_CAPACITY       0x0F
#define FULL_CHARGE_CAPACITY     0x10
#define TIME_TO_FULL             0x13
#define CHARGING_CURRENT         0x14
#define CHARGING_VOLTAGE         0x15
#define BATTERY_STATUS           0x16
#define CYCLE_COUNT              0x17
#define DESIGN_CAPACITY          0x18
#define DESIGN_VOLTAGE           0x19
#define SPEC_INFO                0x1A
#define MFG_DATE                 0x1B
#define SERIAL_NUM               0x1C
#define MFG_NAME                 0x20   // String
#define DEV_NAME                 0x21   // String
#define CELL_CHEM                0x22   // String
#define MFG_DATA                 0x23   // String
#define CELL4_VOLTAGE            0x3C
#define CELL3_VOLTAGE            0x3D
#define CELL2_VOLTAGE            0x3E
#define CELL1_VOLTAGE            0x3F
#define STATE_OF_HEALTH          0x4F
#define DJI_SERIAL               0xD8  // String

#define bufferLen 32
uint8_t i2cBuffer[bufferLen];

TFT_ST7735 tft = TFT_ST7735();
#include "dji_logo-48x48.h"

#define TFT_BACKGROUND  0xD6BB

void setup()
{
  Serial.begin(115200);

  Wire.begin();
  Wire.setClock(100000);
    
  tft.init();
  tft.setRotation(0);	// portrait

  tft.fillScreen(TFT_BLACK);

  drawIcon(dji_logo, (tft.width() - Width)/2,      (tft.height() - Height)/2-40, Width, Height);
  tft.setTextSize(1);
  tft.setTextColor(TFT_BACKGROUND, TFT_BLACK);
  tft.drawCentreString("Mavic Mini", 64, 68, 2);
  tft.drawCentreString("battery info", 64, 90, 1);
  tft.drawCentreString(VERSION, 64, 102, 1);
  tft.drawCentreString("github.com/czipis", 64, 120, 1);

  delay(3000);
  tft.fillScreen(TFT_BACKGROUND);
}

uint8_t read_byte()
{
  while (1)
  {
    if (Wire.available())
    {
      return Wire.read();
    }
  }
}

int fetchWord(byte func)
{
  Wire.beginTransmission(deviceAddress);
  Wire.write(func);
  Wire.endTransmission(false);
  delay(1);// FIX wire bug
  Wire.requestFrom(deviceAddress, 2, true);
  
  uint8_t b1 = read_byte();
  uint8_t b2 = read_byte();
  Wire.endTransmission();
  return (int)b1 | ((( int)b2) << 8);
}



uint8_t i2c_smbus_read_block ( uint8_t command, uint8_t* blockBuffer, uint8_t blockBufferLen )
{
  uint8_t x, num_bytes;
  Wire.beginTransmission(deviceAddress);
  Wire.write(command);
  Wire.endTransmission(false);
  delay(1);
  Wire.requestFrom(deviceAddress, blockBufferLen, true);
  
  num_bytes = read_byte();
  num_bytes = constrain(num_bytes, 0, blockBufferLen - 2);
  for (x = 0; x < num_bytes - 1; x++) { // -1 because x=num_bytes-1 if x<y; last byte needs to be "nack"'d, x<y-1
    blockBuffer[x] = read_byte();
  }
  blockBuffer[x++] = read_byte(); // this will nack the last byte and store it in x's num_bytes-1 address.
  blockBuffer[x] = 0; // and null it at last_byte+1
  Wire.endTransmission();
  return num_bytes;
}

#define BUFF_SIZE 64
void drawIcon(const unsigned short* icon, int16_t x, int16_t y, int8_t width, int8_t height) {

  uint16_t  pix_buffer[BUFF_SIZE];   // Pixel buffer (16 bits per pixel)

  // Set up a window the right size to stream pixels into
  tft.setAddrWindow(x, y, x + width - 1, y + height - 1);

  // Work out the number whole buffers to send
  uint16_t nb = ((uint16_t)height * width) / BUFF_SIZE;

  // Fill and send "nb" buffers to TFT
  for (int i = 0; i < nb; i++) {
    for (int j = 0; j < BUFF_SIZE; j++) {
      pix_buffer[j] = pgm_read_word(&icon[i * BUFF_SIZE + j]);
    }
    tft.pushColors(pix_buffer, BUFF_SIZE);
  }

  // Work out number of pixels not yet sent
  uint16_t np = ((uint16_t)height * width) % BUFF_SIZE;

  // Send any partial buffer left over
  if (np) {
    for (int i = 0; i < np; i++) pix_buffer[i] = pgm_read_word(&icon[nb * BUFF_SIZE + i]);
    tft.pushColors(pix_buffer, np);
  }
}


void loop()
{
  uint8_t length_read = 0;
  

  Serial.print("Manufacturer Name: ");
  length_read = i2c_smbus_read_block(MFG_NAME, i2cBuffer, bufferLen);
  Serial.write(i2cBuffer, length_read);
  Serial.println("");

  Serial.print("Device Name: ");
  length_read = i2c_smbus_read_block(DEV_NAME, i2cBuffer, bufferLen);
  Serial.write(i2cBuffer, length_read);
  Serial.println("");

  Serial.print("Chemistry ");
  length_read = i2c_smbus_read_block(CELL_CHEM, i2cBuffer, bufferLen);
  Serial.write(i2cBuffer, length_read);
  Serial.println("");

  Serial.print("Manufacturer Data: ");
  length_read = i2c_smbus_read_block(MFG_DATA, i2cBuffer, bufferLen);
  Serial.write(i2cBuffer, length_read);
  Serial.println("");

  Serial.print("Design Capacity: " );
  Serial.println(fetchWord(DESIGN_CAPACITY));

  Serial.print("Design Voltage: " );
  Serial.println(fetchWord(DESIGN_VOLTAGE));

  Serial.print("Manufacture Date (D.M.Y): " );

  String formatted_date;
  int mdate = fetchWord(MFG_DATE);
  int mday = B00011111 & mdate;
  int mmonth = mdate >> 5 & B00001111;
  int myear = 1980 + (mdate >> 9 & B01111111);
  formatted_date += mday;
  formatted_date += ".";
  formatted_date += mmonth;
  formatted_date += ".";
  formatted_date += myear;
  Serial.println(formatted_date);
  int str_len = formatted_date.length() + 1; 
  char mfg_date[str_len];
  formatted_date.toCharArray(mfg_date, str_len);


  Serial.print("Serial Number: ");
  length_read = i2c_smbus_read_block(DJI_SERIAL, i2cBuffer, bufferLen);
  char *djiserial = i2cBuffer;
  Serial.println(djiserial); 

  Serial.print("Specification Info: ");
  Serial.println(fetchWord(SPEC_INFO));

  Serial.print("Cycle Count: " );
  uint8_t cycles = fetchWord(CYCLE_COUNT);
  Serial.println(cycles);

  Serial.print("Voltage: ");
  float voltage = (float)fetchWord(VOLTAGE) / 1000;
  Serial.println(voltage);

  Serial.print("Full Charge Capacity: " );
  Serial.println(fetchWord(FULL_CHARGE_CAPACITY));

  Serial.print("Remaining Capacity: " );
  Serial.println(fetchWord(REMAINING_CAPACITY));

  Serial.print("Relative Charge(%): ");
  uint8_t charge = fetchWord(RELATIVE_SOC);
  Serial.println(charge);


  Serial.print("Absolute Charge(%): ");
  Serial.println(fetchWord(ABSOLUTE_SOC));

  Serial.print("Minutes remaining for full charge: ");
  Serial.println(fetchWord(TIME_TO_FULL));

  Serial.print("Cell 1 Voltage: ");
  float cell1 = (float)fetchWord(CELL1_VOLTAGE)/1000;
  Serial.println(cell1);
  Serial.print("Cell 2 Voltage: ");
  float cell2 = (float)fetchWord(CELL2_VOLTAGE)/1000;
  Serial.println(cell2);
  char buffer[5];
  String v = dtostrf(voltage, 4, 2, buffer);
  String c1 = dtostrf(cell1, 4, 2, buffer);
  String c2 = dtostrf(cell2, 4, 2, buffer);
  String cells = v + " " + c1 + '/' + c2;
  str_len = cells.length() + 1; 
  char cellsV[str_len];
  cells.toCharArray(cellsV, str_len);


  Serial.print("State of Health: ");
  Serial.println(fetchWord(STATE_OF_HEALTH));

  Serial.print("Battery Mode (BIN): 0b");
  Serial.println(fetchWord(BATTERY_MODE), BIN);

  Serial.print("Battery Status (BIN): 0b");
  Serial.println(fetchWord(BATTERY_STATUS), BIN);

  Serial.print("Charging Current: ");
  Serial.println(fetchWord(CHARGING_CURRENT));

  Serial.print("Charging Voltage: ");
  Serial.println(fetchWord(CHARGING_VOLTAGE));

  Serial.print("Temp: ");
  unsigned int tempk = fetchWord(TEMPERATURE);
  float temp = tempk / 10.0 - 273.15;
  Serial.println(temp);

  Serial.print("Current (mA): " );
  Serial.println(fetchWord(CURRENT));

  Serial.println(".");

  
  drawIcon(dji_logo, (tft.width() - Width)/2,      0, Width, Height);

  tft.setTextSize(1);
  tft.setTextColor(TFT_BLACK, TFT_BLACK);

  tft.drawString("SERIAL", 3, 54, 1);
  tft.drawRightString(djiserial, 126, 54, 1);
  
  tft.drawString("PRODUCED", 3, 54+15, 1);
  tft.drawRightString(mfg_date, 126, 54+15, 1);
  
  tft.drawString("CYCLES", 3, 54+30, 1);b
  tft.drawNumber(cycles, 45, 54+30, 1);
  tft.drawString("TEMP", 70, 54+30, 1);
  tft.fillRect(102, 54+30 , 25, 10, TFT_BACKGROUND);
  tft.drawFloat(temp, 1, 102, 54+30, 1);
  
  tft.drawString("VOLTS", 3, 54+45, 1);
  tft.fillRect(40, 54+45 , 87, 10, TFT_BACKGROUND);
  tft.drawRightString(cellsV, 126, 54+45, 1);

  unsigned int batt_width = 110;
  unsigned int batt_height = 20;
  tft.drawRect(tft.width()/2 - batt_width/2, 130 , batt_width, batt_height, TFT_BLACK);
  tft.drawRect(tft.width()/2 + batt_width/2 - 1 , 130+batt_height/4 , 5, batt_height/2, TFT_BLACK);
  uint16_t color = TFT_RED;
  if (charge > 25) color = TFT_ORANGE;
  if (charge > 50) color = TFT_YELLOW;
  if (charge > 75) color = TFT_DARKGREEN;
  tft.fillRect(tft.width()/2 - batt_width/2 + 5, 130 + 2 , charge, batt_height-4, color);
  String pct = String(charge) + '%';
  str_len = pct.length() + 1; 
  char p[str_len];
  pct.toCharArray(p, str_len);
  tft.drawString(p,54,132, 2);

  delay(1000);
}
