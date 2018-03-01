#pragma once


/*
 * Simple flag, whether heard a work message.
 *
 * For historical reasons, not simply a variable.
 */

class WorkIn {
public:
	static void resetState();
	static void hearWork();
	static bool isHeardWork();
};

