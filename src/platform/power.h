


// TODO not portable and should be a function
#define VrefInVolts 3.6

// Returns 10 bits of Vcc in same proportion to 0x3FF as Vcc is in proportion to VrefInVolts
uint16_t getVcc();
