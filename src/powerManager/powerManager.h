

#pragma once

/*
 * Understands:
 * - apps power requirements
 * - systems power supply (not a battery, a capacitor.)
 *
 * Here, work takes more power than sync.
 * The work is driving a motor e.g.
 *
 * Depends on OSAL power manager (ADC, or AON Battery Monitor, etc.)
 */
class PowerManager {

public:
	bool isPowerForRadio();

private:

};
