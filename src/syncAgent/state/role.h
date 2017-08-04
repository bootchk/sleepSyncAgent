#pragma once

#include <cassert>

#include "../logging/logger.h"


enum class  Role { Merger, Fisher, Waiting };

// Master/Slave role implemented by Clique.
class MergerFisherRole {
public:
	static Role role;

	static void init() { role = Role::Fisher; }

	static bool isMerger() {return role == Role::Merger;}
	static bool isFisher() {return role == Role::Fisher;}

	static void setFisher() {
		//log("To role Fisher\n");
		assert(isMerger());
		role = Role::Fisher;
	}
	static void setMerger() {
		//log("To role Merger\n");
		assert(isFisher());
		role = Role::Merger;
	}

#ifdef FUTURE
	static bool isWorkMerger() {return role == WorkMerger;}

	static void setWorkMerger() {
			log("To role WorkMerger\n");
			role = WorkMerger;
		}
#endif
};
