
#include "syncSlotStep.h"

#include "../../work/workOut.h"
#include "../../syncAgentImp/state/phase.h"
#include "../../modules/syncSender.h"
#include "../../logging/logger.h"


void SyncSlotStep::sendWorkSync(){
	MailContents work = WorkOut::fetch();	// From app, WorkSyncMaintainer
	Phase::set(PhaseEnum::SyncXmitWorkSync);
	SyncSender::sendWorkSync(work);
}

void SyncSlotStep::sendSync(){
	Phase::set(PhaseEnum::SyncXmit);
	SyncSender::sendMasterSync();
}

void SyncSlotStep::sendControlSync(){
	// SyncSender accesses IntraCliqueManager for message
	SyncSender::sendControlSync();
}
