
#pragma once


/*
 * Stubs
 */
class PowerManager {

public:
	static bool isExcessVoltage() { return false; };
	static bool isPowerForWork() { return true; };
	static bool isPowerForRadio() {return true; };
};
