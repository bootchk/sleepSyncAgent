/*
 * Log to set of LED's.
 * Safe to log to any index, they just won't light if they don't exist.
 */
class LEDLogger {

public:
	static void init();
	static void toggleLEDs();
	static void toggleLED(int ordinal);
};
