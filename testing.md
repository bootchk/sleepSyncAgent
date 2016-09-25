
Draft document about testing


Compile
-

there are stubs so it should compile without a platform implementation.

Mock test
-

without:
- an os to schedule
- a radio
- a second unit

create dummy messages and call a normal
sequence of SyncAgent callbacks.
syncAgent.onSyncWake();
syncAgent.onMsgReceived();
syncAgent.onSyncEnd();
// work msg
syncAgent.onWorkEnd();
etc. fish and merge

Testing with network simulator
-

Implement platform layer wrapping a network simulator.


Testing with real hardware
-

Two-unit test

Multiple unit test


