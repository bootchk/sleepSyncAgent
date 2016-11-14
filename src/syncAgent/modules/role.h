#pragma once

#include <cassert>

#include "../../platform/platform.h"  // logging

typedef enum { Merger, Fisher } RoleType;
// OBS WorkMerger

// Master/Slave role implemented by Clique.
class MergerFisherRole {

private:
	static RoleType role;

public:
	static bool isMerger() {return role == Merger;}
	static bool isFisher() {return role == Fisher;}

	static void setFisher() {
		log("To role Fisher\n");
		assert(isMerger());
		role = Fisher;
	}
	static void setMerger() {
		log("To role Merger\n");
		assert(isFisher());
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
