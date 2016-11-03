
/*
 * Knows how to transmit/broadcast a Sync msg.
 *
 * Collaborates with Radio and Serializer.
 */

class SyncSender {
public:

	/*
	 * Send where some third object knows how to prefabricate SyncMessage in global outwardCommonSyncMsg
	 */
	static void sendPrefabricatedMessage() {
		// assert sender has created message in outwardCommonSyncMsg
		serializer.serializeOutwardCommonSyncMessage();
		assert(serializer.bufferIsSane());
		radio->transmitStaticSynchronously();
	}
};
