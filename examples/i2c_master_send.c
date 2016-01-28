
/***********************************************************************************
  Filename:     i2c_master_send.c

  Description:  This example uses a master to send data to a slave using I2C.

  Comments:     To execute this example, use the IAR project i2c_slave_receive
                as the slave. The slave's code must be executed before executing
                the master's code, since the slave is clocked by the Master. The
                bytes sent are simply numbers 0x00 upto BUFFER_SIZE. When bytes
                up to BUFFER_SIZE are sent, the example will end and LED1 will
                be set on the SmartRF05EB.

  Note:         On the SmartRF05EB, P0_6 and P0_7 from CC2543EM is shared by the 
                EM_JOY_LEVEL and EM_LCD_CS signals. These have to be disconnected 
                by removing jumpers 7-8 and 3-4 on P10 I/O Connector. Thus the I2C 
                and SRF05EB LCD can't be used simultaneously.

                The I2C pins also have external pullups on the EM, at the as 
                required by the I2C standard.

***********************************************************************************/

/***********************************************************************************
* INCLUDES
*/

#include <hal_types.h>
// Include Name definitions of individual bits and bit-fields in the CC254x device registers.
#include <ioCC254x_bitdef.h>
// Include device specific file
#if (chip==2541)
#include "ioCC2541.h"
#elif (chip==2543)
#include "ioCC2543.h"
#elif (chip==2545)
#include "ioCC2545.h"
#else
#error "Chip not supported!"
#endif


/***********************************************************************************
* CONSTANTS
*/

// Define size of buffer and number of bytes to send.
#define BUFFER_SIZE 0x01
#define DATA_SIZE 0xff
#define SLAVE_ADDRESS 0x0D    // 7-bit addressing.

#define READ_FROM_SLAVE 0x01
#define WRITE_TO_SLAVE 0x00


/***********************************************************************************
* LOCAL VARIABLES
*/

// Masters's transmit buffer
static uint8 buffer[BUFFER_SIZE+1];
static uint16 bufferIndex = 0;
static uint16 dataIndex = 0;
static uint8 r_w = 0;     // 0=read 1=write
static uint8 check = 0;   // status register
static uint16 count = 0;
static uint8 t = 0;
static uint8 stat = 0;
static uint8 loop[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static uint8 loop2 = 0;
static uint8 rloop = 0;
static uint8 delta = 0;
static uint8 btt = 0x71;
static uint8 a = 0;
static uint8 b = 0;
static uint8 c = 0;
static uint8 d = 0;
static uint8 e = 0;
static uint8 f = 0;
static uint8 g = 0;
static uint16 delay = 200;
static uint16 delay2 = 100;


static uint8 data[DATA_SIZE];
//static uint8 stat2[140] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};




/***********************************************************************************
* LOCAL FUNCTIONS
*/

/***********************************************************************************
* @fn          I2C_ISR
*
* @brief       Function which sends the next I2C data byte.
*
* @param       none
*
* @return      0
*/
#pragma vector = I2C_VECTOR
__interrupt void I2C_ISR(void)
{ 
  if(r_w==1)
  {
    #if(chip==2541)
        // Clear I2C CPU interrupt flag.
        P2IF = 0;
    #elif(chip==2543 || chip==2545)
        // Clear I2C CPU interrupt flag.
        I2CIF = 0;
    #endif
      
        // If a Start or Restart condition has been transmitted ...
        //while((I2CSTAT & 0x08)==0)
        //{
          //stat=I2CSTAT;
        //}
        
        if (I2CSTAT == 0x08 || I2CSTAT == 0x10)
        {
            // Send Slave address and R/W access.
            I2CDATA = (SLAVE_ADDRESS << 1) | WRITE_TO_SLAVE;
           

            // End Start condition.
            I2CCFG &= ~I2CCFG_STA;
            a++;
            loop[delta]++;
            bufferIndex = 0;
        }

        // If finished transmitting ...
        if (bufferIndex > BUFFER_SIZE)
        {
            // Generate Stop condition.
            I2CCFG |= I2CCFG_STO;
            
          
    #if(chip==2541)
        // Disable interrupt from I2C by setting [IEN2.I2CIE = 0].
        IEN2 &= ~IEN2_P2IE;
    #elif(chip==2543 || chip==2545)
        // Disable interrupt from I2C by setting [IEN2.I2CIE = 0].
        IEN2 &= ~IEN2_I2CIE;
    #endif
        b++;
        // Set SRF05EB LED1.
        P1_0 = 1;   
      }

        // If Slave address or Data byte has been transmitted and acknowledged ...
        else if (I2CSTAT == 0x18 || I2CSTAT == 0x28)
        {
            // Send next I2C byte
            I2CDATA = buffer[bufferIndex++];
            //while((I2CSTAT & 0x30)==0)
            //{
              //stat=I2CSTAT;
              //loop2++;
              
            //}
            loop[delta]++;
            c++;
            
        }
       
        // I2CCFG.SI flag must be cleared by software at the end of the ISR.
        I2CCFG &= ~I2CCFG_SI;
        
    //a=I2CCFG;
    
  }
  
  if (r_w==0)
  {
        #if(chip==2541)
        // Clear I2C CPU interrupt flag.
        P2IF = 0;
    #else
        // Clear I2C CPU interrupt flag.
        I2CIF = 0;
    #endif
      
        // If a Start or Restart condition has been transmitted ...
        if (I2CSTAT == 0x08 || I2CSTAT == 0x10)
        {
            // Send Slave address and R/W access.
            I2CDATA = (SLAVE_ADDRESS << 1) | READ_FROM_SLAVE;
            
            //while(I2CSTAT != 0x40);
        //stat2[g++]=I2CSTAT;
            //stat=I2CSTAT;
            // End Start condition.
            I2CCFG &= ~I2CCFG_STA;
            //I2CCFG &= ~I2CCFG_AA;
        }
        //while((I2CSTAT & 0x40)==0);
        //if (I2CSTAT == 0x40)
        //{
        //  I2CCFG &= ~I2CCFG_AA;
        //}

           // stat=I2CSTAT;
        // If a Data byte has been received and acknowledge has been returned...
        else if (I2CSTAT == 0x58)
        {
          //stat2[g++]=I2CSTAT;
            // Read Data byte.
            data[dataIndex++] = I2CDATA;
            t=1;
            //stat2[g++]=I2CSTAT;
            
        }
      
        // If finished receiving...
        if (t==1)
        {
          
            // Generate Stop condition.
            I2CCFG |= I2CCFG_STO;

    #if(chip==2541)
            // Disable interrupt from I2C by setting [IEN2.I2CIE = 0].
            IEN2 &= ~IEN2_P2IE;
    #elif(chip==2543 || chip==2545)
            // Disable interrupt from I2C by setting [IEN2.I2CIE = 0].
            IEN2 &= ~IEN2_I2CIE;
    #endif
        
            // Set SRF05EB LED1.
            P1_0 = 1;  
            t=0;
            //stat2[g++]=I2CSTAT;
            //stat=I2CSTAT;
      }

        // I2CCFG.SI flag must be cleared by software at the end of the ISR.
        I2CCFG &= ~I2CCFG_SI;
        
  }
  if (r_w==2)
  {
        #if(chip==2541)
        // Clear I2C CPU interrupt flag.
        P2IF = 0;
    #else
        // Clear I2C CPU interrupt flag.
        I2CIF = 0;
    #endif
      
        //stat2[g++]=1;
        // If a Start or Restart condition has been transmitted ...
        //while((I2CSTAT & 0x08)==0);
        if (I2CSTAT == 0x08 || I2CSTAT == 0x10)
        
        {//stat2[g++]=I2CSTAT;
            // Send Slave address and R/W access.
            I2CDATA = (SLAVE_ADDRESS << 1) | READ_FROM_SLAVE;
        
            //stat2[g++]=I2CSTAT;
            
            //{
              stat=I2CSTAT;
            //}
            
            stat=I2CSTAT;
            // End Start condition.
            I2CCFG &= ~I2CCFG_STA;
            //I2CCFG &= ~I2CCFG_AA;
            d++;
            stat=I2CSTAT;
            //stat=I2CSTAT;
            
            //stat2[g++]=2;
        }
        
        // If a Data byte has been received and acknowledge has been returned...
        else if (I2CSTAT == 0x58)
        {
          //stat2[g++]=I2CSTAT;
            // Read Data byte.
            check = I2CDATA;
            t=1;
            rloop++;
            e++;
            //stat2[g++]=3;
        }
        
        // If finished receiving...
        
        if (t==1)
        {
          //I2CCFG &= ~I2CCFG_AA;
            // Generate Stop condition.
            I2CCFG |= I2CCFG_STO;

    #if(chip==2541)
            // Disable interrupt from I2C by setting [IEN2.I2CIE = 0].
            IEN2 &= ~IEN2_P2IE;
    #elif(chip==2543 || chip==2545)
            // Disable interrupt from I2C by setting [IEN2.I2CIE = 0].
            IEN2 &= ~IEN2_I2CIE;
    #endif
        
            // Set SRF05EB LED1.
            P1_0 = 1;  
            t=0;
            //stat2[g++]=4;
            stat=I2CSTAT;
            f++;
      }
        
        //else if(I2CSTAT == 0x40)
        //{
          //stat2[g++]=I2CSTAT;
        //while(I2CSTAT != 0x50);
        
        //}
        
        //stat2[g++]=5;

        // I2CCFG.SI flag must be cleared by software at the end of the ISR.
        I2CCFG &= ~I2CCFG_SI;
        //stat2[g++]=6;
        while(0);
  }
  
        
    //a=I2CCFG;
  //while(0);
}


/***********************************************************************************
* @fn          main
*
* @brief       Send data to a single slave using I2C in Master mode
*
* @param       void
*
* @return      0
*/
int main(void)
{
    /****************************************************************************
    * Clock setup
    * See basic software example "clk_xosc_cc254x"
    */
   // Disable the 32 kHz RCOSC calibration
    SLEEPCMD |= OSC32K_CALDIS;
  
    // Set system clock source to HS XOSC, with no pre-scaling.
    CLKCONCMD = (CLKCONCMD & ~(CLKCON_OSC | CLKCON_CLKSPD)) | CLKCON_CLKSPD_32M;
    // Wait until clock source has changed
    while (CLKCONSTA & CLKCON_OSC);
  
    // Note the 32 kHz RCOSC starts calibrating, if not disabled.

#if (chip==2543)
    /***************************************************************************
    * Setup I/O ports
    *
    * Port and pins used by I2C, at the alternative 1 location:
    * I2C SCL:   P0_6    (Debug Connector P18_5)
    * I2C SDA:   P0_7    (Debug Connector P18_3)
    */

    // Configure I2C for location Alternative 1.
    PERCFG &= ~PERCFG_I2CCFG;
  
    // Give priority to I2C over Timer 1 for port 0 pins.
    PPRI &= ~PPRI_PRI1P0;

    // Set P0_6 and P0_7 as peripheral I/O.
    P0SEL |= BIT7 | BIT6;
  
#elif(chip==2545)
    /***************************************************************************
    * Setup I/O ports
    *
    * Port and pins used by I2C, at the alternative 2 location:
    * I2C SCL:   P2_5    (Debug Connector P18_5)
    * I2C SDA:   P2_6    (Debug Connector P18_3)
    */

    // Configure I2C for location Alternative 2.
    PERCFG |= PERCFG_I2CCFG;
   
    // Give priority to I2C over USART0, then Timer3.
    PPRI = (PPRI & ~PPRI_PRI1P1) | PPRI_PRI1P1_I2C;

    // Set P2_5 and P2_6 as peripheral I/O.
    P2SEL |= BIT6 | BIT5;
  
#elif(chip==2541)
    /***************************************************************************
    * Setup I/O ports
    *
    * CC2541 has dedicated I2C ports
    * I2C SCL:   Pin 2   (Debug Connector P18_5)
    * I2C SDA:   Pin 3   (Debug Connector P18_3)
    */
  
    // Enable I2C on CC2541.
    I2CWC &= ~0x80;
#endif
  
    // Configure P1_0 as GPIO output for LED1
    P1SEL &= BIT0;      // GPIO
    P1DIR |= BIT0;      // Output
    P1_0 = 0;           // LED1 off


  /***************************************************************************
  * Setup interrupt
  */

#if(chip==2541)
    // Clear I2C (P2) CPU interrupt flag.
    P2IF = 0;

    // Enable interrupt from I2C by setting [IEN2.P2IE = 1].
    IEN2 |= IEN2_P2IE;
#else
    // Clear I2C CPU interrupt flag.
    I2CIF = 0;

    // Enable interrupt from I2C by setting [IEN2.I2CIE = 1].
    IEN2 |= IEN2_I2CIE;
#endif
  
    // Enable global interrupts.
    EA = 1;
  
  
    /***************************************************************************
    * Configure I2C
    */

    // Fill the buffer with values 0x00++.
    uint8 value = 0x00;
    uint8 i;
    for (i = 0; i < BUFFER_SIZE; i++)
    {
        buffer[i] = value++;
    }

    // Enable the I2C module with 33 kHz clock rate.
    // The STArt bit signals a master.
    //I2CCFG = (I2CCFG & ~I2CCFG_CR) | I2CCFG_CR_DIV_960 | I2CCFG_ENS1 | I2CCFG_STA;
    I2CCFG = (I2CCFG & ~I2CCFG_CR) | I2CCFG_CR_DIV_960;
    
    /*bufferIndex = 0;
    buffer[0] = 0x81;
    buffer[1] = 0x00;   // internal clock
    I2CCFG = I2CCFG_ENS1 | I2CCFG_STA;
    
    for(int j=0;j<1000;j++);
    
    bufferIndex = 0;
    buffer[0] = 0x82;
    buffer[1] = 0x10;
    IEN2 |= IEN2_P2IE;
    I2CCFG = I2CCFG_ENS1 | I2CCFG_STA;
    
    for(int j=0;j<1000;j++);
    
    buffer[0] = 0x83;
    buffer[1] = 0xc6;
    */
    
    //for(int j=0;j<10000;j++);
    
    
    
    

    // set the clock
    //while((I2CCFG&I2CCFG_STO)!=0);
    bufferIndex = 0;
    delta = 0;
    buffer[0] = 0x81;
    buffer[1] = 0x00;   // internal clock
    r_w=1;
    IEN2 |= IEN2_P2IE;
    I2CCFG = I2CCFG_ENS1 | I2CCFG_STA;
    while((I2CCFG&I2CCFG_SI)==0);
    for(int j=0;j<delay2;j++);
    
    bufferIndex = 0;    
    delta = 1;
    buffer[0] = 0x82;
    buffer[1] = 0x10;
    r_w=1;
    IEN2 |= IEN2_P2IE;
    I2CCFG = I2CCFG_ENS1 | I2CCFG_STA;
    while((I2CCFG&I2CCFG_SI)==0);
    for(int j=0;j<delay2;j++);
    
    //while((I2CCFG&I2CCFG_STO)!=0);
    bufferIndex = 0;
    delta = 2;
    buffer[0] = 0x83;
    buffer[1] = 0xc6;
    r_w=1;
    IEN2 |= IEN2_P2IE;
    I2CCFG = I2CCFG_ENS1 | I2CCFG_STA;
    while((I2CCFG&I2CCFG_SI)==0);
    for(int j=0;j<delay2;j++);
    
    //while((I2CCFG&I2CCFG_STO)!=0);
    bufferIndex = 0;    
    delta = 3;
    buffer[0] = 0x84;
    buffer[1] = 0x92;
    r_w=1;
    IEN2 |= IEN2_P2IE;
    I2CCFG = I2CCFG_ENS1 | I2CCFG_STA;
    while((I2CCFG&I2CCFG_SI)==0);
    for(int j=0;j<delay2;j++);
    
    // set increment frequency   .1Hz = 0x3
    //while((I2CCFG&I2CCFG_STO)!=0);
    bufferIndex = 0;
    delta = 4;
    buffer[0] = 0x85;
    buffer[1] = 0x00;
    r_w=1;
    IEN2 |= IEN2_P2IE;
    I2CCFG = I2CCFG_ENS1 | I2CCFG_STA;
    while((I2CCFG&I2CCFG_SI)==0);
    for(int j=0;j<delay2;j++);
    
    bufferIndex = 0;
    delta = 5;
    buffer[0] = 0x86;
    buffer[1] = 0x10;
    r_w=1;
    IEN2 |= IEN2_P2IE;
    I2CCFG = I2CCFG_ENS1 | I2CCFG_STA;
    while((I2CCFG&I2CCFG_SI)==0);
    for(int j=0;j<delay2;j++);
    
    bufferIndex = 0;
    delta = 6;
    buffer[0] = 0x87;
    buffer[1] = 0x00;
    r_w=1;
    IEN2 |= IEN2_P2IE;
    I2CCFG = I2CCFG_ENS1 | I2CCFG_STA;
    while((I2CCFG&I2CCFG_SI)==0);
    for(int j=0;j<delay2;j++);
    
    // set number of increments    50 = 0x32
    bufferIndex = 0;
    delta = 7;
    buffer[0] = 0x88;
    buffer[1] = 0x00;
    r_w=1;
    IEN2 |= IEN2_P2IE;
    I2CCFG = I2CCFG_ENS1 | I2CCFG_STA;
    while((I2CCFG&I2CCFG_SI)==0);
    for(int j=0;j<delay2;j++);
    
    bufferIndex = 0;
    delta = 8;
    buffer[0] = 0x89;
    buffer[1] = 0x32;
    r_w=1;
    IEN2 |= IEN2_P2IE;
    I2CCFG = I2CCFG_ENS1 | I2CCFG_STA;
    while((I2CCFG&I2CCFG_SI)==0);
    for(int j=0;j<delay2;j++);
    
    // set settling time cycles    
    bufferIndex = 0;
    delta = 9;
    buffer[0] = 0x8a;
    buffer[1] = 0x00;
    r_w=1;
    IEN2 |= IEN2_P2IE;
    I2CCFG = I2CCFG_ENS1 | I2CCFG_STA;
    while((I2CCFG&I2CCFG_SI)==0);
    for(int j=0;j<delay2;j++);
    
    bufferIndex = 0;
    delta = 10;
    buffer[0] = 0x8b;
    buffer[1] = 0x10;
    r_w=1;
    IEN2 |= IEN2_P2IE;
    I2CCFG = I2CCFG_ENS1 | I2CCFG_STA;
    while((I2CCFG&I2CCFG_SI)==0);
    for(int j=0;j<delay2;j++);
    
    // standby mode
    bufferIndex = 0;
    delta = 11;
    buffer[0] = 0x80;
    buffer[1] = 0xb1;
    r_w=1;
    IEN2 |= IEN2_P2IE;
    I2CCFG = I2CCFG_ENS1 | I2CCFG_STA;
    while((I2CCFG&I2CCFG_SI)==0);
    for(int j=0;j<delay2;j++);
    
    // initialize with start frequency
    bufferIndex = 0;
    delta = 12;
    buffer[0] = 0x80;
    buffer[1] = 0x11;
    r_w=1;
    IEN2 |= IEN2_P2IE;
    I2CCFG = I2CCFG_ENS1 | I2CCFG_STA;
    while((I2CCFG&I2CCFG_SI)==0);
    for(int j=0;j<delay2;j++);
    
    // start frequency sweep
    bufferIndex = 0;
    delta = 13;
    buffer[0] = 0x80;
    buffer[1] = 0x21;
    r_w=1;
    IEN2 |= IEN2_P2IE;
    I2CCFG = I2CCFG_ENS1 | I2CCFG_STA;
    while((I2CCFG&I2CCFG_SI)==0);
    for(int j=0;j<delay2;j++);
    
    // check D1 of status register of the AD5933 for availibility of real and imaginary value
  while (count<50)
  {
    bufferIndex = 0;
    buffer[0] = 0xb0;
    buffer[1] = 0x8f;
    r_w=1;
    IEN2 |= IEN2_P2IE;
    I2CCFG = I2CCFG_ENS1 | I2CCFG_STA;
    for(int j=0;j<delay;j++);
    while((I2CCFG&I2CCFG_SI)==0);
    
    r_w=2;
    bufferIndex = 0;
    IEN2 |= IEN2_P2IE;
    I2CCFG = I2CCFG_ENS1 | I2CCFG_STA;
    //while((I2CCFG&I2CCFG_SI)==0);
    for(int j=0;j<delay;j++);
    
    if ((check & 0x02)==0x02)
    {
      // if values are availible read the real and imaginary registers on the AD5933
      
      // real1
      bufferIndex = 0;
      buffer[0] = 0xb0;
      buffer[1] = 0x94;
      r_w=1;
      IEN2 |= IEN2_P2IE;
      I2CCFG = I2CCFG_ENS1 | I2CCFG_STA;
      while((I2CCFG&I2CCFG_SI)==0);    
      for(int j=0;j<delay;j++);
    
      r_w=0;
      bufferIndex = 0;
      IEN2 |= IEN2_P2IE;
      I2CCFG = I2CCFG_ENS1 | I2CCFG_STA;
      //while((I2CCFG&I2CCFG_SI)==0);
      for(int j=0;j<delay;j++);
      
        
      // real2
      bufferIndex = 0;
      buffer[0] = 0xb0;
      buffer[1] = 0x95;
      r_w=1;
      IEN2 |= IEN2_P2IE;
      I2CCFG = I2CCFG_ENS1 | I2CCFG_STA;
      while((I2CCFG&I2CCFG_SI)==0);         
      for(int j=0;j<delay;j++);
    
      r_w=0;
      bufferIndex = 0;
      IEN2 |= IEN2_P2IE;
      I2CCFG = I2CCFG_ENS1 | I2CCFG_STA;
      //while((I2CCFG&I2CCFG_SI)==0);
      for(int j=0;j<delay;j++);
      
        
      // imaginary1
      bufferIndex = 0;
      buffer[0] = 0xb0;
      buffer[1] = 0x96;
      r_w=1;
      IEN2 |= IEN2_P2IE;
      I2CCFG = I2CCFG_ENS1 | I2CCFG_STA;
      while((I2CCFG&I2CCFG_SI)==0);    
      for(int j=0;j<delay;j++);    
      
      r_w=0;
      bufferIndex = 0;
      IEN2 |= IEN2_P2IE;
      I2CCFG = I2CCFG_ENS1 | I2CCFG_STA;
      //while((I2CCFG&I2CCFG_SI)==0);
      for(int j=0;j<delay;j++);      
        
      
      // imaginary2
      bufferIndex = 0;
      buffer[0] = 0xb0;
      buffer[1] = 0x97;
      r_w=1;
      IEN2 |= IEN2_P2IE;
      I2CCFG = I2CCFG_ENS1 | I2CCFG_STA;
      while((I2CCFG&I2CCFG_SI)==0);    
      for(int j=0;j<delay;j++);
    
      r_w=0;
      bufferIndex = 0;
      IEN2 |= IEN2_P2IE;
      I2CCFG = I2CCFG_ENS1 | I2CCFG_STA;
      //while((I2CCFG&I2CCFG_SI)==0);
      for(int j=0;j<delay;j++);
                    
      count++;
      check = 0x00;
      
      
      // increment frequency
      bufferIndex = 0;
      buffer[0] = 0x80;
      buffer[1] = 0x31;
      r_w=1;
      IEN2 |= IEN2_P2IE;
      I2CCFG = I2CCFG_ENS1 | I2CCFG_STA;
      while((I2CCFG&I2CCFG_SI)==0);
      for(int j=0;j<delay;j++);
    
    }
  }
    
    

    /* Main Loop, the transfers are interrupt handled. */
    while(1);
}


/***********************************************************************************
  Copyright 2012 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
***********************************************************************************/