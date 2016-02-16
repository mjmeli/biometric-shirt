#include <stdio.h>
#include <stdlib.h>
#include <math.h>


// Usage:
//  ./freqReg <freq>
int main( int argc, char *argv[] ) {
    unsigned int clockSpeed = 16776000;
    //unsigned int clockSpeed = 16000000;
    unsigned int startFreq = atoi(argv[1]);
    long value = (startFreq / (clockSpeed / 4.0))*pow(2, 27);
    printf("%06x\n", (unsigned int)value);
    return 0;
}
