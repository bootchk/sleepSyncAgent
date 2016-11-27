#pragma once

#include "masterXmitSyncPolicy.h"


/*
 * Policy for xmitting sync that is adaptive:
 * transmits sync continuously in an early stage.
 * Two stages: isolated and joined.
 *
 * Isolated stage is early, when system is just starting up.
 * This condition happens when the sun comes up or lights turned on,
 * and all units undergo mcu reset.
 * In this condition, system sync was lost.
 *
 * After join another clique, or hear work from another clique, advances to joined stage.
 *
 * In joined stage, xmits sync less often.
 *
 * The purpose is to accelerate achieving sync by a factor of 2.
 * After an isolated unit joins another clique, its clique has at at least twice the members.
 * All the members are fishing, so the larger clique finds other cliques faster
 * than an isolated clique.
 * But there is also more possible happenstance contention
 * (where two cliques happen to have the same syncSlot time.)
 *
 * Wrapper around MasterXmitSyncPolicy.
 */


class AdaptiveXmitSyncPolicy {

public:
	static void reset();

	// Called every sync slot
	static bool shouldXmitSync();

	// Advance to next stage (retard frequency of xmittals.)
	static void advanceStage();
};


