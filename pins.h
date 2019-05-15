#include <fsl_device_registers.h>

// macros used to abstract the digital_write function
#define R1 0
#define G1 1
#define B1 2

#define R2 3
#define G2 4
#define B2 5

#define A 6
#define B 7
#define C 8
#define LAT 9
#define CLK 10
#define OE 11

void config_pins_as_output(void);

void digital_write(int pin, int high);

