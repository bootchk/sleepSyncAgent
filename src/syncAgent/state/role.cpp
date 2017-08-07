#include <cassert>

#include "role.h"


namespace {

Role _role;

}


void MergerFisherRole::init() { _role = Role::Fisher; }

Role MergerFisherRole::role() { return _role; }

bool MergerFisherRole::isMerger() {return _role == Role::Merger;}
bool MergerFisherRole::isFisher() {return _role == Role::Fisher;}

void MergerFisherRole::toFisher() {
	//log("To role Fisher\n");
	// No assertion on prior role
	_role = Role::Fisher;
}
void MergerFisherRole::toMerger() {
	//log("To role Merger\n");

	// Can only begin merging while fishing
	assert(isFisher());
	_role = Role::Merger;
}
void MergerFisherRole::toNoFishing() {
	//log("To role NoFishing\n");

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

