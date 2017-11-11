
#include <cassert>

#include <radioSoC.h>	// SystemID

#include "cliqueHistory.h"
#include "cliqueRecord.h"

#include "../logging/logger.h"


namespace {

static const unsigned int RecordCount = 6;

CliqueRecord cliqueRecords[6];

unsigned int currentCliqueIndex = 0;
}


/*
 * History starts with self as master
 */
void CliqueHistory::init() {
	cliqueRecords[0].masterID = System::ID();
	cliqueRecords[0].offsetToNextMastersSync = 0;
}


void CliqueHistory::add(SystemID newMasterID, DeltaTime offsetToNextClique) {

	if (currentCliqueIndex < (RecordCount - 1)) {
		Logger::log("Add cliq history.\n");
		currentCliqueIndex++;
		cliqueRecords[currentCliqueIndex].masterID = newMasterID;
		// TODO offset goes in previous record
		cliqueRecords[0].offsetToNextMastersSync = offsetToNextClique;
	}
	else {
		// history is flawed
		Logger::log("Exhaust cliq history.\n");
	}
	// TODO update the offset of the last CliqueRecord to skip an intermediate clique
}


void CliqueHistory::setCurrentCliqueRecordToFormerClique() {
	Logger::log("Subtract cliq history.\n");
	assert(currentCliqueIndex > 0);
	currentCliqueIndex--;
}

bool CliqueHistory::isCurrentCliqueRecordSelf() {
	return (cliqueRecords[currentCliqueIndex].masterID == System::ID());
}

SystemID CliqueHistory::currentCliqueRecordMasterID() {
	return cliqueRecords[0].masterID;
}

DeltaTime CliqueHistory::offsetToCurrentClique() {
	// TODO latencies?  Near end of sync period?
	return 1; // TODO
}
