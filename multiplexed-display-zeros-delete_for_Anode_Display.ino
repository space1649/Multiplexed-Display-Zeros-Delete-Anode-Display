/**
 * Modofied For Anode Type Display  // Modfied for Anode  ++++++++++++++++++++++++++++++++++++++++++
 * This sketch is specifically for programming the EEPROM used in the 8-bit
 * decimal display decoder described in https://youtu.be/dLh1n2dErzE
 */
#define SHIFT_DATA 2
#define SHIFT_CLK 3
#define SHIFT_LATCH 4
#define EEPROM_D0 5
#define EEPROM_D7 12
#define WRITE_EN 13

/*
   Output the address bits and outputEnable signal using shift registers.
*/
void setAddress(int address, bool outputEnable) {
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, (address >> 8) | (outputEnable ? 0x00 : 0x80));
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, address);

  digitalWrite(SHIFT_LATCH, LOW);
  digitalWrite(SHIFT_LATCH, HIGH);
  digitalWrite(SHIFT_LATCH, LOW);
}


/*
   Read a byte from the EEPROM at the specified address.
*/
byte readEEPROM(int address) {
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, INPUT);
  }
  setAddress(address, /*outputEnable*/ true);

  byte data = 0;
  for (int pin = EEPROM_D7; pin >= EEPROM_D0; pin -= 1) {
    data = (data << 1) + digitalRead(pin);
  }
  return data;
}


/*
   Write a byte to the EEPROM at the specified address.
*/
void writeEEPROM(int address, byte data) {
  setAddress(address, /*outputEnable*/ false);
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, OUTPUT);
  }

  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    digitalWrite(pin, data & 1);
    data = data >> 1;
  }
  digitalWrite(WRITE_EN, LOW);
  delayMicroseconds(1);
  digitalWrite(WRITE_EN, HIGH);
  delay(10);
}


/*
   Read the contents of the EEPROM and print them to the serial monitor.
*/
void printContents() {
  for (int base = 0; base <= 2047; base += 16) {   // changed 255 to 2024
    byte data[16];
    for (int offset = 0; offset <= 15; offset += 1) {
      data[offset] = readEEPROM(base + offset);
    }

    char buf[800];   // original 80
    sprintf(buf, "%03x:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

    Serial.println(buf);
  }
}


void setup() {
  // put your setup code here, to run once:
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  digitalWrite(WRITE_EN, HIGH);
  pinMode(WRITE_EN, OUTPUT);
  Serial.begin(57600);

  // Bit patterns for the digits 0..9 Anode
  byte digits[] = { 0x81, 0xCF, 0x92, 0x86, 0xCC, 0xA4, 0xA0, 0x8F, 0x80, 0x84 };  // Modfied for Anode  ++++++++++++++++++++++++++++++++++++++++++



  // Bit patterns for the digits 0..9 Cathode
  //byte digits[] = { 0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b };

  Serial.println("Programming ones place");
  for (int value = 0; value <= 255; value += 1) {
    writeEEPROM(value, digits[value % 10]);
  }
  Serial.println("Programming tens place");
  for (int value = 0; value <= 255; value += 1) {
    writeEEPROM(value + 256, digits[(value / 10) % 10]);
  }
  Serial.println("Programming hundreds place");
  for (int value = 0; value <= 255; value += 1) {
    writeEEPROM(value + 512, digits[(value / 100) % 10]);
  }
  Serial.println("Programming sign");
  for (int value = 0; value <= 255; value += 1) {
    writeEEPROM(value + 768, 0xFF);  // Modfied for Anode  ++++++++++++++++++++++++++++++++++++++++++
  }

  Serial.println("Programming ones place (twos complement)");
  for (int value = -128; value <= 127; value += 1) {
    writeEEPROM((byte)value + 1024, digits[abs(value) % 10]);
  }
  Serial.println("Programming tens place (twos complement)");
  for (int value = -128; value <= 127; value += 1) {
    writeEEPROM((byte)value + 1280, digits[abs(value / 10) % 10]);
  }
  Serial.println("Programming hundreds place (twos complement)");
  for (int value = -128; value <= 127; value += 1) {
    writeEEPROM((byte)value + 1536, digits[abs(value / 100) % 10]);
  }
  Serial.println("Programming sign (twos complement)");
  for (int value = -128; value <= 127; value += 1) {
    if (value < 0) {
      writeEEPROM((byte)value + 1792, 0xFE);  // Modfied for Anode  
    } else {
      writeEEPROM((byte)value + 1792, 0xFF);  // Modfied for Anode
    }
  }
//***********************************************************************************************************************************
// For Anode Type Display  ff= Off
//Modifying delete leading zeros
//Added Code not to display leading zeros

Serial.println("Modifying tens place");  // Write 1's to 10 addresses starting at address 256 (100h)  9 digits
  for (int value = 256; value <= 265; value += 1) {
    writeEEPROM(value, 0xFF);  // Modfied for Anode  ++++++++++++++++++++++++++++++++++++++++++
  }

Serial.println("Modifying hundreds place");  // Write 1's to 100 addresses starting at address 512 (200h)  99 digits
  for (int value = 512; value <= 611; value += 1) {
    writeEEPROM(value, 0xFF);     // Modfied for Anode  ++++++++++++++++++++++++++++++++++++++++++
  }


Serial.println("Modifying tens place (twos complement)");  // Write 10's to 10 addresses starting at address 1280 (100h)  9 digits
  for (int value = 1280; value <= 1289; value += 1) {
    writeEEPROM(value, 0xFF);  // Modfied for Anode  ++++++++++++++++++++++++++++++++++++++++++
  }

Serial.println("Modifying hundreds place (twos complement)");  // Write 1's to 100 addresses starting at address 1536 (200h)  99 digits
  for (int value = 1536; value <= 1635; value += 1) {
    writeEEPROM(value, 0xFF);   // Modfied for Anode  ++++++++++++++++++++++++++++++++++++++++++   
   }  


Serial.println("Modifying tens place (twos complement)");  // Write 1's to 10 addresses starting at address 1527 (100h)  9 digits
  for (int value = 1527; value <= 1535; value += 1) {
    writeEEPROM(value, 0xFF);   // Modfied for Anode  ++++++++++++++++++++++++++++++++++++++++++
  }

Serial.println("Modifying hundreds place (twos complement)");  // Write 1's to 100 addresses starting at address 1693 (200h)  99 digits
  for (int value = 1693; value <= 1791; value += 1) {
    writeEEPROM(value, 0xFF);    // Modfied for Anode  ++++++++++++++++++++++++++++++++++++++++++
   }  
//***********************************************************************************************************************************

  // Read and print out the contents of the EERPROM
  Serial.println("Reading EEPROM");
  printContents();
}


void loop() {
  // put your main code here, to run repeatedly:

}


/*  Output Final Mod for Anode Display
 
Programming ones place
Programming tens place
Programming hundreds place
Programming sign
Programming ones place (twos complement)
Programming tens place (twos complement)
Programming hundreds place (twos complement)
Programming sign (twos complement)
Modifying tens place
Modifying hundreds place
Modifying tens place (twos complement)
Modifying hundreds place (twos complement)
Modifying tens place (twos complement)
Modifying hundreds place (twos complement)
Reading EEPROM
000:  81 cf 92 86 cc a4 a0 8f   80 84 81 cf 92 86 cc a4
010:  a0 8f 80 84 81 cf 92 86   cc a4 a0 8f 80 84 81 cf
020:  92 86 cc a4 a0 8f 80 84   81 cf 92 86 cc a4 a0 8f
030:  80 84 81 cf 92 86 cc a4   a0 8f 80 84 81 cf 92 86
040:  cc a4 a0 8f 80 84 81 cf   92 86 cc a4 a0 8f 80 84
050:  81 cf 92 86 cc a4 a0 8f   80 84 81 cf 92 86 cc a4
060:  a0 8f 80 84 81 cf 92 86   cc a4 a0 8f 80 84 81 cf
070:  92 86 cc a4 a0 8f 80 84   81 cf 92 86 cc a4 a0 8f
080:  80 84 81 cf 92 86 cc a4   a0 8f 80 84 81 cf 92 86
090:  cc a4 a0 8f 80 84 81 cf   92 86 cc a4 a0 8f 80 84
0a0:  81 cf 92 86 cc a4 a0 8f   80 84 81 cf 92 86 cc a4
0b0:  a0 8f 80 84 81 cf 92 86   cc a4 a0 8f 80 84 81 cf
0c0:  92 86 cc a4 a0 8f 80 84   81 cf 92 86 cc a4 a0 8f
0d0:  80 84 81 cf 92 86 cc a4   a0 8f 80 84 81 cf 92 86
0e0:  cc a4 a0 8f 80 84 81 cf   92 86 cc a4 a0 8f 80 84
0f0:  81 cf 92 86 cc a4 a0 8f   80 84 81 cf 92 86 cc a4
100:  ff ff ff ff ff ff ff ff   ff ff cf cf cf cf cf cf
110:  cf cf cf cf 92 92 92 92   92 92 92 92 92 92 86 86
120:  86 86 86 86 86 86 86 86   cc cc cc cc cc cc cc cc
130:  cc cc a4 a4 a4 a4 a4 a4   a4 a4 a4 a4 a0 a0 a0 a0
140:  a0 a0 a0 a0 a0 a0 8f 8f   8f 8f 8f 8f 8f 8f 8f 8f
150:  80 80 80 80 80 80 80 80   80 80 84 84 84 84 84 84
160:  84 84 84 84 81 81 81 81   81 81 81 81 81 81 cf cf
170:  cf cf cf cf cf cf cf cf   92 92 92 92 92 92 92 92
180:  92 92 86 86 86 86 86 86   86 86 86 86 cc cc cc cc
190:  cc cc cc cc cc cc a4 a4   a4 a4 a4 a4 a4 a4 a4 a4
1a0:  a0 a0 a0 a0 a0 a0 a0 a0   a0 a0 8f 8f 8f 8f 8f 8f
1b0:  8f 8f 8f 8f 80 80 80 80   80 80 80 80 80 80 84 84
1c0:  84 84 84 84 84 84 84 84   81 81 81 81 81 81 81 81
1d0:  81 81 cf cf cf cf cf cf   cf cf cf cf 92 92 92 92
1e0:  92 92 92 92 92 92 86 86   86 86 86 86 86 86 86 86
1f0:  cc cc cc cc cc cc cc cc   cc cc a4 a4 a4 a4 a4 a4
200:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
210:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
220:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
230:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
240:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
250:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
260:  ff ff ff ff cf cf cf cf   cf cf cf cf cf cf cf cf
270:  cf cf cf cf cf cf cf cf   cf cf cf cf cf cf cf cf
280:  cf cf cf cf cf cf cf cf   cf cf cf cf cf cf cf cf
290:  cf cf cf cf cf cf cf cf   cf cf cf cf cf cf cf cf
2a0:  cf cf cf cf cf cf cf cf   cf cf cf cf cf cf cf cf
2b0:  cf cf cf cf cf cf cf cf   cf cf cf cf cf cf cf cf
2c0:  cf cf cf cf cf cf cf cf   92 92 92 92 92 92 92 92
2d0:  92 92 92 92 92 92 92 92   92 92 92 92 92 92 92 92
2e0:  92 92 92 92 92 92 92 92   92 92 92 92 92 92 92 92
2f0:  92 92 92 92 92 92 92 92   92 92 92 92 92 92 92 92
300:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
310:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
320:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
330:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
340:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
350:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
360:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
370:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
380:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
390:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
3a0:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
3b0:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
3c0:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
3d0:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
3e0:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
3f0:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
400:  81 cf 92 86 cc a4 a0 8f   80 84 81 cf 92 86 cc a4
410:  a0 8f 80 84 81 cf 92 86   cc a4 a0 8f 80 84 81 cf
420:  92 86 cc a4 a0 8f 80 84   81 cf 92 86 cc a4 a0 8f
430:  80 84 81 cf 92 86 cc a4   a0 8f 80 84 81 cf 92 86
440:  cc a4 a0 8f 80 84 81 cf   92 86 cc a4 a0 8f 80 84
450:  81 cf 92 86 cc a4 a0 8f   80 84 81 cf 92 86 cc a4
460:  a0 8f 80 84 81 cf 92 86   cc a4 a0 8f 80 84 81 cf
470:  92 86 cc a4 a0 8f 80 84   81 cf 92 86 cc a4 a0 8f
480:  80 8f a0 a4 cc 86 92 cf   81 84 80 8f a0 a4 cc 86
490:  92 cf 81 84 80 8f a0 a4   cc 86 92 cf 81 84 80 8f
4a0:  a0 a4 cc 86 92 cf 81 84   80 8f a0 a4 cc 86 92 cf
4b0:  81 84 80 8f a0 a4 cc 86   92 cf 81 84 80 8f a0 a4
4c0:  cc 86 92 cf 81 84 80 8f   a0 a4 cc 86 92 cf 81 84
4d0:  80 8f a0 a4 cc 86 92 cf   81 84 80 8f a0 a4 cc 86
4e0:  92 cf 81 84 80 8f a0 a4   cc 86 92 cf 81 84 80 8f
4f0:  a0 a4 cc 86 92 cf 81 84   80 8f a0 a4 cc 86 92 cf
500:  ff ff ff ff ff ff ff ff   ff ff cf cf cf cf cf cf
510:  cf cf cf cf 92 92 92 92   92 92 92 92 92 92 86 86
520:  86 86 86 86 86 86 86 86   cc cc cc cc cc cc cc cc
530:  cc cc a4 a4 a4 a4 a4 a4   a4 a4 a4 a4 a0 a0 a0 a0
540:  a0 a0 a0 a0 a0 a0 8f 8f   8f 8f 8f 8f 8f 8f 8f 8f
550:  80 80 80 80 80 80 80 80   80 80 84 84 84 84 84 84
560:  84 84 84 84 81 81 81 81   81 81 81 81 81 81 cf cf
570:  cf cf cf cf cf cf cf cf   92 92 92 92 92 92 92 92
580:  92 92 92 92 92 92 92 92   92 cf cf cf cf cf cf cf
590:  cf cf cf 81 81 81 81 81   81 81 81 81 81 84 84 84
5a0:  84 84 84 84 84 84 84 80   80 80 80 80 80 80 80 80
5b0:  80 8f 8f 8f 8f 8f 8f 8f   8f 8f 8f a0 a0 a0 a0 a0
5c0:  a0 a0 a0 a0 a0 a4 a4 a4   a4 a4 a4 a4 a4 a4 a4 cc
5d0:  cc cc cc cc cc cc cc cc   cc 86 86 86 86 86 86 86
5e0:  86 86 86 92 92 92 92 92   92 92 92 92 92 cf cf cf
5f0:  cf cf cf cf cf cf cf ff   ff ff ff ff ff ff ff ff
600:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
610:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
620:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
630:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
640:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
650:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
660:  ff ff ff ff cf cf cf cf   cf cf cf cf cf cf cf cf
670:  cf cf cf cf cf cf cf cf   cf cf cf cf cf cf cf cf
680:  cf cf cf cf cf cf cf cf   cf cf cf cf cf cf cf cf
690:  cf cf cf cf cf cf cf cf   cf cf cf cf cf ff ff ff
6a0:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
6b0:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
6c0:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
6d0:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
6e0:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
6f0:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
700:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
710:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
720:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
730:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
740:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
750:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
760:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
770:  ff ff ff ff ff ff ff ff   ff ff ff ff ff ff ff ff
780:  fe fe fe fe fe fe fe fe   fe fe fe fe fe fe fe fe
790:  fe fe fe fe fe fe fe fe   fe fe fe fe fe fe fe fe
7a0:  fe fe fe fe fe fe fe fe   fe fe fe fe fe fe fe fe
7b0:  fe fe fe fe fe fe fe fe   fe fe fe fe fe fe fe fe
7c0:  fe fe fe fe fe fe fe fe   fe fe fe fe fe fe fe fe
7d0:  fe fe fe fe fe fe fe fe   fe fe fe fe fe fe fe fe
7e0:  fe fe fe fe fe fe fe fe   fe fe fe fe fe fe fe fe
7f0:  fe fe fe fe fe fe fe fe   fe fe fe fe fe fe fe fe



 */





