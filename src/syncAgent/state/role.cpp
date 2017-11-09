#include <cassert>

#include "role.h"

#include "../logging/logger.h"

namespace {

Role _role;

}


void MergerFisherRole::init() { _role = Role::Fisher; }

Role MergerFisherRole::role() { return _role; }

bool MergerFisherRole::isMerger() {return _role == Role::Merger;}
bool MergerFisherRole::isFisher() {return _role == Role::Fisher;}

void MergerFisherRole::toFisher() {
	Logger::log(Logger::ToFisher);
	// No assertion on prior role
	_role = Role::Fisher;
}

void MergerFisherRole::toMerger() {
	Logger::log(Logger::ToMerger);

	// Can only begin merging while fishing
	assert(isFisher());
	_role = Role::Merger;
}
void MergerFisherRole::toNoFishing() {
	Logger::log(Logger::ToNoFish);

	// No assertion on prior role
	_role = Role::NoFishing;
}


#ifdef FUTURE
	static bool isWorkMerger() {return role == WorkMerger;}

	static void setWorkMerger() {
			log("To role WorkMerger\n");
			role = WorkMerger;
		}
#endif

