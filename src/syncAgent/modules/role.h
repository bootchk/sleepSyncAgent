#pragma once

#include "../../platform/platform.h"  // logging

typedef enum { Merger, WorkMerger, Fisher } RoleType;

// Just the Merger/Fisher role.  Master/Slave role implemented by Clique.
class Role {

private:
	static RoleType role;

public:
	Role() { role = Fisher; }
	static bool isMerger() {return role == Merger;}
	static bool isWorkMerger() {return role == WorkMerger;}
	static bool isFisher() {return role == Fisher;}

	static void setFisher() {
		log("To role Fisher\n");
		role = Fisher;
	}
	static void setMerger() {
		log("To role Merger\n");
		role = Merger;
	}
	static void setWorkMerger() {
			log("To role Merger\n");
			role = Merger;
		}
};
