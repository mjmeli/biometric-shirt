//internal osc of 16.776 MHz

// LCD module connections
sbit LCD_RS at PORTC3_bit;
sbit LCD_EN at PORTC2_bit;
sbit LCD_D4 at PORTC4_bit;
sbit LCD_D5 at PORTC5_bit;
sbit LCD_D6 at PORTC6_bit;
sbit LCD_D7 at PORTC7_bit;

sbit LCD_RS_Direction at DDC3_bit;
sbit LCD_EN_Direction at DDC2_bit;
sbit LCD_D4_Direction at DDC4_bit;
sbit LCD_D5_Direction at DDC5_bit;
sbit LCD_D6_Direction at DDC6_bit;
sbit LCD_D7_Direction at DDC7_bit;
// End LCD module connections
#define SUCCESS 0xff        //Flag

//Command
#define Start 0xa4
#define Stop 0x94
#define Trans 0x84
#define ACK 0xc4

//I2C bus Status
#define START 0x08
#define ReSTART 0x10
#define SLA_W 0x1a
#define SLA_R 0x1b
#define MT_SLA_ACK 0x18
#define MT_SLA_NACK 0x20
#define MR_SLA_ACK 0x40
#define MR_SLA_NACK 0x48
#define MT_DATA_ACK 0x28
#define MT_DATA_NACK 0x30
#define MR_DATA_ACK 0x50
#define MR_DATA_NACK 0x58
#define TWINT 0x80

//AD5933 control codes
#define Init 0x10        //Initialize with start Freq
#define Sweep 0x20        //Start Frequency Sweep
#define IncFreq 0x30        //Increment Frequency
#define RepFreq 0x40        //Repeat Frequency
#define MeaTemp 0x90        //Measure Temperature
#define PowerDown 0xa0        //Power down mode
#define Standby 0xb0        //Standby mode
#define Range2V 0x00        //Output Voltage range 2V
#define Range1V 0x06        //Output Voltage range 1V
#define Range400mV 0x04 //Output Voltage range 400mV
#define Range200mV 0x02 //Output Voltage range 200mV
#define gainx5 0x00        //PGA gain x5
#define gainx1 0x01        //PGA gain x1

//AD5933 Register addresses
#define Control_high 0x80
#define Control_low 0x81
#define Freq_high 0x82
#define Freq_mid 0x83
#define Freq_low 0x84
#define FreqInc_high 0x85
#define FreqInc_mid 0x86
#define FreqInc_low 0x87
#define NumInc_high 0x88
#define NumInc_low 0x89
#define NumSettle_high 0x8a
#define NumSettle_low 0x8b
#define Status 0x8f
#define Temp_high 0x92
#define Temp_low 0x93
#define Real_high 0x94
#define Real_low 0x95
#define Imag_high 0x96
#define Imag_low 0x97

unsigned char Init_TWI(void)
{
    TWBR = 0x18;            //Set baud-rate to 250 KHz at 16 MHz xtal
    TWCR = 0x04;                //Enable TWI-interface
    return 1;
}

void Wait_TWI_int(void)
{
    while(!(TWCR & TWINT));
}

unsigned char Send_start(void)
{
    TWCR=Start;                //Send START

    Wait_TWI_int();                //Wait for TWI interrupt flag to be set

    if((TWSR & 0xF8)!=START || (TWSR & 0xF8)!=ReSTART)
    return TWSR;        //If it failed, return the TWSR value
    return SUCCESS;                //If succeeded, return SUCCESS
}

void Send_stop(void)
{
    TWCR = Stop;                //Send a STOP condition
}

unsigned char Send_adr(unsigned char adr)
{
    Wait_TWI_int();                //Wait for TWI interrupt flag set

    TWDR = adr;
    TWCR = Trans;           //Clear int flag to send byte

    Wait_TWI_int();                //Wait for TWI interrupt flag set

    if((TWSR & 0xF8)!= MT_SLA_ACK || (TWSR & 0xF8)!= MR_SLA_ACK)
    return TWSR;        //If NACK received return TWSR

    return SUCCESS;                //Else return SUCCESS
}

unsigned char Send_byte(unsigned char ddata)
{
    Wait_TWI_int();                //Wait for TWI interrupt flag set

    TWDR = ddata;
    TWCR = Trans;           //Clear int flag to send byte

    Wait_TWI_int();                //Wait for TWI interrupt flag set

    if((TWSR & 0xF8)!= MT_DATA_ACK)
    return TWSR;        //If NACK received return TWSR
    return SUCCESS;                //Else return SUCCESS
}

unsigned char Set_pointer(unsigned char reg_loc)
{
    Send_start();
    Send_adr(SLA_W);
    Send_byte(0xb0);        //Pointer command code '1011 0000'
    Send_byte(reg_loc);        //a register location at which the pointer points
    return 1;
}

unsigned char Byte_write(unsigned char reg_addr, unsigned char ddata)
{
    Send_start();
    Send_adr(SLA_W);
    Send_byte(reg_addr);
    Send_byte(ddata);
    Send_stop();
    return 1;
}

unsigned char Block_write(unsigned char reg_loc, unsigned char byte_num, unsigned char* data_p)
{
    unsigned char i;

    Set_pointer(reg_loc);        //set the pointer location
    Send_start();                //write the data block
    Send_adr(SLA_W);
    Send_byte(0xa0);        //Block write command code '1010 0000'
    Send_byte(byte_num);        //Num of data to be sent

    for(i = 0;i < byte_num;i++)  //Send the data bytes
    {
        Send_byte(*(data_p+i));
    }
    Send_stop();
    return 1;
}

unsigned char Byte_read(unsigned char reg_loc)
{
    Set_pointer(reg_loc);        //set the pointer location

    //Receive a byte
    Send_start();
    Send_adr(SLA_R);
    TWCR = Trans;
    Wait_TWI_int();         //Wait for TWI interrupt flag set
    return TWDR;
}

unsigned char Block_read(unsigned char reg_loc, unsigned char byte_num)
{
    unsigned char i;
    unsigned char* data_p;

    Set_pointer(reg_loc);        //set the pointer location

    //write the data block
    Send_start();
    Send_adr(SLA_W);
    Send_byte(0xa1);        //Block read command code '1010 0001'
    Send_byte(byte_num);        //Num of data to be received

    Send_start();
    Send_adr(SLA_R);
    for(i = 0;i < byte_num;i++)        //Receive all the bytes
    {
        TWCR = ACK;                //Clear int flag and enable acknowledge to receive data.
        Wait_TWI_int();                //Wait for TWI interrupt flag set
        *(data_p+i)=TWDR;
    }

    TWCR = Trans;
    Wait_TWI_int();                        //Wait for TWI interrupt flag set

    *(data_p+i)=TWDR;                //Save Last byte

    Send_stop();
    return data_p;
}

unsigned long int Data_proc(unsigned char data_high, unsigned char data_low)
{
    unsigned long int ddata;
    ddata=(unsigned long int)data_high*256+data_low;
    if(ddata > 0x7fff)
    {
        ddata=0x10000-ddata;
    }
    return ddata;
}

void Display(unsigned long int real, unsigned long int imag, float Res)
{
    char txt1[12];
    char txt2[12];
    char txt3[15];

    lcd_cmd(_lcd_clear);
    lcd_out(1, 1, "R=");
    lcd_out(2, 1, "I=");
    lcd_out(3, 1, "Z=");
    longtostr(real, txt1);
    longtostr(imag, txt2);
    floattostr(Res, txt3);
    lcd_out(1, 3, txt1);
    lcd_out(2, 3, txt2);
    lcd_out(3, 3, txt3);
}

void main(void)
{
    unsigned char a = 0x00, b = 0x00;
    unsigned long int R = 0x00000000, I = 0x00000000;
    float Z = 0.0, GF = 1.0, Result = 0.0;

    //start of program
    Lcd_Init();                        // Initialize LCD
    delay_ms(100);
    Lcd_Cmd(_LCD_CLEAR);               // Clear display
    Lcd_Cmd(_LCD_CURSOR_OFF);          // Cursor off
    Init_TWI();
    lcd_out(4,1,"INIT DONE...");
    delay_ms(750);
    lcd_out(4,1,"CALIBRATING...");
    delay_ms(1000);
    //start frequency register - 50 kHz
    Byte_write ( 0x84, 0x99);
    Byte_write ( 0x83, 0x99);
    Byte_write ( 0x82, 0x19);
    //frequency increment register - 1 kHz
    Byte_write ( 0x87, 0x02);
    Byte_write ( 0x86, 0x7D);
    Byte_write ( 0x85, 0x00);
    //number of increments
    Byte_write ( 0x89, 0x0A);
    Byte_write ( 0x88, 0x00);
    //settling time cycles register - 15
    Byte_write ( 0x8B, 0x0F);
    Byte_write ( 0x8A, 0x00);

    //CONTROL register
    //AD5933 in standby mode
    Byte_write ( 0x80, 0xB0);
    //internal clock
    Byte_write ( 0x81, 0x00);
    //range 1 (1vp-p, 1.6v) PGA = x1
    Byte_write  ( 0x80, 0x01);
    Byte_write(Control_high, 0x10);
    Byte_write(Control_high, 0x21); //start sweep

    while(!(Byte_read(Status) & 0x02));
    //real data
    a = 0x00; b = 0x00;
    a = Byte_read(Real_high);
    b = Byte_read(Real_low);
    R = Data_proc(a, b);
    //Imag data
    a = 0x00; b = 0x00;
    a = Byte_read(Imag_high);
    b = Byte_read(Imag_low);
    I = Data_proc(a, b);
    Z = sqrt(R*R+I*I);
    GF = 1.0/(Z*22000.0);
    while(1)
    {
        lcd_out(4,1,"MEASURING... ");
        R = 0x00000000; I = 0x00000000;
        delay_ms(750);
        //start frequency register - 50 kHz
        Byte_write ( 0x84, 0x99);
        Byte_write ( 0x83, 0x99);
        Byte_write ( 0x82, 0x19);
        //frequency increment register - 1 kHz
        Byte_write ( 0x87, 0x02);
        Byte_write ( 0x86, 0x7D);
        Byte_write ( 0x85, 0x00);
        //number of increments
        Byte_write ( 0x89, 0x0A);
        Byte_write ( 0x88, 0x00);
        //settling time cycles register - 15
        Byte_write ( 0x8B, 0x0F);
        Byte_write ( 0x8A, 0x00);
        //CONTROL register
        //AD5933 in standby mode
        Byte_write ( 0x80, 0xB0);
        //internal clock
        Byte_write ( 0x81, 0x00);
        //range 1 (2vp-p, 1.6v) PGA = x1
        Byte_write ( 0x80, 0x01);
        Byte_write(Control_high, 0x10);
        Byte_write(Control_high, 0x21); //start sweep
        while(!(Byte_read(Status) & 0x02));
        //Initialize
        Byte_write(Control_high, 0x10);
        Byte_write(Control_high, 0x21); //start sweep
        while(!(Byte_read(Status) & 0x02));
        a = 0x00; b = 0x00;
        //real data
        a = Byte_read(Real_high);
        b = Byte_read(Real_low);
        R = Data_proc(a, b);
        a = 0x00; b = 0x00;
        //Imag data
        a = Byte_read(Imag_high);
        b = Byte_read(Imag_low);
        I = Data_proc(a, b);
        Z = sqrt(R*R+I*I);
        Result = 1.0/(Z*GF);
        Display(R, I, Result);
        delay_ms(3000);
    }
}
