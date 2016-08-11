
#pragma once

enum MessageType {
	Sync,
	AbandonMastership,
	Work
};

struct Message {
	MessageType type;
};
