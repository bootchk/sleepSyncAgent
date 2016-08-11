
#include "schedule.h"

/*
 * Only my limited knowledge of the 'clique'.
 * I don't know all members.
 * I don't know accurate history of masters of clique.
 */
class Clique {

public:

	static Schedule schedule;
	
	void reset();
	bool isSelfMaster();

private:
	int masterID;	// self or other unit
};
