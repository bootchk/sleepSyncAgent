
/*
 * Part of implementation of Clique:
 * aspects:
 * - comparison of two Cliques
 * - self's Master/Slave role in Clique
 */

#include "../../config.h"

#include "clique.h"

#include "../logging/logger.h"



namespace {

/*
 *  attribute of clique
 *  Determines:
 *  - Master/Slave role of self
 *  - hierarchy of cliques
 */

SystemID masterID;

}



/*
 * Is msg from member of my clique?
 *
 * Formerly, only the Master sent MasterSync.
 * Now, a Slave member of my clique may send a WorkSync, still identifying the Master i.e. clique
 *
 * (When a MergeSync, might be from a recent member of my clique, now a member of identified clique.)
 */
bool Clique::isMsgFromMyClique(SystemID otherMasterID){ return masterID == otherMasterID; }




/*
 * All units use same comparison.  The direction is arbitrary.
 * For testing, it may help to swap it.
 * No need for equality, no unit can hear itself.
 * my > other means: clique/unit with least numerical ID is better clique
 */
bool Clique::isOtherCliqueBetter(SystemID otherMasterID){

#ifdef LEAST_ID_IS_BETTER_CLIQUE
	return masterID > otherMasterID;
#else
	return masterID < otherMasterID;
#endif

}


SystemID Clique::getMasterID() { return masterID; }



void Clique::setSelfMastership() {
	Logger::log("set self mastership\n");
	masterID = myID();
}

/*
 * In this design MasterSync from master and WorkSync msg from slave
 * both carry the MasterID of the clique master.
 * We might be the master, and so otherID (from WorkSync slave could already be the clique master.)
 *
 * In an alternative design (Work is separate and not piggybacked on Sync)
 * then you can make the assertion:
 * assert(otherID != myID());	// we can't hear our own sync
 */
void Clique::setOtherMastership(SystemID otherID) {
	Logger::log("set other master\n");
	masterID = otherID;
}

bool Clique::isSelfMaster() { return masterID == myID(); }
