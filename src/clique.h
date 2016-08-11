
//#include "Schedule.h"
//#include "Clique.h"


class Clique {

public:
	// Compiler defaults ctor
	//SyncAgent();
	
	void reset();
	bool isSelfMaster();

private:
	//Schedule schedule;
	int masterID;
	int myID;
};
