#pragma once

#include "../../platform/platform.h"  // logging

typedef enum { Merger, Fisher } RoleType;
// OBS WorkMerger

// Master/Slave role implemented by Clique.
class MergerFisherRole {

private:
	static RoleType role;

public:
	MergerFisherRole() { role = Fisher; }
	static bool isMerger() {return role == Merger;}
	static bool isFisher() {return role == Fisher;}

	static void setFisher() {
		log("To role Fisher\n");
		role = Fisher;
	}
	static void setMerger() {
		log("To role Merger\n");
		role = Merger;
	}

#ifdef FUTURE
	static bool isWorkMerger() {return role == WorkMerger;}

	static void setWorkMerger() {
			log("To role WorkMerger\n");
			role = WorkMerger;
		}
#endif
};
