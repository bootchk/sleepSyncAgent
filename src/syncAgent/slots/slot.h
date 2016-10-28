#pragma once


class Slot {
protected:
	static void prepareRadioToTransmitOrReceive();
	static void startReceiving();
	static void stopReceiving();
	static void shutdownRadio();
};
