
/*
 * Knows policy for sending WorkSync
 *
 * Choices:
 * - only Master sends
 * - Masters and Slaves send
 *
 * Disadvantate: Slaves sending WorkSync can contend with Master sending MasterSync.
 *
 * Advantage: Masters and Slaves sending WorkSync might do more work when Master has least power of the clique.
 *
 * Send work is also contingent on app conveying work when it is ready.
 */
class WorkPolicy {
public:
	static bool shouldXmitWorkSync();
};
