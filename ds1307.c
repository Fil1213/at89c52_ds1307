  #include <REGX52.H>
  #include <stdio.H>
  
	#define MSB 1
	#define LSB 0
	#define NACK 1
	#define ACK 0
	#define I2C_DEL 5
	#define I2C_OK 0
	#define I2C_ERROR 1
	
	#define GetRHumidityCmd 0
  #define GetTempCmd 1

 sbit SDA = P0 ^ 1;
 sbit SCL = P0 ^ 0;
 	
	void delay_us(unsigned char i)
	{
	 while(i != 0)i--;
	}

	void delay_mc(unsigned int i)
  {
  while(i != 0)i--;
  }

	bit i2c_start(void)
	{
	 SDA = 0;
	 delay_us(I2C_DEL);
	 SCL = 0;
	 if(SDA == SCL)
		return I2C_OK;
	 else 
		return I2C_ERROR;
	}

	 bit i2c_stop(void)
	 {
		SDA = 0;
		delay_us(I2C_DEL);
		SCL = 1;
		delay_us(I2C_DEL);
		SDA = 1;
		if(SDA == SCL)
		 return I2C_OK;
		else 
		 return I2C_ERROR;
	 }

	 bit i2c_write_byte(unsigned char Byte)
	 {
		bit status;
		unsigned char i;
		 for (i = 0; i < 8; i++)
		 {
			 if(Byte & 0x80)
	      SDA = 1;
			 else 
				SDA = 0; 
			 Byte <<= 1;
			 delay_us(I2C_DEL);
			 SCL = 1;
			 delay_us(I2C_DEL);
			 SCL = 0;
			}
		delay_us(I2C_DEL);
		while(SDA);
		status = SDA;
		SCL = 1;
		delay_us(I2C_DEL);
		SCL = 0;
		return status;
	 } 

	unsigned char i2c_read_byte(bit nack_ack,bit msb_lsb)
	{
	 unsigned char i,temp;	
	 for (i = 0; i < 8; i++) 
	 { 
		SCL = 1;
		delay_us(I2C_DEL);
		
		if(msb_lsb)
		{
		 if(SDA)
			temp |= 0x01;
		 else 
			temp |= 0x00;
		 temp <<= 1;
		}
		else 
		{
		 if(SDA)
			temp |= 0x80;
		 else
			temp |= 0x00;
		 temp >>= 1;
		}
		SCL = 0;
		delay_us(I2C_DEL);
	 }
	 if(nack_ack)
		SDA = 1;
	 else
	  SDA = 0;
	 SCL = 1;
	 delay_us(I2C_DEL);
	 SCL = 0;
	 delay_us(I2C_DEL);
	 SDA = 1;
	 return temp;
	}

void preset_UART(void){
	
	//i8051 11.0592 9600
	SCON = 0x50;
	TH1 = 0xFD;
	TMOD |= 0x20;
	TR1 = 1;
	TI = 1;
	EA = 1;
}
	
//---------------------- Binary to bcd -------------------///
	
  unsigned char Dec_To_Bcd(unsigned char dec)
  {
  return ((((dec / 10) << 4) | (dec % 10)) << 1);
  }
 

//---------------------- Bcd to binary -------------------///

   unsigned char Bcd_To_Dec(unsigned char bcd){
	return (10 * (((bcd >>1) & 0xF0) >> 4) + ((bcd >> 1) & 0x0F));
}

   unsigned char fromDecimalToBCD(unsigned char decimalValue) {
   return ((decimalValue / 10) * 16) + (decimalValue % 10);
		 
  }

  unsigned char fromBCDToDecimal(unsigned char BCDValue) {
  return ((BCDValue / 16) * 10) + (BCDValue % 16);
   }


	void main(void){
		unsigned char temp[7] = {0};
		unsigned char i;	
		preset_UART();
		
		i2c_start();
		i2c_write_byte(0xD0);
		i2c_write_byte(0x00);
		i2c_stop();
		
		while(1){
			
		i2c_start();
		i2c_write_byte(0xD0);
		i2c_write_byte(0x00);
		i2c_stop();
			
		i2c_start();
	  i2c_write_byte(0xD1);
			
		temp[0] =	Bcd_To_Dec(i2c_read_byte(0,1));
		temp[1] =	Bcd_To_Dec(i2c_read_byte(0,1));	
	  temp[2] =	Bcd_To_Dec(i2c_read_byte(0,1));
		temp[3] =	Bcd_To_Dec(i2c_read_byte(0,1));	
		temp[4] =	Bcd_To_Dec(i2c_read_byte(0,1));	
		temp[5] =	Bcd_To_Dec(i2c_read_byte(0,1));
    temp[6] =	Bcd_To_Dec(i2c_read_byte(1,1));			
		i2c_stop();
	
	  printf("/----------------------------/\n");	
		printf("Time = %u : %u : %u\n",(unsigned int)temp[2],(unsigned int)temp[1],(unsigned int)temp[0]);
		printf("Date = %u / %u / %u \n",(unsigned int)temp[4],(unsigned int)temp[5],(unsigned int)temp[6]);	
		delay_mc(50000);
    delay_mc(50000);			
    
		}
	}