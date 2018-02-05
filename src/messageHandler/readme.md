Each slot type handles messages differently.
A MessageHandler encapsulates that.

For example, a Fish slot is looking for Sync messages for other cliques,
and acts on them differently than Sync messages are handled in the Sync slot.