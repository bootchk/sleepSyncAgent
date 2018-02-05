
/*
 * SyncPolicy is ABC, unimplemented
 * Subclasses are:
 * - adaptiveXmitSyncPolicy
 * - masterXmitSyncPolicy
 *
 * API
 *
 * advanceStage()
 *
 * Self has heard another unit, change (usually retard) policy.
 * (The sync message may have same MasterID as self's clique, but msg is definitely from another unit.)
 *
 * Policy is advanced even if self unit not master.
 * (If not master, doesn't use policy.  Self could still send WorkSync, but without policy.)
 * If self unit ever assumes mastership, policy will then be in advanced stage.
 */
