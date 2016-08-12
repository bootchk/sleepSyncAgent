#pragma once


class MasterXmitSyncPolicy {

public:
	void reset();
	bool shouldXmitSync();

private:
	static int countDownToXmit;
	static int countDownToRollover;

};
