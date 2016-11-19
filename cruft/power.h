

// Optional to implement on platform depending on config.h
// See #ifdef SYNC_AGENT_CONSERVE_POWER in augment/PowerManager


// FUTURE not portable and should be a function
#define VrefInVolts 3.6

// Returns 10 bits of Vcc in same proportion to 0x3FF as Vcc is in proportion to VrefInVolts
uint16_t getVcc();
