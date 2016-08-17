#pragma once


class MasterXmitSyncPolicy {

public:
	void reset();
	bool shouldXmitSync();

private:
	static ScheduleCount countDownToXmit;
	static ScheduleCount countDownToRollover;

};
