/*
  Touchmpr121.cpp - Library for using mpr121 breakout board from sparkfun
  Created by Narendra N Aswathanarayana Setty, Feb 17, 2012.  
*/
#include "Arduino.h"
#include "Touchmpr121.h"

DetectTouch::DetectTouch(int irqpin)
{
  pinMode(irqpin, INPUT);
  _irqpin = irqpin;
}

void DetectTouch::DetectTouch_init(int TOU, int REL, int i2c_add)
{
  DDRD |= 0b00010011;
  PORTD = 0b00110000;
  i2c_address = i2c_add;
  i2cInit();
  mpr121QuickConfig();
  TOU_THRESH=TOU;
  REL_THRESH=REL;
  delay(300);
}

byte DetectTouch::mpr121Read(uint8_t address)
{
  byte data;
  
  i2cSendStart();
  i2cWaitForComplete();
  
  i2cSendByte(i2c_address);	// write 0xB4
  i2cWaitForComplete();
  
  i2cSendByte(address);	// write register address
  i2cWaitForComplete();
  
  i2cSendStart();
  
  i2cSendByte(i2c_address+1);	// write 0xB5
  i2cWaitForComplete();
  i2cReceiveByte(TRUE);
  i2cWaitForComplete();
  
  data = i2cGetReceivedByte();	// Get MSB result
  i2cWaitForComplete();
  i2cSendStop();
  
  cbi(TWCR, TWEN);	// Disable TWI
  sbi(TWCR, TWEN);	// Enable TWI
  
  return data;
}

void DetectTouch::mpr121Write(unsigned char address, unsigned char data)
{
  i2cSendStart();
  i2cWaitForComplete();
  
  i2cSendByte(i2c_address);// write 0xB4
  i2cWaitForComplete();
  
  i2cSendByte(address);	// write register address
  i2cWaitForComplete();
  
  i2cSendByte(data);
  i2cWaitForComplete();
  
  i2cSendStop();
}

void DetectTouch::mpr121QuickConfig(void)
{
  mpr121Write(ELE_CFG, 0x00);
  //mpr121Write(0x5c, 0x10);
 // mpr121Write(FIL_CFG, 0x24);
  
  // Section A
  // This group controls filtering when data is > baseline.
  mpr121Write(MHD_R, 0x01);
  mpr121Write(NHD_R, 0x01);
  mpr121Write(NCL_R, 0x00);
  mpr121Write(FDL_R, 0x00);

  // Section B
  // This group controls filtering when data is < baseline.
  mpr121Write(MHD_F, 0x01);
  mpr121Write(NHD_F, 0x01);
  mpr121Write(NCL_F, 0xFF);
  mpr121Write(FDL_F, 0x02);
  
  // Section C
  // This group sets touch and release thresholds for each electrode
  mpr121Write(ELE0_T, TOU_THRESH);
  mpr121Write(ELE0_R, REL_THRESH);
  mpr121Write(ELE1_T, TOU_THRESH);
  mpr121Write(ELE1_R, REL_THRESH);
  mpr121Write(ELE2_T, TOU_THRESH);
  mpr121Write(ELE2_R, REL_THRESH);
  mpr121Write(ELE3_T, TOU_THRESH);
  mpr121Write(ELE3_R, REL_THRESH);
  mpr121Write(ELE4_T, TOU_THRESH);
  mpr121Write(ELE4_R, REL_THRESH);
  mpr121Write(ELE5_T, TOU_THRESH);
  mpr121Write(ELE5_R, REL_THRESH);
  mpr121Write(ELE6_T, TOU_THRESH);
  mpr121Write(ELE6_R, REL_THRESH);
  mpr121Write(ELE7_T, TOU_THRESH);
  mpr121Write(ELE7_R, REL_THRESH);
  mpr121Write(ELE8_T, TOU_THRESH);
  mpr121Write(ELE8_R, REL_THRESH);
  mpr121Write(ELE9_T, TOU_THRESH);
  mpr121Write(ELE9_R, REL_THRESH);
  mpr121Write(ELE10_T, TOU_THRESH);
  mpr121Write(ELE10_R, REL_THRESH);
  mpr121Write(ELE11_T, TOU_THRESH);
  mpr121Write(ELE11_R, REL_THRESH);
  
  mpr121Write(MHDPROXR, 0xff);
  mpr121Write(NHDPROXR, 0xff);
  mpr121Write(NCLPROXR, 0x00);
  mpr121Write(FDLPROXR, 0x00);
  mpr121Write(MHDPROXF, 0x01);
  mpr121Write(NHDPROXF, 0x01);
  mpr121Write(NCLPROXF, 0xff);
  mpr121Write(FDLPROXF, 0xff);
  mpr121Write(NHDPROXT, 0x00);
  mpr121Write(NCLPROXT, 0x00);
  mpr121Write(FDLPROXT, 0x00);
  // Section D
  // Set the Filter Configuration
  // Set ESI2
  mpr121Write(FIL_CFG, 0xb3);
  
  // Section E
  // Electrode Configuration
  // Enable 6 Electrodes and set tELE_CFG to 0x00 to return to standby mode
  mpr121Write(ELE_CFG, 0x0C);	// Enables all 12 Electrodes
  //mpr121Write(ELE_CFG, 0x06);		// Enable first 6 electrodes
  
  // Section F
  // Enable Auto Config and auto Reconfig
  //mpr121Write(ATO_CFG0, 0x0B);
  //mpr121Write(ATO_CFGU, 0xCA);	// USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V   mpr121Write(ATO_CFGL, 0x82);	// LSL = 0.65*USL = 0x82 @3.3V
  //mpr121Write(ATO_CFGT, 0xB6);	// Target = 0.9*USL = 0xB5 @3.3V
}

int DetectTouch::whichkey()
{
  touchstatus = mpr121Read(0x01) << 8;
  touchstatus |= mpr121Read(0x00);
  n_key=0;
 for (i=0; i<12; i++)
 {
   if (touchstatus & (1<<i))
   {
     n_key++;
     keypress=1<<i;
   }
 }
 if (n_key == 1)
 {
   Serial.print("You have touched a sensor");
   switch (keypress)
   {
     case 1:
     Serial.println(":0");
     key = 0;
     break;
     case 2:
     Serial.println(":1");
     key = 1;
     break;
     case 4:
     Serial.println(":2");
     key = 2;
     break;
     case 8:
     Serial.println(":3");
     key = 3;
     break;
     case 16:
     Serial.println(":4");
     key = 4;
     break;
     case 32:
     Serial.println(":5");
     key = 5;
     break;
     case 64:
     Serial.println(":6");
     key = 6;
     break;
     case 128:
     Serial.println(":7");
     key = 7;
     break;
     case 256:
     Serial.println(":8");
     key = 8;
     break;
     case 512:
     Serial.println(":9");
     key = 9;
     break;
     case 1024:
     Serial.println("10");
     key = 10;
     break;
     case 2048:
     Serial.println(":11");
     key = 11;
     break;
   }
 }
 else
 {
   Serial.println("Press only single sensor");
   key = -1;
 }
 return key;      
}

int DetectTouch::checkInterrupt()
{
  if(digitalRead(_irqpin) == HIGH)
  {
  return -1;
  }
  key = whichkey();
  return key;    
}

/*********************
 ****I2C Functions****
 *********************/

void DetectTouch::i2cInit(void)
{
	// set i2c bit rate to 40KHz
	i2cSetBitrate(100);
	// enable TWI (two-wire interface)
	sbi(TWCR, TWEN);	// Enable TWI
}

void DetectTouch::i2cSetBitrate(unsigned short bitrateKHz)
{
	unsigned char bitrate_div;
	// set i2c bitrate
	// SCL freq = F_CPU/(16+2*TWBR))
	cbi(TWSR, TWPS0);
	cbi(TWSR, TWPS1);
	
	//calculate bitrate division	
	bitrate_div = ((F_CPU/4000l)/bitrateKHz);
	if(bitrate_div >= 16)
		bitrate_div = (bitrate_div-16)/2;
	outb(TWBR, bitrate_div);
}

void DetectTouch::i2cSendStart(void)
{
	WRITE_sda();
	// send start condition
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
}

void DetectTouch::i2cSendStop(void)
{
	// transmit stop condition
        TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}

void DetectTouch::i2cWaitForComplete(void)
{
	int i = 0;		//time out variable
	
	// wait for i2c interface to complete operation
    while ((!(TWCR & (1<<TWINT))) && (i < 90))
		i++;
}

void DetectTouch::i2cSendByte(unsigned char data)
{
	delay_ms(1);
	//printf("sending 0x%x\n", data);
	WRITE_sda();
	// save data to the TWDR
	TWDR = data;
	// begin send
	TWCR = (1<<TWINT)|(1<<TWEN);
}

void DetectTouch::i2cReceiveByte(unsigned char ackFlag)
{
	// begin receive over i2c
	if( ackFlag )
	{
		// ackFlag = TRUE: ACK the recevied data
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
	}
	else
	{
		// ackFlag = FALSE: NACK the recevied data
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
	}
}

unsigned char DetectTouch::i2cGetReceivedByte(void)
{
	// retieve received data byte from i2c TWDR
	return( inb(TWDR) );
}

unsigned char DetectTouch::i2cGetStatus(void)
{
	// retieve current i2c status from i2c TWSR
	return( inb(TWSR) );
}

void DetectTouch::delay_ms(uint16_t x)
{
  uint8_t y, z;
  for ( ; x > 0 ; x--){
    for ( y = 0 ; y < 90 ; y++){
      for ( z = 0 ; z < 6 ; z++){
        asm volatile ("nop");
      }
    }
  }
}
