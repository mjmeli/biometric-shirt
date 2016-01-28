/
***************************************************************************

Author        : sean brennan dac applications

Date          : APRIL. 2006

File          : AD5933_I2C_Master.c

Hardware      : Applicable to ADuC702x rev H or I silicon
Currently targetting ADuC7026.

Description   : I2C master	to demonstrate with I2C_Slave.c

Operates in two modes, read & write (called recieve and
    transmit here). At the begining of an I2C transmission, the
    Master sends an address. The LSB of this address determines
    if the Master is to read (1) or write (0).

    This code also demonstrates basic UART functionality.
    The baudrate is calculated with the following formula:

    DL = HCLK
    _______
    Baudrate * 2 *16

    ***************************************************************************/

    #include<ADuC7020.h>				  // 	Include ADuC7020 Header File
    #include"stdio.h"                     // include standard i/o Header file


    void IRQ_Handler(void) __irq;    //	IRQ Funtion Prototype
    void delay(int);
    void i2c_write ( unsigned int reg_address, unsigned int reg_write_data);
    int AD5933_read (unsigned int register_address);
    void i2c_read(void);
    char hexidecimal_ascii(char toconv);   //hex to ascii conversion
    void send_data_uart(short to_send);  // sends data up UART
    extern int write (char * ptr, int len);   // Functions used to
    extern int getchar (void);           // to output data
    extern int putchar (int);                // Write character to Serial Port
    void Measure_Temp (void);
    void sweep (void);
    void convert (short bin, char *str); // converts real/imag (2's complement to hex/decimal)


    #define count 0x0;					// Number of bytes to be recieved - 1
    #define address_pointer_command 0xB0;



    int i = 0, Receive_byte[1];              // Size of receive_byte should be (count + 1)
    char a;
    short int status_register ;
    unsigned char jchar = 0x30;
    unsigned int real_data;
    unsigned int imag_data;
    unsigned char binary[16];// array used to convert 2's complement to binary


    int main(void)
    {

        IRQEN = PLA_IRQ0_BIT+SM_MASTER1_BIT ;             // Enable i2c in IRQEnable
        GP4DAT = 0x04040000;            // Configure P4.2 as output
        GP3DAT = 0xff000000;            // Configure port 3 as output
        GP1CON = 0x2211;                // I2C on P1.2 and P1.3
        I2C1CFG = 0x82;                 // Master Enable & Enable Generation of Master Clock

        // I2C-Master setup
        I2C1DIV = 0xcfcf;              // 0x3232 = 400kHz
        // 0xCFCF = 100kHz

        // UART set up at 9600bps
        // Start setting up UART at 9600bps
        COMCON0 = 0x080;           // Setting DLAB
        COMDIV0 = 0x088;           // Setting DIV0 and DIV1 to DL calculated
        COMDIV1 = 0x000;
        COMCON0 = 0x007;           // Clearing DLAB

        // Transmit	to start frequency register
        // program 30khz start frequency assuming internal osc of 16.776Khz
        i2c_write ( 0x84, 0x45);
        i2c_write ( 0x83, 0xA6);
        i2c_write ( 0x82, 0x0E);

        // Transmit to frequency increment register
        // program 1Khz frequency increment assuming internal osc of 16.776Khz
        i2c_write ( 0x87, 0x02);
        i2c_write ( 0x86, 0x7D);
        i2c_write ( 0x85, 0x00);

        // Transmit to NUMBER OF INCREMENTS register
        // program 10 frequency increments
        i2c_write ( 0x89, 0x0A);
        i2c_write ( 0x88, 0x00);

        // Transmit to settling time cycles register
        // program 15 output cycles at each frequency before a adc conversion
        i2c_write ( 0x8B, 0x0F);
        i2c_write ( 0x8A, 0x00);


        // Transmit to CONTROL register
        // place the AD5933 in standby mode
        i2c_write ( 0x80, 0xB0);

        // Choose the internal system clock
        i2c_write ( 0x81, 0x00);

        // Choose range 1 (2vp-p, 1.6v) PGA = x1
        i2c_write ( 0x80, 0x01);

        // initialise the sensor with contents of start frequency regsister with range 1 (2vp-p, 1.6v) PGA = x1
        i2c_write ( 0x80, 0x10);
        // start of frequency sweep  (2vp-p, 1.6v) PGA = x1
        i2c_write ( 0x80, 0x20);
        // initialise the sweep sequence
        sweep ();

        //issue a measure temperature command to the control register
        i2c_write ( 0x80, 0x90);
        // read the temperature data and send to uart
        Measure_Temp();



        while(1)
        {
            // do nothing
        };

        return 0;
    }





    /////////////////////////////////////// function's///////////////////////////////////////////
    void delay (int length)
    {
        while(length >0)
        length--;
    }

    void i2c_write ( unsigned int reg_address, unsigned int reg_write_data)
    {

        I2C1MTX =   reg_address;   //  register address.send i2c byte address
        I2C1MTX =   reg_write_data;      // register data. send i2c byte address
        I2C1ADR =   0x1A;       //  device address. set i2c address(LSB = 0, Master Write)
        delay(4000);  // nominal delay
        return;
    }

    int AD5933_read (unsigned int register_address)
    {
        i2c_write(0xB0,register_address);    // set the address pointer
        i2c_read();
        return(Receive_byte[0]);
    }

    void i2c_read(void)
    {
        i = 0;
        I2C1CNT =   count;            // Number of bytes to be read from slave
        I2C1ADR =  0x1B;       //  device address. set i2c address(LSB = 1, Master Read)
        delay(4000);
        return;
    }

    void send_data_uart(short to_send)  // sends data up UART
    {
        //while(!(0x020==(COMSTA0 & 0x020))){}
        //	COMTX = 0x0A;						// output Line feed
        //while(!(0x020==(COMSTA0 & 0x020))){}
        //	COMTX = 0x0D;						// output Carrage return
        while(!(0x020==(COMSTA0 & 0x020)))
        {
        }
        COMTX = (hexidecimal_ascii ((to_send >> 12) & 0x0F));
        while(!(0x020==(COMSTA0 & 0x020)))
        {
        }
        COMTX = (hexidecimal_ascii ((to_send >> 8) & 0x0F));
        while(!(0x020==(COMSTA0 & 0x020)))
        {
        }
        COMTX = hexidecimal_ascii ((to_send >> 4) & 0x0F);
        while(!(0x020==(COMSTA0 & 0x020)))
        {
        }
        COMTX = hexidecimal_ascii(to_send & 0x0F);
        while(!(0x020==(COMSTA0 & 0x020)))
        {
        }
        COMTX = 0x0A;                 // output Line feed
        while(!(0x020==(COMSTA0 & 0x020)))
        {
        }
        COMTX = 0x0D;                 // output Carrage return
        return;
    }
    char hexidecimal_ascii(char toconv)   //hex to ascii conversion
    {
        if(toconv<0x0A)
        {
            toconv += 0x30;
        } else
        {
            toconv += 0x37;
        }

        return(toconv);
    }


    void sweep (void)
    {
        unsigned int real_byte_high;
        unsigned int real_byte_low;

        unsigned int imag_byte_high;
        unsigned int imag_byte_low;


        signed short int imag_data;
        signed short int real_data;

        //write(output3,25); // output "start sweep" to uart
        printf ("Start of Frequency sweep\n");       // printf function call

        for(;;)
        // status reg D0 = valid temp, D1 = valid real/imag data, D2 = frequency sweep complete
        {
            // D1 status reg loop
            status_register = AD5933_read(0x8F);     // read the status register
            status_register = (status_register & 0x2); // mask off the valid data bit
            if( ((status_register)| 0xFD )==  0xFF)  // valid data should be present after start freqy command
            {
                // D1 true condition
                //printf ("Status register is %u (dec) \n",status_register);       // printf function call

                if( (AD5933_read(0x8F)| 0xFB )!=  0xFF)// D2 test condition
                {

                    real_byte_high = AD5933_read(0x94);
                    real_byte_low = AD5933_read(0x95);
                    imag_byte_high = AD5933_read(0x96);
                    imag_byte_low =  AD5933_read(0x97);

                    real_data = ((real_byte_high << 8) |  real_byte_low);
                    imag_data = ((imag_byte_high << 8) |  imag_byte_low);

                    // real data

                    convert(real_data,binary);
                    printf ("original  Real register = 0x%x\n",(unsigned int)((unsigned short)real_data));       // printf function call
                    //printf ("processed Real register = %s (binary equivilant of above)\n",binary);
                    // printf ("\n%s ",binary);
                    //printf ("processed real register = %d(decimal equivilant)\n",(int)real_data);
                    //printf ("\n");

                    binary[0x0]='\0';


                    convert(imag_data,binary);
                    printf ("original  imag register = 0x%x\n",(unsigned int)((unsigned short)imag_data));       // printf function call
                    //printf ("processed imag register = %s (binary equivilant of above)\n",binary);
                    //printf ("\n%s",binary);
                    //printf ("processed imag register = %d(decimal equivilant)\n%d\r",(int)imag_data);
                    //printf ("\n");

                    binary[0x0]='\0';

                    // increment to the next frequency
                    i2c_write ( 0x80, 0x30);

                } // end of D2 test condition

                else // End of frequency sweep exit loop
                {
                    //printf ("Status register is %u (dec) \n",status_register);
                    break;
                }

            } // end of D1 true condition
        } // end of for loop


    }// end of sweep function

    void convert (short bin, char *str) // converts from 2s complement to hex.
    {
        unsigned short mask;      // used to check each individual bit, unsigned
        //    to alleviate sign extension problems

        mask = 0x8000;           // Set only the high-end bit
        while(mask)             // Loop until MASK is empty
        {
            if(bin & mask)     // test the masked bit
            *str = '1';   // if true, value is 1
            else
            *str = '0';   // if false, value is 0
            str++;              // next character
            mask >>= 1;         // shift the mask 1 bit
        }
        *str = 0;               // add the trailing null
    }



    ////////////////////////////////////////////////////////////////////////////////////////////


    /*************************************************/
    /*************************************************/
    /************	IRQ Service Routine  *************/
    /*************************************************/
    /*************************************************/

    void IRQ_Handler() __irq
    {
        

        // i2c Recieve
        if((I2C1MSTA & 0x8) == 0x8)                // Master Recieve IRQ
        {
            Receive_byte[0] = I2C1MRX;

        }
        //  wait until the user has released push button
        //	while(GP0DAT & 0x00010)	{} // increment to the next frequency

        //     I2C1ADR = 	0x1A;	 		//  device address. set i2c address(LSB = 0, Master Write) MMR 0XXFFFF081C
        //     I2C1MTX =   0x80;			//  register address.send i2c byte address	 			   MMR OXFFFF0814
        //     I2C1MTX =   0x30;			// register data. send i2c byte address	 			       MMR OXFFFF0814
        //     GP4DAT &= 0x04000000;		// Turn on led to indicate sweep point
        // 	write(output1,25);		// Output  string to hyperterminal



        return ;

    }
