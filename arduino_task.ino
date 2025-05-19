// lcd pin connections
const int rs = 7;
const int en = 8;
const int d4 = 9;
const int d5 = 10;
const int d6 = 11;
const int d7 = 12;

// sensors
const int tempPin = A0;
const int ldrPin = A1;

void pulseEnable() {
  digitalWrite(en, LOW);
  delayMicroseconds(1);
  digitalWrite(en, HIGH);
  delayMicroseconds(1);
  digitalWrite(en, LOW);
  delayMicroseconds(100);
}

void send4Bits(int value) {
  digitalWrite(d4, (value >> 0) & 0x01);
  digitalWrite(d5, (value >> 1) & 0x01);
  digitalWrite(d6, (value >> 2) & 0x01);
  digitalWrite(d7, (value >> 3) & 0x01);
}

void lcdCmd(byte cmd) {
  digitalWrite(rs, LOW);
  send4Bits(cmd >> 4);
  pulseEnable();
  send4Bits(cmd & 0x0F);
  pulseEnable();
  delay(2);
}

void lcdChar(byte data) {
  digitalWrite(rs, HIGH);
  send4Bits(data >> 4);
  pulseEnable();
  send4Bits(data & 0x0F);
  pulseEnable();
  delay(2);
}

void lcdInit() {
  delay(50);
  send4Bits(0x03);
  pulseEnable();
  delay(5);
  pulseEnable();
  delayMicroseconds(150);
  pulseEnable();

  send4Bits(0x02);  // 4-bit mode
  pulseEnable();

  lcdCmd(0x28);     // 2-line, 5x8
  lcdCmd(0x0C);     // display on, cursor off
  lcdCmd(0x06);     // entry mode
  lcdCmd(0x01);     // clear
  delay(5);
}

void lcdClear() {
  lcdCmd(0x01);
  delay(2);
}

void lcdSetCursor(byte col, byte row) {
  byte row_offsets[] = {0x00, 0x40};
  lcdCmd(0x80 | (col + row_offsets[row]));
}

void lcdPrint(const char* str) {
  while (*str) {
    lcdChar(*str++);
  }
}

void lcdPrintFloat(float val, int precision = 1) {
  char buffer[16];
  dtostrf(val, 0, precision, buffer);
  lcdPrint(buffer);
}

void lcdCreateChar(byte location, byte charmap[]) {
  location &= 0x7;
  lcdCmd(0x40 | (location << 3));
  for (int i = 0; i < 8; i++) {
    lcdChar(charmap[i]);
  }
}

// custom characters
byte thermometer[8] = {
  B00100,
  B01010,
  B01010,
  B01110,
  B01110,
  B11111,
  B11111,
  B01110
};

byte lightBulb[8] = {
  B00100,
  B01110,
  B01110,
  B00100,
  B00100,
  B00000,
  B00100,
  B00000
};

void setup() {
  for (int pin = rs; pin <= d7; pin++) {
    pinMode(pin, OUTPUT);
  }

  lcdInit();
  lcdCreateChar(0, thermometer);
  lcdCreateChar(1, lightBulb);
}

void loop() {
  int rawValue = analogRead(tempPin);
  float voltage = rawValue * (5.0 / 1023.0);
  float tempC = voltage / 0.01;

  lcdClear();
  lcdSetCursor(0, 0);
  lcdChar(0);  // thermometer
  lcdPrint(" Temp: ");
  lcdPrintFloat(tempC, 1);
  lcdPrint("C");

  delay(2000);

  int lightRaw = analogRead(ldrPin);
  int lightLevel = map(lightRaw, 0, 1023, 0, 100);

  lcdClear();
  lcdSetCursor(0, 0);
  lcdChar(1);  // light bulb
  lcdPrint(" Light: ");
  lcdPrintFloat(lightLevel, 0);
  lcdPrint("%");

  delay(2000);
}
