#pragma once

#include <platformTypes.h>


class Controller {
public:
	/*
	 * Effectively reducing size of cliques,
	 * since reduces range of master's messages.
	 * !!! Cliques may lose and seek new masters.
	 */
	static void setXmitPower(WorkPayload);
};
